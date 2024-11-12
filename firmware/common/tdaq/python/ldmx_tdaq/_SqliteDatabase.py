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



class SqliteDatabase(pr.Device):

    SqliteBase = sqlalchemy.ext.declarative.declarative_base()    

    def __init__(self, *, url='sqlite:////u1/bareese/test.db'):
        super().__init__()
        
        self._log = pr.logInit(cls=self, name="SqliteFileWriter", path=None)
        self._url = url
        self._engine = sqlalchemy.create_engine(url, connect_args={"check_same_thread": False})
        
        self.queue = queue.Queue()
        self._thread = threading.Thread(target=self._worker)
        self._thread.start()
        self._parsers = {}
        
        self.SqliteBase.metadata.create_all(self._engine)

        self.SessionFactory = sqlalchemy.orm.sessionmaker(bind=self._engine)

        self._insert_count = 0

        self.add(pr.LocalVariable(
            name = 'RowWrites',
            mode = 'RO',
            pollInterval = 1,
            localGet = lambda: self._insert_count))

        sqlalchemy.event.listen(self._engine, 'before_execute', self.count_writes)


        inspector = sqlalchemy.inspect(self._engine)
        table_names = inspector.get_table_names()

        self.lock = threading.Lock()
        self.table_insert_counts = defaultdict(int)

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
            

    def count_writes(self, conn, clauseelement, multiparams, params):
        # Check if this operation is an insert
        if isinstance(clauseelement, Insert):
            table_name = clauseelement.table.name

            # Calculate the row count based on the structure of multiparams
            row_count = len(multiparams) if multiparams else 1  # `len(multiparams)` gives the correct number of rows for bulk inserts

            # Update the counts with thread-safety
            with self.lock:
                self._insert_count += row_count
                self.table_insert_counts[table_name] = self.table_insert_counts.get(table_name, 0) + row_count
                

    def add_parser(self, table, parser):
        self._parsers[table] = parser

    def put(self, table, data):
        self.queue.put((table, data))

    def _stop(self):
        self.queue.put(None)
        self._thread.join()
        print('SQL Receiver finished')   
        

    def _worker(self):
        while True:
            insert_dict = defaultdict(list)
            count = 0
            start_time = time.time()
            loop_start_time = start_time

            # Process events until the queue is empty or None is encountered
            while True:
                event = self.queue.get()

                # Check for None event to signal exit
                if event is None:
                    # If None is encountered, break to process the current batch and then exit
                    break

                # Continue only if the database connection is present
                if not self._engine:
                    continue

                # Process the event using its parser and add the result to the insert_dict
                parser = self._parsers[event[0]]
                insert_dict[event[0]].extend(parser(event[1]))
                count += 1

                # If the queue is empty, break to process the batch and continue
                if self.queue.empty():
                    break

            #duration = time.time() - start_time
            #print(f'Processed {count} queue entries in {duration:.4f} seconds = {count/duration:.4f} events/second')
            #start_time = time.time()
            # Insert all entries accumulated in insert_dict into the database
            try:
                with self._engine.begin() as connection:
                    for table, batch_data in insert_dict.items():
                        #print(table, batch_data)
                        connection.execute(table.insert(), batch_data)

                end_time = time.time()
                #duration = end_time-start_time
                full_duration = end_time-loop_start_time
                #print(f'Added {count} events to the database in {duration:.4f} seconds = {count/duration:.4f} events/second')
                print(f'Total {count} events done in {full_duration:.4f} seconds = {count/full_duration:.4f} events/second')

            except Exception as e:
                print(f"Error inserting into database: {e}")
                raise e

            # Exit the loop if None was received, indicating the end of processing
            if event is None:
                print("Exiting worker due to None event.")
                break   
