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
import surf.devices.silabs as silabs
import ldmx_tdaq

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

        self.addRemoteVariables(
            name         = 'status: NLINK',
            description  = 'The level driven by pin LINK. High impedance is interpreted as 1.',
            offset       = (0x01<<2),
            bitSize      = 1,
            bitOffset    = 0,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )
        
        self.addRemoteVariables(
            name         = 'status: NALERT',
            description  = 'The level driven by the local side pin ALERT. High impedance is interpreted as 1.',
            offset       = (0x01<<2),
            bitSize      = 1,
            bitOffset    = 1,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval

        )

        self.addRemoteVariables(
            name         = 'status: EXT_NALERT',
            description  = 'The level driven into the remote ALERT pin. Link must be established.',
            offset       = (0x01<<2),
            bitSize      = 1,
            bitOffset    = 2,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'status: SPEED_IDX',
            description  = '<0-8>. Encoded index from values set on SPEED1 and SPEED2. See Table 2 of datasheet.',
            offset       = (0x01<<2),
            bitSize      = 4,
            bitOffset    = 4,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )


        self.addRemoteVariables(
            name         = 'EVENT: LINK_GOOD',
            description  = 'The local and remote I2C networks are connected.',
            offset       = (0x02<<2),
            bitSize      = 1,
            bitOffset    = 0,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'EVENT: LINK_LOST',
            description  = 'The local and remote I2C networks have lost link communication.',
            offset       = (0x02<<2),
            bitSize      = 1,
            bitOffset    = 1,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )
                        
        self.addRemoteVariables(
            name         = 'EVENT: FAULT',
            description  = 'Set if any field in the FAULT register is set by the system. Clearing this bit clears all bits in the FAULT register.',
            offset       = (0x02<<2),
            bitSize      = 1,
            bitOffset    = 2,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'FAULT: I2C_WRITE_FAULT',
            description  = '',
            offset       = (0x04<<2),
            bitSize      = 1,
            bitOffset    = 0,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'FAULT: LINK_FAULT',
            description  = '',
            offset       = (0x04<<2),
            bitSize      = 1,
            bitOffset    = 1,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )
        
        self.addRemoteVariables(
            name         = 'FAULT: EXT_I2C_FAULT',
            description  = '',
            offset       = (0x04<<2),
            bitSize      = 1,
            bitOffset    = 2,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )

        self.addRemoteVariables(
            name         = 'FAULT: TX_BUF_OVERFLOW',
            description  = '',
            offset       = (0x04<<2),
            bitSize      = 1,
            bitOffset    = 3,
            mode         = 'RO',
            number       = 1,
            stride       = 0,
            pollInterval = pollInterval
        )




class ZccmApplication(pr.Device):
    def __init__(self,
                 top_level="",
                 **kwargs):
        super().__init__(**kwargs)

        # FC Receiver
        self.add(ldmx_tdaq.FcReceiver(
            name         = 'FCReceiver',
            offset       = 0x10_0000,
            hidden       = False
        ))

        # Backplane (RM0/RM1) UART
        self.add(pr.RemoteVariable(
            name         = 'RM01_UART',
            offset       = 0x1_C0EC,
            pollInterval = 0,
            hidden       = False
        ))

        # Backplane (RM2/RM3) UART
        self.add(pr.RemoteVariable(
            name         = 'RM23_UART',
            offset       = 0x2_C0EC,
            pollInterval = 0,
            hidden       = False
        ))

        
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
        self.add(silabs.Si5345Lite(
            name           = 'SYNTH_I2C',
            offset         = 0x3_0000,
            #pollInterval   = 0,
            #hidden         = False,
            #csv_input_file = top_level+'firmware/common/ts/python/ldmx_ts/Si5344/Si5344-RevD-zCCM_gen-Registers.txt'
        ))

        # jitter cleaner clock chip 
        self.add(silabs.Si5345Lite(
            name           = 'JITTER_I2C',
            offset         = 0x4_0000,
            #pollInterval   = 0,
            #hidden         = False,
            #csv_input_file = top_level+'firmware/common/ts/python/ldmx_ts/Si5344/Si5344-RevD-zCCM_gen-Registers.txt'
        ))
        
        self.add(pr.RemoteVariable(
            name         = 'input_register',
            offset       = 0xB_0000,
            bitSize      = 32,
            mode         = 'RW',
            pollInterval = 1,
        ))

        self.add(pr.RemoteVariable(
            name         = 'output_registerA',
            offset       = 0xB_0100,
            bitSize      = 32,
            mode         = 'RW',
            pollInterval = 1,
        ))

        
        self.add(pr.RemoteVariable(
            name         = 'output_registerB',
            offset       = 0xB_0104,
            bitSize      = 32,
            mode         = 'RW',
            pollInterval = 1,
        ))
