import pyrogue as pr

class FcTxLogic(pr.Device):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add(pr.RemoteVariable(
            name = 'FcState',
            offset = 0x00,
            bitSize = 4,
            bitOffset = 0,
            base = pr.UInt,
            enum = {
                0b0000 : 'RESET',
                0b0001 : 'IDLE',
                0b0010 : 'BC0',
                0b0011 : 'PRESTART',
                0b0100 : 'RUNNING',
                0b0101 : 'STOPPED',                
            }))

        @self.command()
        def AdvanceRunState():
            state = self.FcState.value()
            state += 1
            self.FcState.set(state, write=True)

        @self.command()
        def StopRun():
            self.FcState.setDisp('STOPPED')

        @self.command()
        def ResetRun():
            self.FcState.setDisp('RESET')
