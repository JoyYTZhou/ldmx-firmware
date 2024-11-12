import enum
import time
import threading
import queue

from sqlalchemy import Column, Integer, BigInteger, SmallInteger, CheckConstraint, Computed
from sqlalchemy.orm import mapped_column, Mapped
from sqlalchemy.ext.hybrid import hybrid_property
from sqlalchemy.exc import SQLAlchemyError, OperationalError

import rogue

import ldmx_tdaq
import ldmx_ts

import pyrogue as pr

def with_retry(session_factory, func, *args, retries=3, delay=1):
    """
    Attempts to run the provided function with the given arguments,
    retrying on OperationalError (database is locked) up to 'retries' times.

    Parameters:
    - session_factory: SQLAlchemy session factory to create new sessions.
    - func: The function to execute within the session.
    - *args: Arguments for the function.
    - retries: Number of retries if a lock error occurs.
    - delay: Delay between retries in seconds.
    """
    for attempt in range(retries):
        try:
            with session_factory() as session:
                func(session, *args)
            break  # Exit loop if function runs successfully
        except OperationalError as e:
            if "database is locked" in str(e):
                print(f'{args}')
                print(f"Attempt {attempt + 1} failed: database is locked. Retrying in {delay} seconds...")
                time.sleep(delay)
            else:
                raise  # Raise other OperationalErrors that aren?t lock-related
    else:
        print("All retry attempts failed.")


class MsgType(enum.Enum):
    six_channel = '6_channel'
    eight_channel = '8_channel'

class RawEventDataSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'raw_event_data'

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    data: = Column(LargeBinary, nullable=False)

# Define the 'ts_raw_daq_events' table
class TsRawDaqEventSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'ts_raw_daq_event'

    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    # Uncomment this if a foreign key relationship is required
    # event_id: Mapped[int] = mapped_column(Integer, ForeignKey('events.id'), nullable=False)
    pulse_id: Mapped[int] = mapped_column(BigInteger, nullable=False)  # uint64 -> BigInteger
    bunch_count: Mapped[int] = mapped_column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    channel_count: Mapped[int] = mapped_column(SmallInteger, nullable=False)    
    lane: Mapped[int] = mapped_column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    flags: Mapped[int] = mapped_column(SmallInteger, nullable=False)
#     capId: Mapped[int] = mapped_column(SmallInteger, Computed('flags & 0x3'))
#     ce: Mapped[int] = mapped_column(SmallInteger, Computed('(flags >> 2) & 0x1'))
#     bc0: Mapped[int] = mapped_column(SmallInteger, Computed('(flags >> 3) & 0x1'))
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
    def __init__(self, dataclass, database, **kwargs):
        super().__init__(**kwargs)

        self.database = database
        self.dataclass = dataclass

        self.database.add_handler(self.dataclass, self.insertEvent)
        
    def process(self, frame):
        # Read the frame into numpy array
        rawNumpy = frame.getNumpy(0, frame.getPayload())

        # Parse the numpy array
        event = self.parseEvent(rawNumpy)

        #print(f'Put {event} in queue')

        self.database.put(self.dataclass, event)

        
            
