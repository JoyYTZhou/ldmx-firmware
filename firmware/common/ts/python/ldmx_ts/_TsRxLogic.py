import pyrogue as pr
import surf.xilinx
import ldmx_tdaq
import ldmx_ts

class TsRxLogic(pr.Device):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add(pr.RemoteVariable(
            name = 'LinkUp',
            offset = 0x0,
            mode = 'RO',
            bitOffset = 0,
            bitSize = 1,
            base = pr.Bool,
            pollInterval = 1))

        self.add(pr.RemoteCommand(
            name = 'CountReset',
            offset = 0x0,
            bitOffset = 1,
            bitSize = 1,
            function = pr.Command.touchOne))

        self.add(pr.RemoteCommand(
            name = 'Reset',
            offset = 0x18,
            bitSize = 1,
            bitOffset = 0,
            function = pr.Command.touchOne))

        self.add(pr.RemoteVariable(
            name = 'Loopback',
            offset = 0x4,
            bitSize = 3,
            enum = {
                0b000: 'Off',
                0b010: 'Near-end PMA'}))

        self.add(pr.RemoteVariable(
            name = 'RxFrameCount',
            offset = 0x08,
            bitSize = 64,
            base = pr.UInt,
            disp = '{:d}',
            mode = 'RO',
            pollInterval = 1))

        self.add(pr.RemoteVariable(
            name = 'RxErrorCount',
            offset = 0x10,
            bitSize = 32,
            base = pr.UInt,
            disp = '{:d}',
            mode = 'RO',
            pollInterval = 1))

        self.add(pr.RemoteVariable(
            name = 'InitCount',
            offset = 0x24,
            bitSize = 32,
            base = pr.UInt,
            disp = '{:d}',
            mode = 'RO',
            pollInterval = 1))

        self.add(pr.RemoteVariable(
            name = 'Bc0Count',
            offset = 0x28,
            bitSize = 32,
            base = pr.UInt,
            disp = '{:d}',
            mode = 'RO',
            pollInterval = 1))

        self.add(pr.RemoteVariable(
            name = 'State',
            mode = 'RO',
            offset = 0x14,
            bitSize = 4,
            bitOffset = 0,
            pollInterval = 1,
            enum = {
                0x0: 'INIT_S',
                0x1: 'WAIT_RESETDONE_LOW_S',
                0x2: 'WAIT_RESETDONE_HIGH_S',
                0x8: 'WAIT_COMMA_S',
                0x9: 'WORD_1_S',
                0xa: 'WORD_2_S',
                0xb: 'WORD_3_S',
                0xc: 'WORD_4_S',
                0xd: 'WORD_5_S'}))

        self.add(pr.RemoteVariable(
            name = 'RxData',
            offset = 0x20,
            bitSize = 16,
            bitOffset = 0,
            base = pr.UInt,
            mode = 'RO'))

        self.add(pr.RemoteVariable(
            name = 'RxDataK',
            offset = 0x20,
            bitSize = 2,
            bitOffset = 16,
            base = pr.UInt,
            mode = 'RO'))

        @self.command()
        def CheckLink():
            pass
            
