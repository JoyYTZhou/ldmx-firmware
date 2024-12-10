import rogue.interfaces.stream as ris
import ldmx_tdaq

class DaqEventFilter(ris.Slave, ris.Master):
    def __init__(self, *, subsytemId=None, contributorId=None, **kwargs):
        ris.Slave.__init__(self)
        ris.Master.__init__(self)

        print('Making DaqEventFilter')

        self.subsytemId = subsytemId
        self.contributorId = contributorId

    def _acceptFrame(self, frame):
#        with frame.lock():
        rawNumpy = frame.getNumpy(0, frame.getPayload())
        header = rawNumpy[0:16].view(ldmx_tdaq.EventHeaderDType)

        forward = True
        if (self.subsytemId is not None and header['subsytemId'] != self.subsytemId):
            forward = False

        if (self.contributorId is not None and header['contributorId'] != self.contributorId):
            forward = False

        #print(f'Got Frame - {header} - {forward=} - {self.__class__.__name__}')                

        if forward is True:
            self._sendFrame(frame)
            
        
class DaqDebug(ris.Slave):

    def _acceptFrame(self, frame):
        npy = frame.getNumpy()

        header = npy[0:16].view(ldmx_tdaq.EventHeaderDType)
        print(f'Got frame with header \n {header} \n size = {len(npy[16:])}')
