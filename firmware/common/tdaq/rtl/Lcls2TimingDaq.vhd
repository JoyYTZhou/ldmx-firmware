-------------------------------------------------------------------------------
-- File       : Lcls2TimingDaq.vhd
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
use surf.AxiStreamPkg.all;
use surf.Pgp2fcPkg.all;
use surf.EthMacPkg.all;
use surf.SsiPkg.all;

library lcls_timing_core;
use lcls_timing_core.TimingPkg.all;

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;
use ldmx_tdaq.DaqPkg.all;

library ldmx_ts;
use ldmx_ts.TsPkg.all;

entity Lcls2TimingDaq is

   generic (
      TPD_G            : time             := 1 ns;
      AXIL_BASE_ADDR_G : slv(31 downto 0) := (others => '0'));
   port (
      -- LCLS Timing and LDMX FC busses
      lclsTimingClk : in sl;
      lclsTimingRst : in sl;
      lclsTimingBus : in TimingBusType;
      fcBus         : in FcBusType;

      -- AXI-Lite Interface (axilClk domain)
      axilClk         : in  sl;
      axilRst         : in  sl;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType;

      -- Streaming interface to ETH
      axisClk         : in  sl;
      axisRst         : in  sl;
      eventAxisMaster : out AxiStreamMasterType;
      eventAxisSlave  : in  AxiStreamSlaveType);

end entity Lcls2TimingDaq;

architecture rtl of Lcls2TimingDaq is

   constant AXIS_CFG_C : AxiStreamConfigType := ssiAxiStreamConfig(dataBytes => 32, tDestBits => 0);

   type StateType is (WAIT_ROR_S, DO_DATA_S, TAIL_S);

   type RegType is record
      rorFifoRdEn : sl;
      rorFifoRst  : sl;
      pulseIdSent : slv(15 downto 0);
      fcBus       : FcBusType;
      axisMaster  : AxiStreamMasterType;
   end record RegType;

   constant REG_INIT_C : RegType := (
      rorFifoRdEn => '0',
      rorFifoRst  => '0',
      pulseIdSent => (others => '0'),
      fcBus       => FC_BUS_INIT_C,
      axisMaster  => axiStreamMasterInit(AXIS_CFG_C));

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   signal axisCtrl : AxiStreamCtrlType;

   signal rorFifoTimestamp : FcTimestampType;
   signal rorFifoValid     : sl;
   signal timingMessageSlv : slv(255 downto 0);
   signal ramRdData        : slv(255 downto 0);

   constant NUM_AXIL_C          : natural := 2;
   constant AXIL_LOC_C          : natural := 0;
   constant AXIL_EVENT_FORMAT_C : natural := 1;

   constant AXIL_XBAR_CFG_C : AxiLiteCrossbarMasterConfigArray(NUM_AXIL_C-1 downto 0) := (
      AXIL_LOC_C          => (
         baseAddr         => AXIL_BASE_ADDR_G + X"000",
         addrBits         => 8,
         connectivity     => X"FFFF"),
      AXIL_EVENT_FORMAT_C => (
         baseAddr         => AXIL_BASE_ADDR_G + X"100",
         addrBits         => 8,
         connectivity     => X"FFFF"));


   signal locAxilWriteMasters : AxiLiteWriteMasterArray(NUM_AXIL_C-1 downto 0);
   signal locAxilWriteSlaves  : AxiLiteWriteSlaveArray(NUM_AXIL_C-1 downto 0) := (others => AXI_LITE_WRITE_SLAVE_EMPTY_DECERR_C);
   signal locAxilReadMasters  : AxiLiteReadMasterArray(NUM_AXIL_C-1 downto 0);
   signal locAxilReadSlaves   : AxiLiteReadSlaveArray(NUM_AXIL_C-1 downto 0)  := (others => AXI_LITE_READ_SLAVE_EMPTY_DECERR_C);

   signal syncAxilWriteMaster : AxiLiteWriteMasterType;
   signal syncAxilWriteSlave  : AxiLiteWriteSlaveType;
   signal syncAxilReadMaster  : AxiLiteReadMasterType;
   signal syncAxilReadSlave   : AxiLiteReadSlaveType;

   function toSlv (
      msg : TimingMessageType)
      return slv is
      variable ret : slv(255 downto 0);
   begin
      ret                 := (others => '0');
      ret(63 downto 0)    := msg.pulseId;
      ret(127 downto 64)  := msg.timeStamp;
      ret(159 downto 128) := msg.beamRequest;
      ret(175 downto 160) := resize(msg.fixedRates, 16);
      ret(191 downto 176) := msg.control(3);
      return ret;
   end function toSlv;


