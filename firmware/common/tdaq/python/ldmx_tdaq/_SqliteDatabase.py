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



class SqliteDatabase(pr.Device):

    SqliteBase = sqlalchemy.ext.declarative.declarative_base()    

    def __init__(self, *, url='sqlite:////u1/bareese/test.db'):
        super().__init__()
        
        self._log = pr.logInit(cls=self, name="SqliteFileWriter", path=None)
        self._url = url
        self._engine = sqlalchemy.create_engine(url)

        self.SqliteBase.metadata.create_all(self._engine)

        self.SessionFactory = sqlalchemy.orm.sessionmaker(bind=self._engine)

        self.tables = {}

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
            with self.lock:
                self._insert_count += 1
                self.table_insert_counts[table_name] = self.table_insert_counts[table_name] + 1

    def addStream(self, table_name, stream):
        self.tables[table_name] = stream

        

