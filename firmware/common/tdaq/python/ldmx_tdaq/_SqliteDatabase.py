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

    def __init__(self, *, url='sqlite:////dev/shm/bareese/test.db'):
        super().__init__()
        
        self._log = pr.logInit(cls=self, name="SqliteFileWriter", path=None)
        self._url = url
        self._engine = sqlalchemy.create_engine(url, connect_args={"check_same_thread": False})
        
        self.queue = queue.Queue()
        self._thread = threading.Thread(target=self._worker)
        self._thread.start()
        self._handlers = {}
        
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
                localGet = lambda: self.table_insert_counts[table]))

        @self.command()
        def ResetWriteCounts():
            with self.lock:
                self._insert_count = 0
                for k in self.table_insert_counts:
                    self.table_insert_counts[k] = 0
            

    def count_writes(self, conn, clauseelement, multiparams, params):
        #print('Called count_writes')
        if isinstance(clauseelement, Insert):
            table_name = clauseelement.table.name
            row_count = len(multiparams[0]) if multiparams else 1  # Use 1 as fallback for single inserts
            with self.lock:
                self._insert_count += row_count
                self.table_insert_counts[table_name] = self.table_insert_counts[table_name] + row_count

    def add_handler(self, dataclass, handler):
        self._handlers[dataclass] = handler

    def _stop(self):
        if not self.queue.empty():
            print('Waiting for SQL Receiver to finish')
        self.queue.put(None)
        self._thread.join()
        print('SQL Receiver finished')   
        

    def _worker(self):
        while True:
            # Block and wait for a queue entry to arrive
            event = self.queue.get()

            # Exit thread if a None entry is received
            if event is None:
                return

            # Continue only if the database connection is present
            if not self._engine:
                continue

            try:
                with self._engine.begin() as connection:
                    count = 0
                    start_time = time.time()
                    while event is not None:
                        #print(f'Read {event} from queue')
                        handler = self._handlers[event.__class__]
                        # Insert the event into the database
                        handler(connection, event)
                        count = count + 1

                        # If the queue is empty, commit the transaction and break
                        if self.queue.empty():
                            duration = time.time() - start_time
                            print(f'Processed {count} events in {duration:4f} seconds = {count/duration:4f} events/second')
                            start_time = time.time()                            
                            break

                        # Get the next event from the queue
                        event = self.queue.get()

                    # Transaction auto-commits here when exiting wth with block
                        

                duration = time.time() - start_time
                print(f'Added {count} events to the database in {duration:4f} seconds = {count/duration:4f} events/second')


            except Exception as e:
                print(e)
