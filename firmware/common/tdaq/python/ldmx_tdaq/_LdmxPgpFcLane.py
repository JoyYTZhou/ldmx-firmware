import rogue
import pyrogue as pr
import surf.xilinx
import surf.protocols.pgp
import ldmx_tdaq

class LdmxPgpFcLane(pr.Device):
    def __init__(self, numVc, gtType='GTY', **kwargs):
        super().__init__(**kwargs)


        self.add(surf.protocols.pgp.Pgp2fcAxi(
            name   = "Pgp2Fc",
            statusCountBits = 32,
            errorCountBits  = 32,
            offset = 0x4000))

        if gtType == 'GTY':
            self.add(ldmx_tdaq.Pgp2fcGtyCoreWrapper(
                name   = "GTY",
                offset = 0x0000))

        if gtType == 'GTH':
            self.add(ldmx_tdaq.Pgp2fcGthCoreWrapper(
                name   = "GTH",
                offset = 0x0000))
            

        if numVc > 0:
            self.add(surf.axi.AxiStreamMonAxiL(
                name = "TxStreamMon",
                offset = 0x8000,
                numberLanes = numVc,
                hideConfig = False,
                chName = None))

            self.add(surf.axi.AxiStreamMonAxiL(
                name = "RxStreamMon",
                offset = 0xC000,
                numberLanes = numVc,
                hideConfig = False,
                chName = None))

            def _linkUp(var, read):
                return all([dep.get(read=read) for dep in var.dependencies])
                    
            self.add(pr.LinkVariable(
                name = 'LinkUp',
                dependencies = [
                    self.Pgp2Fc.RxPhyReady,
                    self.Pgp2Fc.TxPhyReady,
                    self.Pgp2Fc.RxLocalLinkReady,
                    self.Pgp2Fc.RxRemLinkReady,
                    self.Pgp2Fc.TxLinkReady],
                linkedGet = _linkUp))
