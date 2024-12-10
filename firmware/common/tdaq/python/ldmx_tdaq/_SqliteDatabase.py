#-----------------------------------------------------------------------------
# Company    : SLAC National Accelerator Laboratory
#-----------------------------------------------------------------------------
#  Description:
#       PyRogue - SQL Logging Module
#-----------------------------------------------------------------------------
# This file is part of the rogue software platform. It is subject to
# the license terms in the LICENSE.txt file found in the top-level directory
# of this distribution and at:
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
# No part of the rogue software platform, including this file, may be
# copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------

import pyrogue as pr
import sqlalchemy
from sqlalchemy.sql import Insert
import sqlalchemy.ext.declarative
import sqlalchemy.orm
import threading
import queue
import json
from collections import defaultdict
import time



class SqliteDatabase(pr.DataWriter):

    SqliteBase = sqlalchemy.ext.declarative.declarative_base()    

    def __init__(self, *, url='sqlite:////u1/bareese/test.db', **kwargs):
        super().__init__(**kwargs)
        
        self._log = pr.logInit(cls=self, name="SqliteFileWriter", path=None)
        self._url = url
        self._engine = None
        self.SessionFactory = None
        
        self.queue = queue.Queue()
        self._thread = threading.Thread(target=self._worker)
        self._thread.start()

        self.add(pr.LocalVariable(
            name = 'DatabaseUrl',
            mode = 'RO',
            value = url))

        self._insert_count = 0

        self.add(pr.LocalVariable(
            name = 'RowWrites',
            mode = 'RO',
            pollInterval = 1,
            localGet = lambda: self._insert_count))

        # Lock for insert_counts
        self.lock = threading.Lock()
        self.table_insert_counts = defaultdict(int)

        table_names = self.SqliteBase.metadata.tables.keys()        
        for table in table_names:
            self.table_insert_counts[table] = 0;
            self.add(pr.LocalVariable(
                name = f'{table}_inserts',
                mode = 'RO',
                pollInterval = 1,
                localGet = lambda t=table: self.table_insert_counts[t]))


        @self.command()
        def ResetWriteCounts():
            with self.lock:
                self._insert_count = 0
                for k in self.table_insert_counts:
                    self.table_insert_counts[k] = 0
                    
    def _open(self):
#         datafile = self.DataFile.value()
#         abs_path = os.path.abs_path(datafile)
#         url_path = quote(abs_path)
#         url = f'sqlite:///{url}'
        url = self.DatabaseUrl.value()
        self._engine = sqlalchemy.create_engine(url, connect_args={"check_same_thread": False})
        print(f'Opened engine {self._engine}')
        self.SqliteBase.metadata.create_all(self._engine)        
        sqlalchemy.event.listen(self._engine, 'before_execute', self._count_writes)
        self.SessionFactory = sqlalchemy.orm.sessionmaker(bind=self._engine)
        
        self.DatabaseUrl.set(url)
        self.IsOpen.get()

    def _close(self):
        if self._engine:
            self._engine.dispose()
            self._engine = None
        self.IsOpen.get()

    def _isOpen(self):
        return self._engine is not None

    def _stop(self):
        self.queue.put(None)
        self._thread.join()
        print('SQL Receiver finished')
        self._close()

        
    def _count_writes(self, conn, clauseelement, multiparams, params):
        # Check if this operation is an insert
        if isinstance(clauseelement, Insert):
            table_name = clauseelement.table.name

            # Calculate the row count based on the structure of multiparams
            row_count = len(multiparams) if multiparams else 1  # `len(multiparams)` gives the correct number of rows for bulk inserts

            # Update the counts with thread-safety
            with self.lock:
                self._insert_count += row_count
                self.table_insert_counts[table_name] = self.table_insert_counts.get(table_name, 0) + row_count
                
    def put(self, parser, data):
        #print(f'database.put({parser}, {data})')
        self.queue.put((parser, data))

    def execute(self, stmt):
        if not self._engine:
            print('No engine for statement')
            return None

        with self._engine.begin() as connection:
            result = connection.execute(stmt)

        return result


    def _worker(self):
        while True:
            insert_dict = defaultdict(list)
            count = 0
            start_time = time.time()
            loop_start_time = start_time


            # Process events until the queue is empty or None is encountered
            while True:
                entry = self.queue.get()

                #print(f'Got an entry - {entry}')

                # Check for None entry to signal exit
                if entry is None:
                    # If None is encountered, break to process the current batch and then exit
                    break

                # Continue only if the database connection is present
                if not self._engine:
                    #print('Got entry but engine is None')
                    continue

                # Process the data using its parser and add the result to the insert_dict
                # Parser returns a dict of {'table':[{row_dict}]}
                # Append these new entries to the running insert_dict
                parser, data = entry
                entry_inserts = parser(data)
                #print(f'{entry_inserts=}')
                for table, inserts in entry_inserts.items():
                    insert_dict[table].extend(inserts)
                count += 1
                #print(f'{insert_dict=}')

                # If the queue is empty, break to process the batch and continue
                if self.queue.empty():
                    break

            # Insert all entries accumulated in insert_dict into the database
            # Do this table by table for optimal efficiency
            if len(insert_dict) > 0:
                try:
                    with self._engine.begin() as connection:
                        for table, batch_data in insert_dict.items():
                            #print(table, batch_data)
                            connection.execute(table.insert(), batch_data)

                    end_time = time.time()
                    #duration = end_time-start_time
                    full_duration = end_time-loop_start_time
                    #print(f'Added {count} entrys to the database in {duration:.4f} seconds = {count/duration:.4f} entrys/second')
                    print(f'Processed and inserted {count} entrys from queue {full_duration:.4f} seconds = {count/full_duration:.4f} entrys/second')

                except Exception as e:
                    print(f"Error inserting into database: {e}")
                    raise e

            # Exit the loop if None was received, indicating the end of processing
            if entry is None:
                print("Exiting worker due to None entry.")
                break   
