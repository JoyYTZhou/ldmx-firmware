import numpy as np
import ldmx_tdaq

LclsTimingMsgDType = np.dtype([
    ('pulseId', np.uint64),  
    ('timeStamp', np.uint64),
    ('fixedRates', np.uint16),    
    ('control3', np.uint16),
    ('empty1', np.uint32),
    ('empty2', np.uint64)
])
    

LclsTimingDaqEventDType = np.dtype([
    ('header', ldmx_tdaq.EventHeaderDType),
    ('msg', LclsTimingMsgDType)
])
