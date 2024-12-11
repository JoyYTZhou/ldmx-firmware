import numpy as np
import ldmx_tdaq

LclsTimingMsgDType = np.dtype([
    ('pulseId', np.uint64),  
    ('timeStamp', np.uint64),
    ('beamRequest', np.uint32),
    ('fixedRates', np.uint16),    
    ('control3', np.uint16),
    ('empty', np.uint64)
])
    

LclsTimingDaqEventDType = np.dtype([
    ('header', ldmx_tdaq.EventHeaderDType),
    ('msg', LclsTimingMsgDType)
])
