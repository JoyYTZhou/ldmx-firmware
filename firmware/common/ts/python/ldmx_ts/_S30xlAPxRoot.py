import pyrogue as pr
import pyrogue.interfaces.simulation
import pyrogue.protocols

import axipcie

import rogue

import ldmx_tdaq
import ldmx_ts


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
            self.tsDaqEventStream = rogue.interfaces.stream.TcpClient('localhost', SIM_DAQ_EVENT_PORT)
            self.tsTrigEventStream = rogue.interfaces.stream.TcpClient('localhost', SIM_TRIG_EVENT_PORT)

        else:
            # Open rUDP connections
            self.srpUdp = pyrogue.protocols.UdpRssiPack(host=host, port=8192, packVer=2, name='SrpRssi')
            self.tsDaqUdp = pyrogue.protocols.UdpRssiPack(host=host, port=8193, packVer=2, name='TsDaqEventRssi')
            self.trigDataUdp = pyrogue.protocols.UdpRssiPack(host=host, port=8194, packVer=2, name='TsTrigEventRssi')

            self.addInterface(self.srpUdp, self.tsDaqUdp, self.trigDataUdp)
            
            self.srpStream = self.srpUdp.application(dest=0)
            self.tsDaqEventStream = self.tsDaqUdp.application(dest=0)
            self.tsTrigEventStream = self.trigDataUdp.application(dest=0)

        # Add stream interfaces for clean exit
        self.addInterface(self.srpStream, self.tsDaqEventStream, self.tsTrigEventStream)
            
        # Connect srp stream to srp protocol
        self.srp == self.srpStream

        # Instantiate the FPGA Device Tree
        self.add(ldmx_ts.S30xlAPx(
            memBase = self.srp,
            expand = True))

        #daqDebug = ldmx_ts.TsRawDaqEventReceiver() #rogue.interfaces.stream.Slave()
#        daqDebug.setDebug(100, 'Daq Debug')
        #trigDebug = ldmx_ts.TsS30xlThresholdTriggerEventReceiver() # rogue.interfaces.stream.Slave()
#        trigDebug.setDebug(100, 'Trig Debug')

        #self.addInterface(daqDebug, trigDebug)

        #self.tsDaqEventStream >> daqDebug
        #self.tsTrigEventStream >> trigDebug

        # Create a filter for TS RAW DAQ Events and send the TS DAQ data through it
#         self.tsRawDaqEventFilter = ldmx_ts.TsRawDaqEventFilter()
#         self.addInterface(self.tsRawDaqEventFilter)
#         self.tsRawDaqEventFilter << self.tsDaqEventStream 

        # Generic TS Raw event receiver for debug
#        self.tsRawDaqEventReceiver = ldmx_ts.TsRawDaqEventReceiver()
#        self.addInterface(self.tsRawDaqEventReceiver)
        #self.tsRawDaqEventReceiver << self.tsRawDaqEventFilter 
#        self.tsDaqEventStream >> self.tsRawDaqEventReceiver

        # Create a filter for TS Threshold Trigger Events
#         self.tsS30xlThresholdTriggerEventFilter = ldmx_ts.TsS30xlThresholdTriggerEventFilter()
#         self.addInterface(self.tsS30xlThresholdTriggerEventFilter)
#         self.tsS30xlThresholdTriggerEventFilter << self.tsTrigEventStream

        # Generic Threshold event receiver for debug
#        self.tsS30xlThresholdTriggerEventReceiver = ldmx_ts.TsS30xlThresholdTriggerEventReceiver()
#        self.addInterface(self.tsS30xlThresholdTriggerEventReceiver)
#        self.tsS30xlThresholdTriggerEventReceiver <<  self.tsS30xlThresholdTriggerEventFilter
#        self.tsTrigEventStream >> tsS30xlThresholdTriggerEventReceiver

        fifo1 = rogue.interfaces.stream.Fifo(0, 0, True)
        fifo2 = rogue.interfaces.stream.Fifo(0, 0, True)
        fifo3 = rogue.interfaces.stream.Fifo(0, 0, True)
        fifo4 = rogue.interfaces.stream.Fifo(0, 0, True)

        self.addInterface(fifo1, fifo2, fifo3, fifo4)

        # Unbatch the streams
        self.tsDaqEventStreamUnbatcher = rogue.protocols.batcher.SplitterV1()
        self.tsDaqEventStreamUnbatcher << fifo1 << self.tsDaqEventStream
        self.addInterface(self.tsDaqEventStreamUnbatcher)

        self.tsTrigEventStreamUnbatcher = rogue.protocols.batcher.SplitterV1()
        self.tsTrigEventStreamUnbatcher << fifo2 << self.tsTrigEventStream
        self.addInterface(self.tsTrigEventStreamUnbatcher)
        

        # Add the Sqlite Database
        self.add(ldmx_tdaq.SqliteDatabase())
        self.addInterface(self.SqliteDatabase)

        # Create and connect SQL Receivers
        self.tsRawDaqEventSqlReceiver = ldmx_ts.TsRawDaqEventSqlReceiver(database=self.SqliteDatabase)
        self.add(self.tsRawDaqEventSqlReceiver)
        self.addInterface(self.tsRawDaqEventSqlReceiver)
#        self.tsRawDaqEventSqlReceiver << self.tsRawDaqEventFilter
        self.tsRawDaqEventSqlReceiver << fifo3 << self.tsDaqEventStreamUnbatcher

        self.tsS30xlThresholdTriggerEventSqlReceiver = ldmx_ts.TsS30xlThresholdTriggerEventSqlReceiver(database=self.SqliteDatabase)
        self.add(self.tsS30xlThresholdTriggerEventSqlReceiver)
        self.addInterface(self.tsS30xlThresholdTriggerEventSqlReceiver)
        #self.tsS30xlThresholdTriggerEventSqlReceiver << self.tsS30xlThresholdTriggerEventFilter
        self.tsS30xlThresholdTriggerEventSqlReceiver << fifo4 << self.tsTrigEventStreamUnbatcher
        
        # Log variable
#         self.sqlLogger = pyrogue.interfaces.SqlLogger(
#             root = self,
#             url = 'sqlite:///test.db')

#        self.addInterface(self.sqlLogger)

