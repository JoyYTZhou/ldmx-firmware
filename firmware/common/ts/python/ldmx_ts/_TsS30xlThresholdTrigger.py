import pyrogue as pr

class TsS30xlThresholdTrigger(pr.Device):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add(pr.RemoteVariable(
            name = 'Threshold',
            offset = 0x10,
            valueBits = 32,
            base = pr.UInt,
            disp = '{:d}',
            units = 'PE'))

        
