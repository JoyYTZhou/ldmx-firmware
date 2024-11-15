import enum
import time
import threading
import queue

from sqlalchemy import Column, Integer, BigInteger, SmallInteger, CheckConstraint, Computed, BLOB
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
    data = Column(BLOB, nullable=False)

# Define the 'ts_raw_daq_events' table
class TsRawDaqEventSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'ts_raw_daq_event'

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    timestamp: Mapped[int] = mapped_column(BigInteger, nullable=False)
    pulse_id: Mapped[int] = mapped_column(BigInteger, Computed('timestamp / 8'))
    bunch_count: Mapped[int] = mapped_column(SmallInteger, Computed('timestamp & 0x3F'))
    channel_count: Mapped[int] = mapped_column(SmallInteger, nullable=False)    
    lane: Mapped[int] = mapped_column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    flags: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    capId: Mapped[int] = mapped_column(SmallInteger, Computed('flags & 0x3'))
    ce: Mapped[int] = mapped_column(SmallInteger, Computed('(flags / 4) & 0x1'))
    bc0: Mapped[int] = mapped_column(SmallInteger, Computed('(flags / 8) & 0x1'))
    adc0: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    adc1: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    adc2: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    adc3: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    adc4: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    adc5: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    adc6: Mapped[int] = mapped_column(SmallInteger, nullable=True)
    adc7: Mapped[int] = mapped_column(SmallInteger, nullable=True)
    tdc0: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    tdc1: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    tdc2: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    tdc3: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    tdc4: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    tdc5: Mapped[int] = mapped_column(SmallInteger, nullable=False)
    tdc6: Mapped[int] = mapped_column(SmallInteger, nullable=True)
    tdc7: Mapped[int] = mapped_column(SmallInteger, nullable=True)    

    # Corrected CheckConstraint for allowed values of channel_count
    __table_args__ = (
        CheckConstraint('channel_count = 6 OR channel_count = 8', name='check_channel_count'),
    )    

    @hybrid_property
    def adc(self):
        if self.channel_count == 6:
            return [self.adc0, self.adc1, self.adc2, self.adc3, self.adc4, self.adc5]
        elif self.channel_count == 8:
            return [self.adc0, self.adc1, self.adc2, self.adc3, self.adc4, self.adc5, self.adc6, self.adc7]

    @adc.setter
    def adc(self, values):
        self.adc0, self.adc1, self.adc2, self.adc3, self.adc4, self.adc5 = values[:6]
        if self.channel_count == 8:
            self.adc6, self.adc7 = values[6:8]
        else:
            self.adc6, self.adc7 = None, None  # Set to None for 6-channel events

    @hybrid_property
    def tdc(self):
        if self.channel_count == 6:
            return [self.tdc0, self.tdc1, self.tdc2, self.tdc3, self.tdc4, self.tdc5]
        elif self.channel_count == 8:
            return [self.tdc0, self.tdc1, self.tdc2, self.tdc3, self.tdc4, self.tdc5, self.tdc6, self.tdc7]

    @tdc.setter
    def tdc(self, values):
        self.tdc0, self.tdc1, self.tdc2, self.tdc3, self.tdc4, self.tdc5 = values[:6]
        if self.channel_count == 8:
            self.tdc6, self.tdc7 = values[6:8]
        else:
            self.tdc6, self.tdc7 = None, None  # Set to None for 6-channel events
            

class SqlEventReceiver(pr.DataReceiver):
    def __init__(self, table, database, **kwargs):
        super().__init__(**kwargs)

        self.database = database
        self.table = table

#         self.database.add_parser(self.table, self.parser)

    def parser(self, data):
        # Default parser 
        return [{'data': data}]
        
    def process(self, frame):
        # Read the frame into numpy array
        ba = frame.getBa()

        self.database.put(self.parser, ba)

        
            
