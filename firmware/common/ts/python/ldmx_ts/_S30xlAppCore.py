import pyrogue as pr
import ldmx_tdaq
import ldmx_ts
import surf.protocols.batcher

class Delays(pr.Device):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        for fiber in range(2):
            self.add(pr.RemoteVariable(
                name = f'TsFiber{fiber}Delay',
                base = pr.UInt,
                offset = 0x000000 + (4*fiber),
                bitSize = 8,
                mode = 'RO'))

        for fiber in range(2):
            self.add(pr.RemoteVariable(
                name = f'TsFiber{fiber}Aligned',
                base = pr.Bool,
                offset = 0x000000 + (4*fiber),
                bitSize = 1,
                bitOffset = 8,
                mode = 'RO'))

        for ch in range(12):
            self.add(pr.RemoteVariable(
                name = f'TrigAmplitude{ch}Delay',
                base = pr.UInt,
                offset = 0x1000 + (4*ch),
                bitSize = 8,
                bitOffset = 0,
                mode = 'RO'))

        for ch in range(12):
            self.add(pr.RemoteVariable(
                name = f'TrigAmplitude{ch}Aligned',
                base = pr.Bool,
                offset = 0x1000 + (4*ch),
                bitSize = 1,
                bitOffset = 8,
                mode = 'RO'))

class S30xlAppCore(pr.Device):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.add(ldmx_tdaq.FcReceiver(
            offset = 0x0_0000))

        self.add(ldmx_ts.TsDataRx(
            offset = 0x2000_0000,
            expand = True))

#         self.add(Delays(
#             offset = 0x10_0000))

            
            
            

#         self.add(surf.protocols.batcher.AxiStreamBatcherAxil(
#             name = 'RawBatcher',
#             offset = 0x100000))

#         self.add(surf.protocols.batcher.AxiStreamBatcherAxil(
#             name = 'TrigBatcher',
#             offset = 0x100100))
        

