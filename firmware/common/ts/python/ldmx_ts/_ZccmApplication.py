#-----------------------------------------------------------------------------
# This file is part of the 'Camera link gateway'. It is subject to
# the license terms in the LICENSE.txt file found in the top-level directory
# of this distribution and at:
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
# No part of the 'Camera link gateway', including this file, may be
# copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE.txt file.
#-----------------------------------------------------------------------------

import pyrogue as pr
import csv

class PCA9536(pr.Device):
    def __init__(self,
                 description = "Container for xxx",
                 pollInterval = 1,
            **kwargs):
        super().__init__(description=description, **kwargs)       

        self.addRemoteVariables(
            name         = 'INPUT',
            description  = 'value when configured as input; Default: 0xFX',
            offset       = (0x00<<2),
            bitSize      = 8,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'OUTPUT',
            description  = 'value when configured as output; Default: 0xFF',
            offset       = (0x01<<2),
            bitSize      = 8,
            mode         = 'RW',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'POLARITY',
            description  = 'Polarity of pin; inverted when 1; Default: 0x00',
            offset       = (0x02<<2),
            bitSize      = 8,
            mode         = 'RW',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'CONFIG',
            description  = 'configuration of pins; 1 is input (high-impedance output driver); Default: 0xFF',
            offset       = (0x03<<2),
            bitSize      = 8,
            mode         = 'RW',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

class LTC4331(pr.Device):
    def __init__(self,
                 description = "Container for xxx",
                 pollInterval = 1,
            **kwargs):
        super().__init__(description=description, **kwargs)

        self.addRemoteVariables(
            name         = 'SCRATCH',
            description  = 'Scratch register',
            offset       = (0x05<<2),
            bitSize      = 8,
            mode         = 'RW',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

class Si5344(pr.Device):
    def __init__(self,
                 description = "Container for xxx",
                 pollInterval = 1,
            **kwargs):
        super().__init__(description=description, **kwargs)

        n= self.addRemoteVariables(
            name         = 'PAGE',
            description  = 'Selects one of 256 possible pages',
            offset       = 0x1,
            bitSize      = 8,
            mode         = 'RW',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        #current_page = n.get() 
        #print("current_page:",current_page)
        
        with open('ldmx_ts/Si5344/Si5344-RevD-zCCM_gen-Registers.txt') as csvfile:
            reader = csv.reader(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
            # Loop through the rows in the CSV file                                                                                          
            for row in reader:
                if(row[0][0] == '#' ): continue
                if(row[0] == 'Address'): continue
                address = int(row[0],16)
                page   = (address&0xFF00)>>8
                offset = (address&0xFF)
                data   = int(row[1],16)
                
                self.addRemoteVariables(
                    name         = hex(address),
                    description  = hex(address),
                    offset       = (offset<<2),
                    bitSize      = 8,
                    mode         = 'RW',
                    number       = 1,
                    stride       = 0,
                    pollInterval = pollInterval                    
                )
                
                print("n = self.getNode({0})".format(hex(address)))
                #n = self.getNode(hex(address))
                print("self.PAGE[0].set({0})".format(page))
                #self.PAGE[0].set(page)
                print("n.set({0})".format(data))
                #n.set(value)

        # self.addRemoteVariables(
        #     name         = 'PN_BASE_LOWER',
        #     description  = 'lower 2 digits of part number',
        #     offset       = 0x2,
        #     bitSize      = 8,
        #     mode         = 'RO',
        #     number       = 1,
        #     stride       = 0,
        #     pollInterval = pollInterval
        # )

        # self.addRemoteVariables(
        #     name         = 'PN_BASE_UPPER',
        #     description  = 'upper 2 digits of part number',
        #     offset       = 0x3,
        #     bitSize      = 8,
        #     mode         = 'RO',
        #     number       = 1,
        #     stride       = 0,
        #     pollInterval = pollInterval
        # )

        
        
class ZccmApplication(pr.Device):
    def __init__(self,**kwargs):
        super().__init__(**kwargs)

        # Backplane (RM0/RM1) GPIO
        self.add(PCA9536(
            name         = 'RM01_GPIO',
            offset       = 0x1_0000,
            pollInterval = 0,
            hidden       = False
        ))
        
        # RM0/RM1 I2C extender
        self.add(LTC4331(
            name         = 'RM01_I2C_EXT',
            offset       = 0x1_8000,
            pollInterval = 0,
            hidden       = False,
        ))

        # Backplane (RM2/RM3) GPIO
        self.add(PCA9536(
            name         = 'RM23_GPIO',
            offset       = 0x2_0000,
            pollInterval = 0,
            hidden       = False
        ))
        
        # RM2/RM3 I2C extender
        self.add(LTC4331(
            name         = 'RM23_I2C_EXT',
            offset       = 0x2_8000,
            pollInterval = 0,
            hidden       = False,
        ))

        # synthesizing clock chip 
        self.add(Si5344(
            name         = 'SYNTH_I2C',
            offset       = 0x3_0000,
            pollInterval = 0,
            hidden       = False,
        ))

        # jitter cleaner clock chip 
        self.add(Si5344(
            name         = 'JITTER_I2C',
            offset       = 0x4_0000,
            pollInterval = 0,
            hidden       = False,
        ))
        
        self.add(pr.RemoteVariable(
            name         = 'input_register',
            offset       = 0xE_0000,
            bitSize      = 32,
            mode         = 'RW',
            pollInterval = 1,
        ))

        self.add(pr.RemoteVariable(
            name         = 'output_registerA',
            offset       = 0xE_0100,
            bitSize      = 32,
            mode         = 'RW',
            pollInterval = 1,
        ))

        
        self.add(pr.RemoteVariable(
            name         = 'output_registerB',
            offset       = 0xE_0104,
            bitSize      = 32,
            mode         = 'RW',
            pollInterval = 1,
        ))
