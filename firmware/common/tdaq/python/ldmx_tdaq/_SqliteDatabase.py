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
        
        self._handlers = {}
        self._queues = {}
        self._queues_lock = threading.Lock()
        self._thread = threading.Thread(target=self._worker)
        self._thread.start()
        
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
        self._queues[dataclass] = queue.Queue()

    def put(self, typ, event):
        self._queues[typ].put(event)

    def _stop(self):
        with self._queues_lock:
#             # Check if queues have any events to process
#             if self._queues and any(not q.empty() for q in self._queues.values()):
#                 # Wait until all queues are empty
#                 while any(not q.empty() for q in self._queues.values()):
#                     for q in self._queues.values():
#                         print(f'q.empty= {q.empty()}')
#                     print('Waiting for SQL Receiver to finish processing queues')
#                     time.sleep(0.1)  # Short sleep to avoid busy-waiting

            # Insert a `None` into each remaining queue to signal the worker to stop
            for q in self._queues.values():
                print('Stop with None')
                q.put(None)

        # Wait for the worker thread to finish
        self._thread.join()
        print('SQL Receiver finished')

    def _worker(self):
        while True:
            # Acquire lock to safely check and access self._queues
            with self._queues_lock:

                # Process each queue directly from active_queues
                for event_type, q in self._queues.items():
                    if q.empty():
                        continue
                    else:
                        event = q.get()

                    # If a None event is received return so the thread can terminate
                    if event is None:
                        print('Got None Event')
                        return

                    # Only proceed if the database engine is available
                    if not self._engine:
                        continue

                    try:
                        # Start a new transaction for the current event type's queue
                        print('Got Event, opening connection')
                        with self._engine.begin() as connection:
                            count = 0
                            start_time = time.time()
                            batch_data = []
                            handler = self._handlers.get(event_type)                            

                            # Process all events for this specific event type
                            while event is not None:
                                table, bd = handler(connection, event)  # Handle the event (e.g., insert into the table)
                                count += 1

                                batch_data.extend(bd)

                                if q.empty():
                                    event = None
                                else:
                                    event = q.get()

                            connection.execute(table.insert(), batch_data)

                        # Log the transaction results
                        duration = time.time() - start_time
                        print(f'Committed {count} events of type {event_type} in {duration:.4f} seconds = {count/duration:.4f} events/second')

                    except Exception as e:
                        print(f"Error processing queue for event type {event_type}: {e}")
                        raise e

            # Short sleep to reduce CPU usage if no events are available
            time.sleep(0.01)