begin

   -------------------------------------------------------------------------------------------------
   -- AXIL Crossbar
   -------------------------------------------------------------------------------------------------
   U_XBAR : entity surf.AxiLiteCrossbar
      generic map (
         TPD_G              => TPD_G,
         NUM_SLAVE_SLOTS_G  => 1,
         NUM_MASTER_SLOTS_G => NUM_AXIL_C,
         MASTERS_CONFIG_G   => AXIL_XBAR_CFG_C)
      port map (
         axiClk              => axilClk,
         axiClkRst           => axilRst,
         sAxiWriteMasters(0) => axilWriteMaster,
         sAxiWriteSlaves(0)  => axilWriteSlave,
         sAxiReadMasters(0)  => axilReadMaster,
         sAxiReadSlaves(0)   => axilReadSlave,
         mAxiWriteMasters    => locAxilWriteMasters,
         mAxiWriteSlaves     => locAxilWriteSlaves,
         mAxiReadMasters     => locAxilReadMasters,
         mAxiReadSlaves      => locAxilReadSlaves);

   U_AxiLiteAsync : entity surf.AxiLiteAsync
      generic map (
         TPD_G        => TPD_G,
         COMMON_CLK_G => false)
      port map (
         sAxiClk         => axilClk,                          -- [in]
         sAxiClkRst      => axilRst,                          -- [in]
         sAxiReadMaster  => locAxilReadMasters(AXIL_LOC_C),   -- [in]
         sAxiReadSlave   => locAxilReadSlaves(AXIL_LOC_C),    -- [out]
         sAxiWriteMaster => locAxilWriteMasters(AXIL_LOC_C),  -- [in]
         sAxiWriteSlave  => locAxilWriteSlaves(AXIL_LOC_C),   -- [out]
         mAxiClk         => lclsTimingClk,                    -- [in]
         mAxiClkRst      => lclsTimingRst,                    -- [in]
         mAxiReadMaster  => syncAxilReadMaster,               -- [out]
         mAxiReadSlave   => syncAxilReadSlave,                -- [in]
         mAxiWriteMaster => syncAxilWriteMaster,              -- [out]
         mAxiWriteSlave  => syncAxilWriteSlave);              -- [in]

   -------------------------------------------------------------------------------------------------
   -- Buffer readout requests
   -------------------------------------------------------------------------------------------------
   U_FcTimestampFifo_1 : entity ldmx_tdaq.FcTimestampFifo
      generic map (
         TPD_G           => TPD_G,
         GEN_SYNC_FIFO_G => false,
         SYNTH_MODE_G    => "inferred",
         MEMORY_TYPE_G   => "distributed",
         ADDR_WIDTH_G    => 7)
      port map (
         rst         => r.rorFifoRst,          -- [in]
         wrClk       => lclsTimingClk,         -- [in]
         wrFull      => open,                  -- [out]
         wrTimestamp => fcBus.readoutRequest,  -- [in]
         rdClk       => lclsTimingClk,         -- [in]
         rdEn        => r.rorFifoRdEn,         -- [in]
         rdTimestamp => rorFifoTimestamp,      -- [out]
         rdValid     => rorFifoValid);         -- [out]


   -------------------------------------------------------------------------------------------------
   -- Buffer Timing messages
   -------------------------------------------------------------------------------------------------
   -- Buffer and delay incoming data to ROR
   timingMessageSlv <= toSlv(lclsTimingBus.message);
   U_SimpleDualPortRam_DATA : entity surf.SimpleDualPortRam
      generic map (
         TPD_G         => TPD_G,
         MEMORY_TYPE_G => "distributed",
         DOB_REG_G     => false,
         BYTE_WR_EN_G  => false,
         DATA_WIDTH_G  => 256,
         ADDR_WIDTH_G  => 4)
      port map (
         clka   => lclsTimingClk,                              -- [in]
         ena    => '1',                                        -- [in]
         wea    => lclsTimingBus.strobe,                       -- [in]
         addra  => lclsTimingBus.message.pulseId(3 downto 0),  -- [in]
         dina   => timingMessageSlv,                           -- [in]
         clkb   => lclsTimingClk,                              -- [in]
         enb    => '1',                                        -- [in]
         regceb => '1',                                        -- [in]
         rstb   => '0',                                        -- [in]
         addrb  => rorFifoTimestamp.pulseId(3 downto 0),       -- [in]
         doutb  => ramRdData);                                 -- [out]


   comb : process (fcBus, lclsTimingBus, r, ramRdData, rorFifoTimestamp, rorFifoValid) is
      variable v : RegType;
   begin
      v := r;

      v.axisMaster                 := axiStreamMasterInit(AXIS_CFG_C);
      v.rorFifoRdEn                := '0';
      v.rorFifoRst                 := '0';
      v.fcBus.readoutRequest.valid := '0';

      -- Needed by DaqEventFormatter to reset the FIFO
      v.fcBus.runState := fcBus.runState;


      -- Incomming timing messages overwite any sent flags
      if (lclsTimingBus.strobe = '1') then
         v.pulseIdSent(conv_integer(lclsTimingBus.message.pulseId(3 downto 0))) := '0';
      end if;

      -- Read RoRs from FIFO, associate with LCLS Timing Data
      -- If LCLS Timing Data already sent, just burn the ROR
      if (rorFifoValid = '1') then
         v.rorFifoRdEn := '1';

         if (r.pulseIdSent(conv_integer(rorFifoTimestamp.pulseId(3 downto 0))) = '0') then
            -- Generate an AXI-Stream frame with LCLS Timing Data
            v.axisMaster.tValid                                               := '1';
            v.axisMaster.tData(255 downto 0)                                  := ramRdData;
            v.axisMaster.tLast                                                := '1';
            -- Pass readout request to DaqEventFormatter
            v.fcBus.readoutRequest                                            := rorFifoTimestamp;
            v.fcBus.readoutRequest.valid                                      := '1';
            -- Mark this pulseID as sent
            v.pulseIdSent(conv_integer(rorFifoTimestamp.pulseId(3 downto 0))) := '1';
         end if;

      end if;

      -- Clear FIFO in reset state
      if (fcBus.runState = RUN_STATE_RESET_C) then
         v.rorFifoRst := '1';
      end if;


      rin <= v;

   end process comb;

   seq : process (lclsTimingClk) is
   begin
      if (rising_edge(lclsTimingClk)) then
         r <= rin after TPD_G;
      end if;
   end process seq;

   U_DaqEventFormatter_1 : entity ldmx_tdaq.DaqEventFormatter
      generic map (
         TPD_G                     => TPD_G,
         SUBSYSTEM_ID_G            => TDAQ_TRIGGER_SUBSYSTEM_ID_C,
         CONTRIBUTOR_ID_G          => X"03",
         RAW_AXIS_CFG_G            => AXIS_CFG_C,
         EVENT_FIFO_PAUSE_THRESH_G => 2**9-16,
         EVENT_FIFO_ADDR_WIDTH_G   => 9,
         EVENT_FIFO_SYNTH_MODE_G   => "inferred",
         EVENT_FIFO_MEMORY_TYPE_G  => "block")
      port map (
         fcClk185        => lclsTimingClk,                             -- [in]
         fcRst185        => lclsTimingRst,                             -- [in]
         fcBus           => r.fcBus,                                   -- [in]
         axilClk         => axilClk,                                   -- [in]
         axilRst         => axilRst,                                   -- [in]
         axilReadMaster  => locAxilReadMasters(AXIL_EVENT_FORMAT_C),   -- [in]
         axilReadSlave   => locAxilReadSlaves(AXIL_EVENT_FORMAT_C),    -- [out]
         axilWriteMaster => locAxilWriteMasters(AXIL_EVENT_FORMAT_C),  -- [in]
         axilWriteSlave  => locAxilWriteSlaves(AXIL_EVENT_FORMAT_C),   -- [out]
         rawAxisClk      => lclsTimingClk,                             -- [in]
         rawAxisRst      => lclsTimingRst,                             -- [in]
         rawAxisMaster   => r.axisMaster,                              -- [in]
         rawAxisCtrl     => axisCtrl,                                  -- [out]
         eventAxisClk    => axisClk,                                   -- [in]
         eventAxisRst    => axisRst,                                   -- [in]
         eventAxisMaster => eventAxisMaster,                           -- [out]
         eventAxisSlave  => eventAxisSlave);                           -- [in]   

end architecture rtl;
