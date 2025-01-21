import pyrogue as pr
import pyrogue.interfaces.simulation
import pyrogue.interfaces.stream
import pyrogue.utilities.fileio
import pyrogue.protocols

import axipcie

import rogue

import ldmx_tdaq
import ldmx_ts

import threading
import sys
import traceback
import time

class S30xlAPxRoot(pr.Root):
    def __init__(self, sim=True, emu=False, host='192.168.10.10', **kwargs):
        super().__init__(timeout=100000, **kwargs)

        self.zmqServer = pyrogue.interfaces.ZmqServer(root=self, addr='127.0.0.1', port=0)
        self.addInterface(self.zmqServer)

        self.srp = rogue.protocols.srp.SrpV3()
        self.addInterface(self.srp)
        
        if sim is True:
            SIM_SRP_PORT = 10000
            SIM_DAQ_EVENT_PORT = 11000
            SIM_TRIG_EVENT_PORT = 12000

            self.srpStream = rogue.interfaces.stream.TcpClient('localhost', SIM_SRP_PORT)
            self.eventStream = rogue.interfaces.stream.TcpClient('localhost', SIM_DAQ_EVENT_PORT)

        else:
            # Open rUDP connections
            self.srpUdp = pyrogue.protocols.UdpRssiPack(host=host, jumbo=True, port=8192, packVer=2, name='SrpRssi')
            self.eventUdp = pyrogue.protocols.UdpRssiPack(host=host, jumbo=True, port=8193, packVer=2, name='TsDaqEventRssi')

            self.addInterface(self.srpUdp, self.eventUdp) #, self.trigDataUdp)
            
            self.srpStream = self.srpUdp.application(dest=0)
            self.eventStream = self.eventUdp.application(dest=0)


        # Add stream interfaces for clean exit
        self.addInterface(self.srpStream, self.eventStream) #, self.tsTrigEventStream)
            
        # Connect srp stream to srp protocol
        self.srp == self.srpStream

        # Instantiate the FPGA Device Tree
        self.add(ldmx_ts.S30xlAPx(
            memBase = self.srp,
            expand = True))

        
        #trigDebug = ldmx_ts.TsS30xlThresholdTriggerEventReceiver() # rogue.interfaces.stream.Slave()
#        trigDebug.setDebug(100, 'Trig Debug')

        #self.addInterface(daqDebug, trigDebug)

        #self.tsDaqEventStream >> daqDebug
        #self.tsTrigEventStream >> trigDebug
        
        fifo1 = rogue.interfaces.stream.Fifo(0, 0, False)
        fifo2 = rogue.interfaces.stream.Fifo(0, 0, False)
        fifo3 = rogue.interfaces.stream.Fifo(0, 0, False)
        fifo4 = rogue.interfaces.stream.Fifo(0, 0, False)
        self.addInterface(fifo1, fifo2, fifo3, fifo4)        

        self.eventStreamUnbatcher = rogue.protocols.batcher.SplitterV1()
        self.eventStream >> fifo1 >> self.eventStreamUnbatcher
        #self.eventStreamUnbatcher << fifo1 << self.eventStream
        self.addInterface(self.eventStreamUnbatcher)

        daqDebug = rogue.interfaces.stream.Slave()
        daqDebug.setDebug(1, 'Daq Debug')
        self.addInterface(daqDebug)
        #self.eventStream >> daqDebug        
        

        
        # Create a filter for TS RAW DAQ Events and send the TS DAQ data through it
        self.tsRawDaqEventFilter = ldmx_ts.TsRawDaqEventFilter()
        self.addInterface(self.tsRawDaqEventFilter)
        self.tsRawDaqEventFilter << self.eventStreamUnbatcher


        # Create a filter for TS Threshold Trigger Events
        self.tsS30xlThresholdTriggerEventFilter = ldmx_ts.TsS30xlThresholdTriggerEventFilter()
        self.addInterface(self.tsS30xlThresholdTriggerEventFilter)
        self.tsS30xlThresholdTriggerEventFilter << self.eventStreamUnbatcher


        self.lclsTimingEventFilter = ldmx_tdaq.DaqEventFilter(subsystemId=1, contributorId=3)
        self.addInterface(self.lclsTimingEventFilter)
        self.lclsTimingEventFilter << self.eventStreamUnbatcher


        # Add the Sqlite Database
        self.add(ldmx_tdaq.SqliteDatabase(hidden=False))
        self.addInterface(self.SqliteDatabase)

        # Create and connect SQL Receivers
        self.tsRawDaqEventSqlReceiver = ldmx_ts.TsRawDaqEventSqlReceiver(database=self.SqliteDatabase)
        self.add(self.tsRawDaqEventSqlReceiver)
        self.addInterface(self.tsRawDaqEventSqlReceiver)
        self.tsRawDaqEventFilter >> fifo2 >> self.tsRawDaqEventSqlReceiver

        self.tsS30xlThresholdTriggerEventSqlReceiver = ldmx_ts.TsS30xlThresholdTriggerEventSqlReceiver(database=self.SqliteDatabase)
        self.add(self.tsS30xlThresholdTriggerEventSqlReceiver)
        self.addInterface(self.tsS30xlThresholdTriggerEventSqlReceiver)
        self.tsS30xlThresholdTriggerEventFilter >> fifo3 >> self.tsS30xlThresholdTriggerEventSqlReceiver

#         self.lclsTimingEventSqlReceiver = ldmx_tdaq.LclsTimingEventSqlReceiver(database=self.SqliteDatabase)
#         self.add(self.lclsTimingEventSqlReceiver)
#         self.addInterface(self.lclsTimingEventSqlReceiver)
#         self.lclsTimingEventFilter >> fifo4 >> self.lclsTimingEventSqlReceiver 
        

        configStream = pyrogue.interfaces.stream.Variable(root=self)

#         self.add(pyrogue.utilities.fileio.StreamWriter(name='DataWriter')) #, configStream={0: configStream}))
#         self.tsDaqEventStreamUnbatcher >>  self.DataWriter.getChannel(1)
#         self.tsTrigEventStreamUnbatcher >> self.DataWriter.getChannel(2)

        # Debug Slave
#         dbg = rogue.interfaces.stream.Slave()

#         # Set debug mode for first 100 bytes, with name myDebug
#         dbg.setDebug(100,"myDebug")

#         # Connect the src and dst
#         self.tsDaqEventStreamUnbatcher >> dbg
#         self.tsTrigEventStreamUnbatcher >> dbg

#         self.addInterface(dbg)
        

        #self.add(ldmx_tdaq.SqliteVariableLogger(self.SqliteDatabase))
        
        # Log variable
#         self.sqlLogger = pyrogue.interfaces.SqlLogger(
#             root = self,
#             url = 'sqlite:///test.db')

#        self.addInterface(self.sqlLogger)

        @self.command()
        def list_thread_stack_traces():
            for thread_id, frame in sys._current_frames().items():
                print(f"\nThread ID: {thread_id}")
                print("".join(traceback.format_stack(frame)))

