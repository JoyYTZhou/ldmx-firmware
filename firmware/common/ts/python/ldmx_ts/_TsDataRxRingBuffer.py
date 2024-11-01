
import pyrogue as pr
import surf.axi
import time

class TsDataRxRingBuffer(surf.axi.AxiLiteRingBuffer):

    # Last comment added by rherbst for demonstration.
    def __init__(self, **kwargs):
        super().__init__(datawidth=18, **kwargs)

        @self.command()
        def Dump2():
            self.BufEn.set(False)
            self.ClearBuffer()
            self.BufEn.set(True)
            time.sleep(5)
            self.BufEn.set(False)
            
            mask =  (1<<self._datawidth)-1
            length  = self.BufferLength.get()
#             if length > 512:
#                 length = 256

            buff = []
            for i in range(length):
                buff.append( self.data.get(index=i) & mask )

            for word in buff:
                if (word & 0x10000 != 0) and (word & 0x20000 == 0):
                    print() # print a new line if start of frame seen
                print(f'{word:#07x}', end= ' ')
