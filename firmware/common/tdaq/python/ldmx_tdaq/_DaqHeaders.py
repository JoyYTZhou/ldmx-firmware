import numpy as np
from dataclasses import dataclass, field
from typing import List
import ldmx_tdaq

@dataclass
class EventHeader:
    burnCount: int
    subsystemId: int
    contributorId: int
    timestamp: int
    bunchCount: int
    pulseId: int
    #data: np.ndarray

    @classmethod
    def create_from_numpy(cls, arr):
        header = cls(
            burnCount = int(arr[0]),
            subsystemId = int(arr[1]),
            contributorId = int(arr[2]),
            timestamp = int(arr[8:16].view(np.uint64)),
            bunchCount = int(arr[8]),
            pulseId = int(arr[8:16].view(np.uint64))>>8)
        if header.burnCount !=0:
            print(header)
        return header

    def fill_from_numpy(self, arr):
        """ Fill an EventHeader object from a frame numpy array """
        self.burnCount = int(arr[0])
        self.subsystemId = int(arr[1])
        self.contributorId = int(arr[2])
        self.timestamp = int(arr[8:16].view(np.uint64))
        self.bunchCount = int(arr[8])
        self.pulseId = int(arr[8:16].view(np.uint64))>>8
        #if header.burnCount !=0:
        print(self)

    @classmethod
    def data(cls, arr):
        return arr[16:]

EventHeaderDType = np.dtype([
    ('burnCount', np.uint8),
    ('subsystemId', np.uint8),
    ('contributorId', np.uint8),
    ('empty1', np.uint8),
    ('empty2', np.uint32),
    ('timestamp', np.uint64),
])

def print_dtype(array):
    if array.dtype.names:
        for row in array:
            print({name: row[name] for name in array.dtype.names})
    else:
        print(array)
