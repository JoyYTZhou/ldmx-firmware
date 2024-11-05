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
            

class SqlEventReceiver(rogue.interfaces.stream.Slave):
    def __init__(self, database, **kwargs):
        super().__init__(**kwargs)

        self.database = database
        self._queue = queue.Queue()
        self._thread = threading.Thread(target=self._worker)
        self._thread.start()

    def _acceptFrame(self, frame):
        # Read the frame into numpy array
        rawNumpy = frame.getNumpy(0, frame.getPayload())

        # Parse the numpy array
        event = self.parseEvent(rawNumpy)

        self._queue.put(event)

    def _stop(self):
        if not self._queue.empty():
            print('Waiting for SQL Receiver to finish')
        self._queue.put(None)
        self._thread.join()
        print('SQL Receiver finished')   
        

    def _worker(self):
        while True:
            # Block and wait for a queue entry to arrive
            event = self._queue.get()

            # Exit thread if a None entry is received
            if event is None:
                return

            # Continue only if the database connection is present
            if not self.database._engine:
                continue

            try:
                with self.database.SessionFactory() as session:
                    while event is not None:
                        # Insert the event into the database
                        self.insertEvent(session, event)

                        # If the queue is empty, commit the transaction and break
                        if self._queue.empty():
                            session.commit()
                            break

                        # Get the next event from the queue
                        event = self._queue.get()

            except Exception as e:
                print(e)
                # Handle database disconnection
                #self.database._engine = None
                #pr.logException(self._log, e)
                #self._log.error(f"Lost database connection to {self._url}")

    
            
class TsRawDaqEventSqlReceiver(SqlEventReceiver):

    def insertEvent(self, session, event):
        for i, msg in enumerate(event.msgs):
            #print(f'Writing msg into database - {msg}')
            sqlEvent = TsRawDaqEventSql(
                pulse_id = event.header.pulseId,
                bunch_count = event.header.bunchCount,
                channel_count = 6,
                lane = msg.lane,
                capId = msg.capId,
                ce = msg.ce,
                bc0 = msg.bc0,
                adc = msg.adc,
                tdc = msg.tdc)

            session.add(sqlEvent)


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

    def insertEvent(self, session, event):
        sqlEvent = TsS30xlThresholdTriggerEventSql(
            pulse_id = event.header.pulseId,
            bunch_count = event.header.bunchCount,
            hits = event.hits,
            amplitudes = event.amplitudes)

        session.add(sqlEvent)
        

    def parseEvent(self, rawNumpy):
        # Parse the numpy array
        return ldmx_ts.TsS30xlThresholdTriggerEvent.from_numpy(rawNumpy)