class TsRawDaqEventSqlReceiver(SqlEventReceiver):

    def __init__(self, database, **kwargs):
        super().__init__(dataclass = 'RawDaqEvent', database = database, **kwargs)
        
        self.ts_raw_daq_event_table = TsRawDaqEventSql.__table__

        self.msg_dict = {
            'pulse_id': 0,
            'bunch_count': 0,
            'channel_count': 6,
            'lane': 0,
            'capId': 0,
            'ce': 0,
            'bc0': 0,
            'adc0': 0,
            'adc1': 0,
            'adc2': 0,
            'adc3': 0,
            'adc4': 0,
            'adc5': 0,
            'tdc0': 0,
            'tdc1': 0,
            'tdc2': 0,
            'tdc3': 0,
            'tdc4': 0,
            'tdc5': 0           
        }

    def insertEvent(self, connection, event):
        # Prepare a list to hold all dictionaries for bulk insert
        batch_data = []

        event_view = event.view(ldmx_ts.TsS30xlRawDaqEventDType)
        # Collect all rows for the batch insert
        for i in range(2):
            msg = event_view['msgs'][0][i]
            msg_data = {
                'pulse_id': event_view['header']['pulseId'][0],
                'bunch_count': event_view['header']['bunchCount'][0],
                'channel_count': 6,
                'lane': msg['lane'],
                'flags': msg['flags'],
                'adc0': msg['adc'][0],
                'adc1': msg['adc'][1],
                'adc2': msg['adc'][2],
                'adc3': msg['adc'][3],
                'adc4': msg['adc'][4],
                'adc5': msg['adc'][5],
                'tdc0': msg['tdc'][0],
                'tdc1': msg['tdc'][1],
                'tdc2': msg['tdc'][2],
                'tdc3': msg['tdc'][3],
                'tdc4': msg['tdc'][4],
                'tdc5': msg['tdc'][5]
            }
            #print(msg_data)
            batch_data.append(msg_data)

        return (self.ts_raw_daq_event_table, batch_data)
        # Execute a bulk insert with all collected rows
        if batch_data:
            connection.execute(self.ts_raw_daq_event_table.insert(), batch_data)
        
    def parseEvent(self, rawNumpy):
        #return rawNumpy.view(ldmx_ts.TsS30xlRawDaqEventDType)
        #return ldmx_ts.TsRawDaqEvent.from_numpy(rawNumpy)
        return rawNumpy
        

class TsS30xlThresholdTriggerEventSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'ts_s30xl_threshold_trigger_event'

    id = Column(Integer, primary_key=True, autoincrement=True)
    pulse_id = Column(BigInteger, nullable=False) # uint64 -> BigInteger
    bunch_count = Column(SmallInteger, nullable=False) # uint8 -> SmallInteger
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
        super().__init__(dataclass = ldmx_ts.TsS30xlThresholdTriggerEvent, database = database, **kwargs)

        self.table = TsS30xlThresholdTriggerEventSql.__table__

        self.table_dict = {
            'pulse_id': 0,
            'bunch_count': 0,
            'hits': 0,
            'amplitude0': 0,
            'amplitude1': 0,
            'amplitude2': 0,
            'amplitude3': 0,
            'amplitude4': 0,
            'amplitude5': 0,
            'amplitude6': 0,
            'amplitude7': 0,
            'amplitude8': 0,
            'amplitude9': 0,
            'amplitude10': 0,
            'amplitude11': 0}

    def insertEvent(self, connection, event):
        self.table_dict['pulse_id'] = event.header.pulseId
        self.table_dict['bunch_count'] = event.header.bunchCount
        self.table_dict['hits'] = event.hits
        self.table_dict['amplitude0'] = event.amplitudes[0]
        self.table_dict['amplitude1'] =  event.amplitudes[1]
        self.table_dict['amplitude2'] =  event.amplitudes[2]
        self.table_dict['amplitude3'] =  event.amplitudes[3]
        self.table_dict['amplitude4'] =  event.amplitudes[4]
        self.table_dict['amplitude5'] =  event.amplitudes[5]
        self.table_dict['amplitude6'] =  event.amplitudes[6]
        self.table_dict['amplitude7'] =  event.amplitudes[7]
        self.table_dict['amplitude8'] =  event.amplitudes[8]
        self.table_dict['amplitude9'] =  event.amplitudes[9]
        self.table_dict['amplitude10'] = event.amplitudes[10]
        self.table_dict['amplitude11'] = event.amplitudes[11]

        return (self.table, [self.table_dict])

        connection.execute(self.table.insert(), [self.table_dict])
        

    def parseEvent(self, rawNumpy):
        # Parse the numpy array
        return ldmx_ts.TsS30xlThresholdTriggerEvent.from_numpy(rawNumpy)

