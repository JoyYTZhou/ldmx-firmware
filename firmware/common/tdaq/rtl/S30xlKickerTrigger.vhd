-------------------------------------------------------------------------------
-- File       : S30xlKickerTrigger.vhd
-- Company    : SLAC National Accelerator Laboratory
-------------------------------------------------------------------------------
-- Description:
-------------------------------------------------------------------------------
-- This file is part of 'PGP PCIe APP DEV'.
-- It is subject to the license terms in the LICENSE.txt file found in the
-- top-level directory of this distribution and at:
--    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
-- No part of 'PGP PCIe APP DEV', including this file,
-- may be copied, modified, propagated, or distributed except according to
-- the terms contained in the LICENSE.txt file.
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;

library lcls_timing_core;
use lcls_timing_core.TimingPkg.all;

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;
use ldmx_tdaq.TriggerPkg.all;

entity S30xlKickerTrigger is

   generic (
      TPD_G                : time    := 1 ns;
      AXIL_CLK_IS_FC_CLK_G : boolean := false);
   port (
      -- LCLS Timing and LDMX FC busses
      fcClk         : in  sl;
      fcRst         : in  sl;
      fcBus         : in  FcBusType;
      lclsTimingBus : in  TimingBusType;
      triggerData   : out TriggerDataType;

      -- AXI-Lite Interface (axilClk domain)
      axilClk         : in  sl;
      axilRst         : in  sl;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType);


end entity S30xlKickerTrigger;

architecture rtl of S30xlKickerTrigger is

   type RegType is record
      rorPattern     : slv(63 downto 0);
      rorQueue       : slv(63 downto 0);
      kickerCount    : slv(63 downto 0);
      countReset     : sl;
      triggerData    : TriggerDataType;
      axilWriteSlave : AxiLiteWriteSlaveType;
      axilReadSlave  : AxiLiteReadSlaveType;
   end record RegType;

   constant REG_INIT_C : RegType := (
      rorPattern     => (others => '0'),
      rorQueue       => (others => '0'),
      kickerCount    => (others => '0'),
      countReset     => '0',
      triggerData    => TRIGGER_DATA_INIT_C,
      axilWriteSlave => AXI_LITE_WRITE_SLAVE_INIT_C,
      axilReadSlave  => AXI_LITE_READ_SLAVE_INIT_C);

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   signal syncAxilReadMaster  : AxiLiteReadMasterType;
   signal syncAxilReadSlave   : AxiLiteReadSlaveType;
   signal syncAxilWriteMaster : AxiLiteWriteMasterType;
   signal syncAxilWriteSlave  : AxiLiteWriteSlaveType;

begin

   U_AxiLiteAsync_1 : entity surf.AxiLiteAsync
      generic map (
         TPD_G         => TPD_G,
         COMMON_CLK_G  => AXIL_CLK_IS_FC_CLK_G,
         PIPE_STAGES_G => 0)
      port map (
         sAxiClk         => axilClk,              -- [in]
         sAxiClkRst      => axilRst,              -- [in]
         sAxiReadMaster  => axilReadMaster,       -- [in]
         sAxiReadSlave   => axilReadSlave,        -- [out]
         sAxiWriteMaster => axilWriteMaster,      -- [in]
         sAxiWriteSlave  => axilWriteSlave,       -- [out]
         mAxiClk         => fcClk,                -- [in]
         mAxiClkRst      => fcRst,                -- [in]
         mAxiReadMaster  => syncAxilReadMaster,   -- [out]
         mAxiReadSlave   => syncAxilReadSlave,    -- [in]
         mAxiWriteMaster => syncAxilWriteMaster,  -- [out]
         mAxiWriteSlave  => syncAxilWriteSlave);  -- [in]




   comb : process (fcBus, fcRst, lclsTimingBus, r, syncAxilReadMaster, syncAxilWriteMaster) is
      variable v      : RegType;
      variable axilEp : AxiLiteEndPointType;
   begin
      v := r;

      v.countReset        := '0';
      v.triggerData.valid := '0';
      v.triggerData.bc0   := '0';

      ----------------------------------------------------------------------------------------------
      -- Axi Lite
      ----------------------------------------------------------------------------------------------
      axiSlaveWaitTxn(axilEp, syncAxilWriteMaster, syncAxilReadMaster, v.axilWriteSlave, v.axilReadSlave);

      axiSlaveRegister(axilEp, X"00", 0, v.rorPattern);
      axiSlaveRegisterR(axilEp, X"08", 0, r.kickerCount);
      axiSlaveRegister(axilEp, X"10", 0, v.countReset);

      axiSlaveDefault(axilEp, v.axilWriteSlave, v.axilReadSlave, AXI_RESP_DECERR_C);

      ----------------------------------------------------------------------------------------------
      -- Trigger decisions
      ----------------------------------------------------------------------------------------------
      if (fcBus.bc0 = '1') then
         v.triggerData.valid := '1';
         v.triggerData.bc0   := '1';
      end if;

      if (fcBus.bunchStrobe = '1') then
         v.triggerData.valid := '1';
         v.rorQueue          := '0' & r.rorQueue(63 downto 1);
      end if;

      if (lclsTimingBus.strobe = '1') then
         v.triggerData.data(0) := lclsTimingBus.message.control(3)(1);  -- 10 Hz S30XL Kicker trigger
         if (v.triggerData.data(0) = '1') then
            -- Load pattern into shift reg
            v.rorQueue    := r.rorPattern;
            -- Increment kicker count
            v.kickerCount := r.kickerCount + 1;
         end if;
      end if;

      v.triggerData.data(1) := v.rorQueue(0);

      if (r.countReset = '1') then
         v.kickerCount := (others => '0');
      end if;

      triggerData        <= r.triggerData;
      syncAxilWriteSlave <= r.axilWriteSlave;
      syncAxilReadSlave  <= r.axilReadSlave;

      if (fcRst = '1') then
         v := REG_INIT_C;
      end if;

      rin <= v;

   end process comb;

   seq : process (fcClk) is
   begin
      if (rising_edge(fcClk)) then
         r <= rin after TPD_G;
      end if;
   end process seq;



end architecture rtl;
