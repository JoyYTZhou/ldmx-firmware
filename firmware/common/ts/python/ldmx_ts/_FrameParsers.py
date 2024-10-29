import pyrogue as pr
import rogue

import numpy as np
from dataclasses import dataclass, field
from typing import List

from ldmx_tdaq import EventHeader, DaqEventFilter, SubsystemId

import ldmx_ts


    
@dataclass
class TsData6ChMsg:
    lane: int
    capId: int
    ce: int
    bc0: int
    adc: List[int] = field(default_factory=list)
    tdc: List[int] = field(default_factory=list)

    @classmethod
    def from_numpy(cls, data):
        msg = cls(
            lane = int(data[15]),
            adc = [int(data[i]) for i in range(0, 6)],
            tdc = [int(data[i]) for i in range(8, 14)],
            capId = int(data[14] & 0x3),
            ce = int(data[14]>>2 & 0x1),
            bc0 = int(data[14]>>3 & 0x1))
        return msg
    
@dataclass
class TsRawDaqEvent:
    header: EventHeader
    msgs: List[TsData6ChMsg] = field(default_factory=list)

    @classmethod
    def from_numpy(cls, arr):
        header = EventHeader.from_numpy(arr)
        event_data = EventHeader.data(arr).reshape(-1, 16)
        num_messages = len(event_data)

        ret = cls(
            header = header,
            msgs = [TsData6ChMsg.from_numpy(msg_raw) for msg_raw in event_data])
        return ret

class TsRawDaqEventFilter(DaqEventFilter):
    def __init__(self):
        super().__init__(
            subsystemId = SubsystemId.TS_DAQ,
            contributorId = ldmx_ts.DaqContributorId.RAW_DATA)
    
class TsRawDaqEventReceiver(rogue.interfaces.stream.Slave):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
    def _acceptFrame(self, frame):
        rawNumpy = frame.getNumpy(0, frame.getPayload())

        event = TsRawDaqEvent.from_numpy(rawNumpy)
        print(event)



@dataclass
class TsS30xlThresholdTriggerEvent:
    header: EventHeader
    hits: np.uint16    
    amplitudes: List[np.uint32] = field(default_factory=list)


    @classmethod
    def from_numpy(cls, arr):
        header = EventHeader.from_numpy(arr)
        event_data = EventHeader.data(arr).reshape(-1, 4)
        
        amplitudes = [int.from_bytes(ch[:3], byteorder='little') for ch in event_data]
        hits = np.packbits(event_data[:, 3], bitorder='little').view(np.uint16)[0]

        ret = cls(
            header = header,
            amplitudes = amplitudes,
            hits = hits)
        return ret
    
class TsS30xlThresholdTriggerEventFilter(DaqEventFilter):
    def __init__(self):
        super().__init__(
            subsystemId = SubsystemId.TS_TRIGGER,
            contributorId = ldmx_ts.TriggerContributorId.S30XL_THRESHOLD_TRIGGER)
    

class TsS30xlThresholdTriggerEventReceiver(rogue.interfaces.stream.Slave):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
    def _acceptFrame(self, frame):
        rawNumpy = frame.getNumpy(0, frame.getPayload())

        event = TsS30xlThresholdTriggerEvent.from_numpy(rawNumpy)
        print(event)
        
        
