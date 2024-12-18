import pyrogue as pr

class S30xlKickerTrigger(pr.Device):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add(pr.RemoteVariable(
            name = 'RorPattern',
            offset = 0x00,
            bitOffset = 0,
            bitSize = 64,
            base = pr.UInt))

        self.add(pr.RemoteVariable(
            name = 'KickerCount',
            offset = 0x08,
            bitOffset = 0,
            bitSize = 64,
            base = pr.UInt,
            mode = 'RO'))

        self.add(pr.RemoteCommand(
            name = 'CountReset',
            offset = 0x10,
            bitOffset = 1,
            bitSize = 1,
            function = pr.Command.touchOne))

            
            
