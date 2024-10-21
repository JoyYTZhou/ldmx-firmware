import pyrogue as pr
import rogue

import numpy as np
from dataclasses import dataclass, field
from typing import List

@dataclass
class EventHeader:
    version: int
    subsystemId: int 
    contributorId: int
    bunchCount: int
    pulseId: int
    #data: np.ndarray

    @classmethod
    def from_numpy(cls, arr):
        header = cls(
            version = int(arr[0]),
            subsystemId = int(arr[1]),
            contributorId = int(arr[2]),
            bunchCount = int(arr[7]),
            pulseId = int(arr[8:16].view(np.uint64)))
        return header

    @classmethod
    def data(cls, arr):
        return arr[16:]

    
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
class TsDaqRawEvent:
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

class TsDaqEventReceiver(rogue.interfaces.stream.Slave):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
    def _acceptFrame(self, frame):
        rawNumpy = frame.getNumpy(0, frame.getPayload())

        event = TsDaqRawEvent.from_numpy(rawNumpy)
        print(event)



@dataclass
class TsThresholdTrigEvent:
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
    
    

class TsThresholdTrigEventReceiver(rogue.interfaces.stream.Slave):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        
    def _acceptFrame(self, frame):
        rawNumpy = frame.getNumpy(0, frame.getPayload())

        event = TsThresholdTrigEvent.from_numpy(rawNumpy)
        print(event)
        
        
