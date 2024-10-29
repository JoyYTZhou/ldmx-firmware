import pyrogue as pr

class TsRxMsgAligner(pr.Device):
    def __init__(self, lanes, **kwargs):
        super().__init__(**kwargs)

        self.add(pr.RemoteVariable(
            name = 'State',
            mode = 'RO',
            base = pr.UInt,
            offset = 0x00,
            bitSize = 3,
            bitOffset = 0,
            enum = {
                0b000: 'None',
                0b001: 'WAIT_BC0_STATE_S',
                0b010: 'WAIT_BC0_DATA_S',
                0b100: 'ALIGNED_S'}))

        self.add(pr.RemoteVariable(
            name = 'TimestampFifoCount',
            mode = 'RO',
            offset = 0x04,
            bitSize = 8,
            base = pr.UInt,
            disp = '{:d}'))

        for lane in range(lanes):
            self.add(pr.RemoteVariable(
                name = f'TsMsgFifoCount[{lane}]',
                mode = 'RO',
                offset = 0x10 + (lane*4),
                bitSize = 8,
                base = pr.UInt,
                disp = '{:d}'))
        