class TsRawDaqEventSqlReceiver(SqlEventReceiver):

    def __init__(self, database, **kwargs):
        super().__init__(table = TsRawDaqEventSql.__table__ , database = database, **kwargs)
        
    def parser(self, data):
        # Prepare a list to hold all dictionaries for bulk insert
        batch_data = []

        event_view = data.view(ldmx_ts.TsS30xlRawDaqEventDType)
        # Collect all rows for the batch insert
        for i in range(2):
            msg = event_view['msgs'][0][i]
            msg_data = {
                'timestamp': int(event_view['header']['timestamp'][0]),
                'channel_count': 6,
                'lane': int(msg['lane']),
                'flags': int(msg['flags']),
                'adc0': int(msg['adc'][0]),
                'adc1': int(msg['adc'][1]),
                'adc2': int(msg['adc'][2]),
                'adc3': int(msg['adc'][3]),
                'adc4': int(msg['adc'][4]),
                'adc5': int(msg['adc'][5]),
                'tdc0': int(msg['tdc'][0]),
                'tdc1': int(msg['tdc'][1]),
                'tdc2': int(msg['tdc'][2]),
                'tdc3': int(msg['tdc'][3]),
                'tdc4': int(msg['tdc'][4]),
                'tdc5': int(msg['tdc'][5])
            }
            #print(msg_data)
            batch_data.append(msg_data)

        return {self.table: batch_data}

    def process(self, frame):
        self.database.put(self.parser, frame.getNumpy())
        #return rawNumpy.view(ldmx_ts.TsS30xlRawDaqEventDType)
        #return ldmx_ts.TsRawDaqEvent.from_numpy(rawNumpy)
        #return rawNumpy
        

class TsS30xlThresholdTriggerEventSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'ts_s30xl_threshold_trigger_event'

    id = Column(Integer, primary_key=True, autoincrement=True)
    timestamp: Mapped[int] = mapped_column(BigInteger, nullable=False)
    pulse_id: Mapped[int] = mapped_column(BigInteger, Computed('timestamp / 8'))
    bunch_count: Mapped[int] = mapped_column(SmallInteger, Computed('timestamp & 0x3F'))
    hits = Column(Integer, nullable=False)
    amplitude0 = Column(Integer, nullable=False)
    amplitude1 = Column(Integer, nullable=False)
    amplitude2 = Column(Integer, nullable=False)
    amplitude3 = Column(Integer, nullable=False)
    amplitude4 = Column(Integer, nullable=False)
    amplitude5 = Column(Integer, nullable=False)
    amplitude6 = Column(Integer, nullable=False)
    amplitude7 = Column(Integer, nullable=False)
    amplitude8 = Column(Integer, nullable=False)
    amplitude9 = Column(Integer, nullable=False)
    amplitude10 = Column(Integer, nullable=False)
    amplitude11 = Column(Integer, nullable=False)    

    @hybrid_property
    def amplitudes(self):
        return [getattr(self, f'amplitude{i}') for i in range(12)]

    @amplitudes.setter
    def amplitudes(self, values):
        for i in range(12):
            setattr(self, f'amplitude{i}', values[i])


class TsS30xlThresholdTriggerEventSqlReceiver(SqlEventReceiver):

    def __init__(self, database, **kwargs):
        super().__init__(table = TsS30xlThresholdTriggerEventSql.__table__, database = database, **kwargs)

    def parser(self, data):
        event = ldmx_ts.TsS30xlThresholdTriggerEvent.from_numpy(data)
        table_dict = {
            'timestamp': event.header.timestamp,
            'hits': event.hits,
            'amplitude0': event.amplitudes[0],
            'amplitude1':  event.amplitudes[1],
            'amplitude2':  event.amplitudes[2],
            'amplitude3':  event.amplitudes[3],
            'amplitude4':  event.amplitudes[4],
            'amplitude5':  event.amplitudes[5],
            'amplitude6':  event.amplitudes[6],
            'amplitude7':  event.amplitudes[7],
            'amplitude8':  event.amplitudes[8],
            'amplitude9':  event.amplitudes[9],
            'amplitude10': event.amplitudes[10],
            'amplitude11': event.amplitudes[11]}

        return {self.table: [table_dict]}


    def process(self, frame):
        self.database.put(self.parser, frame.getNumpy())

