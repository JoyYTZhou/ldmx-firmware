import rogue.interfaces.stream as ris
import ldmx_tdaq

class DaqEventFilter(ris.Slave, ris.Master):
    def __init__(self, *, subsytemId=None, contributorId=None, **kwargs):
        ris.Slave.__init__(self)
        ris.Master.__init__(self)

        print('Making DaqEventFilter')

        self.subsytemId = subsytemId
        self.contributorId = contributorId

        self.header = ldmx_tdaq.EventHeader(
            version=0,
            subsystemId=0,
            contributorId=0,
            bunchCount=0,
            pulseId=0)

    def _acceptFrame(self, frame):
        rawNumpy = frame.getNumpy(0, frame.getPayload())
        self.header.fill_from_numpy(rawNumpy)

        print(f'{self.__class__.__name__} got frame {self.header}')

        forward = True
        if (self.subsytemId is not None and self.header.subsytemId != self.subsytemId):
            print(f'{self.header.subsytemId=} != {self.subsytemId=}')
            forward = False

        if (self.contributorId is not None and self.header.contributorId != self.contributorId):
            print(f'{self.header.contributorId=} != {self.contributorId=}')            
            forward = False

        if forward is True:
            print('Forwarding')
            self._sendFrame(frame)

        
