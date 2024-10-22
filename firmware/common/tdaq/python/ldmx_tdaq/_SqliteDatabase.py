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
import sqlalchemy.ext.declarative
import sqlalchemy.orm
import threading
import queue
import json



class SqliteDatabase(pr.Device):

    SqliteBase = sqlalchemy.ext.declarative.declarative_base()    

    def __init__(self, *, url='sqlite:///test.db'):
        super().__init__()
        
        self._log = pr.logInit(cls=self, name="SqliteFileWriter", path=None)
        self._url = url
        self._engine = sqlalchemy.create_engine(url)

        self.SqliteBase.metadata.create_all(self._engine)

        self.SessionFactory = sqlalchemy.orm.sessionmaker(bind=self._engine)

        self.tables = {}

    def addStream(self, table_name, stream):
        self.tables[table_name] = stream

        

