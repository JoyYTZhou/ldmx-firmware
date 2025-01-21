-------------------------------------------------------------------------------
-- Title      : 
-------------------------------------------------------------------------------
-- Company    : SLAC National Accelerator Laboratory
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- This file is part of LDMX. It is subject to
-- the license terms in the LICENSE.txt file found in the top-level directory
-- of this distribution and at:
--    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
-- No part of LDMX, including this file, may be
-- copied, modified, propagated, or distributed except according to the terms
-- contained in the LICENSE.txt file.
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;
use surf.AxiStreamPkg.all;

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;
use ldmx_tdaq.TriggerPkg.all;

library ldmx_ts;
use ldmx_ts.TsPkg.all;


entity S30xlGlobalTriggerLogic is

   generic (
      TPD_G : time := 1 ns);
   port (
      -----------------------------
      -- Raw Trigger Data In
      -----------------------------
      lclsTimingClk          : in sl;
      lclsTimingRst          : in sl;
      tsThresholdTriggerData : in TriggerDataType;
      synTriggerData         : in TriggerDataType;
      kickerTriggerData      : in TriggerDataType;
      triggerTimestamp       : in FcTimestampType;
      fcBus                  : in FcBusType;

      --------
      -- Outut
      --------
      gtRor           : out FcTimestampType;
      gtDaqAxisMaster : out AxiStreamMasterType;
      gtDaqAxisSlave  : in  AxiStreamSlaveType;

      -- Axil inteface
--       axilClk         : in  sl;
--       axilRst         : in  sl;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType  := AXI_LITE_READ_SLAVE_EMPTY_DECERR_C;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType := AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);

end entity S30xlGlobalTriggerLogic;

architecture rtl of S30xlGlobalTriggerLogic is

   constant MIN_ROR_PERIOD_C : slv(3 downto 0) := toSlv(3, 4);

   type RegType is record
      counter                   : slv(3 downto 0);
      enableKickerTriggers      : sl;
      enableSynTriggers         : sl;
      enableTsThresholdTriggers : sl;
      thresholdRorPattern       : slv(63 downto 0);
      thresholdRorQueue         : slv(63 downto 0);
      gtRor                     : FcTimestampType;
      axilReadSlave             : AxiLiteReadSlaveType;
      axilWriteSlave            : AxiLiteWriteSlaveType;
   end record RegType;

   constant REG_INIT_C : RegType := (
      counter                   => (others => '0'),
      enableKickerTriggers      => '0',
      enableSynTriggers         => '0',
      enableTsThresholdTriggers => '0',
      thresholdRorPattern       => (others => '0'),
      thresholdRorQueue         => (others => '0'),
      gtRor                     => FC_TIMESTAMP_INIT_C,
      axilReadSlave             => AXI_LITE_READ_SLAVE_INIT_C,
      axilWriteSlave            => AXI_LITE_WRITE_SLAVE_INIT_C);

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   signal tsS30xlThresholdTriggerDaq : TsS30xlThresholdTriggerDaqType;
   signal synTriggerMessage          : FcMessageType;

begin

   tsS30xlThresholdTriggerDaq <= toThresholdTriggerDaq(tsThresholdTriggerData, triggerTimestamp);
   synTriggerMessage          <= toFcMessage(synTriggerData.data(FC_LEN_C-1 downto 0), synTriggerData.valid);

   comb : process (axilReadMaster, axilWriteMaster, fcBus, kickerTriggerData, r, synTriggerData,
                   triggerTimestamp, tsS30xlThresholdTriggerDaq) is
      variable v      : RegType;
      variable axilEp : AxiLiteEndpointType;
   begin
      v := r;

      ----------------------------------------------------------------------------------------------
      -- AXI Lite Registers
      ----------------------------------------------------------------------------------------------
      axiSlaveWaitTxn(axilEp, axilWriteMaster, axilReadMaster, v.axilWriteSlave, v.axilReadSlave);

      axiSlaveRegister (axilEp, x"00", 0, v.enableSynTriggers);
      axiSlaveRegister (axilEp, x"00", 1, v.enableTsThresholdTriggers);
      axiSlaveRegister (axilEp, x"00", 2, v.enableKickerTriggers);
      axiSlaveRegister(axilEp, X"10", 0, v.thresholdRorPattern);
      axiSlaveRegisterR(axilEp, X"20", 0, r.thresholdRorQueue);


      -- Closeout the transaction
      axiSlaveDefault(axilEp, v.axilWriteSlave, v.axilReadSlave, AXI_RESP_DECERR_C);


      v.gtRor.valid := '0';

      -- Count down to achieve minimum ror spacing
      if (r.counter /= 0) then
         v.counter := v.counter - 1;
      end if;

      if (r.counter = 0) then

         -- Special case for bc0
         -- If BC0 seen send a ROR         
         if (synTriggerData.bc0 = '1') then
            v.counter     := MIN_ROR_PERIOD_C;
            v.gtRor       := triggerTimestamp;
            v.gtRor.valid := '1';
         end if;

         -- Gate triggers unless in RUNNING state
         if (fcBus.runState = RUN_STATE_RUNNING_C) then

            -- TS Triggering
            if (r.enableTsThresholdTriggers = '1' and tsS30xlThresholdTriggerDaq.valid = '1') then
               if (tsS30xlThresholdTriggerDaq.hits /= 0 or tsS30xlThresholdTriggerDaq.bc0 = '1') then
                  v.counter           := MIN_ROR_PERIOD_C;
                  v.gtRor             := triggerTimestamp;
                  v.gtRor.valid       := '1';
--                   v.thresholdRorQueue := r.thresholdRorPattern;
--                elsif (r.thresholdRorQueue /= 0) then
--                   v.counter           := MIN_ROR_PERIOD_C;
--                   v.gtRor             := triggerTimestamp;
--                   v.gtRor.valid       := r.thresholdRorQueue(0);
--                   v.thresholdRorQueue := '0' & r.thresholdRorQueue(63 downto 1);
               end if;
            end if;

            -- Synthetic triggering
            if (r.enableSynTriggers = '1' and synTriggerData.valid = '1' and synTriggerData.data(0) = '1') then
               v.counter     := MIN_ROR_PERIOD_C;
               v.gtRor       := triggerTimestamp;
               v.gtRor.valid := '1';
            end if;

            -- Kicker triggering
            if (r.enableKickerTriggers = '1' and kickerTriggerData.valid = '1' and kickerTriggerData.data(1) = '1') then
               v.counter     := MIN_ROR_PERIOD_C;
               v.gtRor       := triggerTimestamp;
               v.gtRor.valid := '1';
            end if;


         end if;

      end if;

      rin <= v;

      gtRor          <= r.gtRor;
      axilReadSlave  <= r.axilReadSlave;
      axilWriteSlave <= r.axilWriteSlave;

   end process;


-- Have to use async reset since recovered lcls clock can drop out
   seq : process (lclsTimingClk, lclsTimingRst) is
   begin
      if (lclsTimingRst = '1') then
         r <= REG_INIT_C after TPD_G;
      elsif (rising_edge(lclsTimingClk)) then
         r <= rin after TPD_G;
      end if;
   end process seq;

end architecture rtl;
