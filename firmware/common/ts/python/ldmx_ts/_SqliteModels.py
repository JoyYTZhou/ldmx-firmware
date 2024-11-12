import enum
import time
import threading
import queue

from sqlalchemy import Column, Integer, BigInteger, SmallInteger, CheckConstraint
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

# Define the 'ts_raw_daq_events' table
class TsRawDaqEventSql(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'ts_raw_daq_event'

    id = Column(Integer, primary_key=True, autoincrement=True)
#    event_id = Column(Integer, ForeignKey('events.id'), nullable=False)
    pulse_id = Column(BigInteger, nullable=False) # uint64 -> BigInteger
    bunch_count = Column(SmallInteger, nullable=False) # uint8 -> SmallInteger
    channel_count = Column(SmallInteger, nullable=False)    
    lane = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    capId = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    ce = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    bc0 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    adc0 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    adc1 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    adc2 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    adc3 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    adc4 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger
    adc5 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger
    adc6 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger
    adc7 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger    
    tdc0 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    tdc1 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    tdc2 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    tdc3 = Column(SmallInteger, nullable=False)  # uint8 -> SmallInteger
    tdc4 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger
    tdc5 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger
    tdc6 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger
    tdc7 = Column(SmallInteger, nullable=True)  # uint8 -> SmallInteger    

    # Add a CheckConstraint to restrict the allowed values for channel_count
    __table_args__ = (
        CheckConstraint(channel_count.in_([6, 8]), name='check_channel_count'),
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

        self.database.put(event)

        
            
class TsRawDaqEventSqlReceiver(SqlEventReceiver):

    def __init__(self, database, **kwargs):
        super().__init__(dataclass = ldmx_ts.TsRawDaqEvent, database = database, **kwargs)
        
        self.ts_raw_daq_event_table = TsRawDaqEventSql.__table__

#         sqlalchemy.Table(
#             'ts_raw_daq_event', sqlalchemy.MetaData(),
#             Column('id', Integer, primary_key=True, autoincrement=True),
#             # Column('event_id', Integer, ForeignKey('events.id'), nullable=False),  # Uncomment if using a foreign key
#             Column('pulse_id', BigInteger, nullable=False),
#             Column('bunch_count', SmallInteger, nullable=False),
#             Column('channel_count', SmallInteger, nullable=False),
#             Column('lane', SmallInteger, nullable=False),
#             Column('capId', SmallInteger, nullable=False),
#             Column('ce', SmallInteger, nullable=False),
#             Column('bc0', SmallInteger, nullable=False),
#             Column('adc0', SmallInteger, nullable=False),
#             Column('adc1', SmallInteger, nullable=False),
#             Column('adc2', SmallInteger, nullable=False),
#             Column('adc3', SmallInteger, nullable=False),
#             Column('adc4', SmallInteger, nullable=False),
#             Column('adc5', SmallInteger, nullable=False),
#             Column('adc6', SmallInteger, nullable=True),
#             Column('adc7', SmallInteger, nullable=True),
#             Column('tdc0', SmallInteger, nullable=False),
#             Column('tdc1', SmallInteger, nullable=False),
#             Column('tdc2', SmallInteger, nullable=False),
#             Column('tdc3', SmallInteger, nullable=False),
#             Column('tdc4', SmallInteger, nullable=False),
#             Column('tdc5', SmallInteger, nullable=False),
#             Column('tdc6', SmallInteger, nullable=True),
#             Column('tdc7', SmallInteger, nullable=True),
#             CheckConstraint('channel_count IN (6, 8)', name='check_channel_count')
#         )

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
    
        # Collect all rows for the batch insert
        for msg in event.msgs:
            msg_data = {
                'pulse_id': event.header.pulseId,
                'bunch_count': event.header.bunchCount,
                'channel_count': 6,
                'lane': msg.lane,
                'capId': msg.capId,
                'ce': msg.ce,
                'bc0': msg.bc0,
                'adc0': msg.adc[0],
                'adc1': msg.adc[1],
                'adc2': msg.adc[2],
                'adc3': msg.adc[3],
                'adc4': msg.adc[4],
                'adc5': msg.adc[5],
                'tdc0': msg.tdc[0],
                'tdc1': msg.tdc[1],
                'tdc2': msg.tdc[2],
                'tdc3': msg.tdc[3],
                'tdc4': msg.tdc[4],
                'tdc5': msg.tdc[5]
            }
            batch_data.append(msg_data)

            # Execute a bulk insert with all collected rows
            if batch_data:
                connection.execute(self.ts_raw_daq_event_table.insert(), batch_data)
        
#         for i, msg in enumerate(event.msgs):
#             self.msg_dict['pulse_id']  = event.header.pulseId
#             self.msg_dict['bunch_count'] = event.header.bunchCount
#             self.msg_dict['lane'] = msg.lane
#             self.msg_dict['capId']= msg.capId
#             self.msg_dict['ce'] =msg.ce
#             self.msg_dict['bc0']=msg.bc0
#             self.msg_dict['adc0'] =msg.adc[0]
#             self.msg_dict['adc1'] =msg.adc[1]
#             self.msg_dict['adc2'] =msg.adc[2]
#             self.msg_dict['adc3'] =msg.adc[3]
#             self.msg_dict['adc4'] =msg.adc[4]
#             self.msg_dict['adc5'] =msg.adc[5]
#             self.msg_dict['tdc0'] =msg.tdc[0]
#             self.msg_dict['tdc1'] =msg.tdc[1]
#             self.msg_dict['tdc2'] =msg.tdc[2]
#             self.msg_dict['tdc3'] =msg.tdc[3]
#             self.msg_dict['tdc4'] =msg.tdc[4]
#             self.msg_dict['tdc5'] =msg.tdc[5]

#             connection.execute(self.ts_raw_daq_event_table.insert(), [self.msg_dict])

                    
            
            #print(f'Writing msg into database - {msg}')
#             sqlEvent = TsRawDaqEventSql(
#                 pulse_id = event.header.pulseId,
#                 bunch_count = event.header.bunchCount,
#                 channel_count = 6,
#                 lane = msg.lane,
#                 capId = msg.capId,
#                 ce = msg.ce,
#                 bc0 = msg.bc0,
#                 adc = msg.adc,
#                 tdc = msg.tdc)

#             session.add(sqlEvent)


    def parseEvent(self, rawNumpy):
        return ldmx_ts.TsRawDaqEvent.from_numpy(rawNumpy)
        

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

#         sqlalchemy.Table(
#             'ts_s30xl_threshold_trigger_event', sqlalchemy.MetaData(),
#             Column('id', Integer, primary_key=True, autoincrement=True)
#             Column('pulse_id', BigInteger, nullable=False) # uint64 -> BigInteger
#             Column('bunch_count', SmallInteger, nullable=False) # uint8 -> SmallInteger
#             Column('hits', Integer, nullable=False)
#             Column('amplitude0', Integer, nullable=False)
#             Column('amplitude1', Integer, nullable=False)
#             Column('amplitude2', Integer, nullable=False)
#             Column('amplitude3', Integer, nullable=False)
#             Column('amplitude4', Integer, nullable=False)
#             Column('amplitude5', Integer, nullable=False)
#             Column('amplitude6', Integer, nullable=False)
#             Column('amplitude7', Integer, nullable=False)
#             Column('amplitude8', Integer, nullable=False)
#             Column('amplitude9', Integer, nullable=False)
#             Column('amplitude10', Integer, nullable=False)
#             Column('amplitude11', Integer, nullable=False))

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

        connection.execute(self.table.insert(), [self.table_dict])
#         sqlEvent = TsS30xlThresholdTriggerEventSql(
#             pulse_id = event.header.pulseId,
#             bunch_count = event.header.bunchCount,
#             hits = event.hits,
#             amplitudes = event.amplitudes)

#         session.add(sqlEvent)
        

    def parseEvent(self, rawNumpy):
        # Parse the numpy array
        return ldmx_ts.TsS30xlThresholdTriggerEvent.from_numpy(rawNumpy)

