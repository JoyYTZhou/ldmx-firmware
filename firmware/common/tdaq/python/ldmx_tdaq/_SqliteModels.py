import enum
import time
import threading
import queue

from sqlalchemy import Column, Integer, BigInteger, SmallInteger, CheckConstraint, Computed, BLOB, LargeBinary
from sqlalchemy.orm import mapped_column, Mapped
from sqlalchemy.ext.hybrid import hybrid_property
from sqlalchemy.exc import SQLAlchemyError, OperationalError

import rogue

import ldmx_tdaq
import ldmx_ts

import pyrogue as pr

class RawEventDataSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'raw_event_data'

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    data = Column(LargeBinary, nullable=False)

# Define the 'ts_raw_daq_events' table
class LclsTimingDaqEventSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'lcls_timing_event_data'

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    ldmx_timestamp: Mapped[int] = mapped_column(BigInteger, nullable=False)
    ldmx_pulseId: Mapped[int] = mapped_column(BigInteger, Computed('ldmx_timestamp / 8'))
    ldmx_bunch_count: Mapped[int] = mapped_column(SmallInteger, Computed('ldmx_timestamp & 0x3F'))
    lcls_pulse_id: Mapped[int] = mapped_column(BigInteger, nullable=False)
    lcls_timestamp: Mapped[int] = mapped_column(BigInteger, nullable=False)
    fixed_rates: Mapped[int] = mapped_column(Integer, nullable=False)
    control3: Mapped[int] = mapped_column(Integer, nullable=False)


class SqlEventReceiver(pr.DataReceiver):
    def __init__(self, database, table=None, **kwargs):
        super().__init__(**kwargs)

        self.database = database
        self.table = table

#         self.database.add_parser(self.table, self.parser)

    def parser(self, data):
        #print('parser()')
        # Default parser 
        return {self.table: [{'data':data}]}
        
    def process(self, frame):
        #print('process()')
        # Read the frame into numpy array
        ba = frame.getBa()

        self.database.put(self.parser, ba)

        
            
class LclsTimingEventSqlReceiver(SqlEventReceiver):

    def __init__(self, database, **kwargs):
        super().__init__(table = LclsTimingDaqEventSql.__table__ , database = database, **kwargs)
        
    def parser(self, data):
        # Prepare a list to hold all dictionaries for bulk insert
        event_view = data.view(ldmx_tdaq.LclsTimingDaqEventDType)
#        print(f'Parsing Raw Event - {event_view}')        
        # Collect all rows for the batch insert
        msg_data = [{
            'ldmx_timestamp': int(event_view['header']['timestamp'][0]),
            'lcls_pulse_id': int(event_view['msg']['pulseId'][0]),
            'lcls_timestamp': int(event_view['msg']['timeStamp'][0]),
            'fixed_rates': int(event_view['msg']['fixedRates'][0]),
            'control3': int(event_view['msg']['control3'][0])}]

        return {self.table: msg_data}

    def process(self, frame):
        rawNumpy = frame.getNumpy()
        print(f'Got LclsTimingEvent frame with size {len(rawNumpy)}')
        ldmx_tdaq.print_custom_np_type(rawNumpy[0:16].view(ldmx_tdaq.EventHeaderDType))
        if len(rawNumpy) != 48:
            print(f'\nIncorrect LCLS Timing Frame size {len(rawNumpy)}\n')
            print(rawNumpy[16:])
            print('\n\n\n')
            return


        #print(f'Process Raw Event - {rawNumpy.view(ldmx_ts.TsS30xlRawDaqEventDType)}')
        ldmx_tdaq.print_custom_np_type(rawNumpy[16:].view(ldmx_tdaq.LclsTimingMsgDType))
        print(rawNumpy[16:])        
        self.database.put(self.parser, rawNumpy)
        #return rawNumpy.view(ldmx_ts.TsS30xlRawDaqEventDType)
        #return ldmx_ts.TsRawDaqEvent.from_numpy(rawNumpy)
        #return rawNumpy
        
