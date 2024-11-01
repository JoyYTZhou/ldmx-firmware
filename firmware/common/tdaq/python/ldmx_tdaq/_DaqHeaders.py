import numpy as np
from dataclasses import dataclass, field
from typing import List
import ldmx_tdaq

@dataclass
class EventHeader:
    version: int
    subsystemId: int 
    contributorId: int
    bunchCount: int
    pulseId: int
    #data: np.ndarray

    @classmethod
    def create_from_numpy(cls, arr):
        header = cls(
            version = int(arr[0]),
            subsystemId = int(arr[1]),
            contributorId = int(arr[2]),
            bunchCount = int(arr[7]),
            pulseId = int(arr[8:16].view(np.uint64)))
        return header

    def fill_from_numpy(self, arr):
        """ Fill an EventHeader object from a frame numpy array """
        self.version = int(arr[0])
        self.subsystemId = int(arr[1])
        self.contributorId = int(arr[2])
        self.bunchCount = int(arr[7])
        self.pulseId = int(arr[8:16].view(np.uint64))
        

    @classmethod
    def data(cls, arr):
        return arr[16:]

