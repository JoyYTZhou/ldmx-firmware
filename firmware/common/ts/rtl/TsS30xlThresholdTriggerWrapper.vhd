-------------------------------------------------------------------------------
-- Title      : HitProducerStream Wrapper
-------------------------------------------------------------------------------
-- Company    : SLAC National Accelerator Laboratory
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: VHDL Wrapper for TS HLS code
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

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;
use ldmx_tdaq.TriggerPkg.all;

library ldmx_ts;
use ldmx_ts.TsPkg.all;



entity TsS30xlThresholdTriggerWrapper is

   generic (
      TPD_G : time := 1 ns);
   port (
      fcClk185       : in  sl;
      fcRst185       : in  sl;
      fcTsMsg        : in  TsData6ChMsgArray(1 downto 0);
      fcMsgTimestamp : in  FcTimestampType;
      daqData        : out TsS30xlThresholdTriggerDaqType;
      gtData         : out TriggerDataType;

      axilClk         : in  sl;
      axilRst         : in  sl;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType);

end entity TsS30xlThresholdTriggerWrapper;

architecture rtl of TsS30xlThresholdTriggerWrapper is
   component ts_s30xl_threshold_trigger_hw is
      port (
         ap_clk                 : in  std_logic;
         ap_rst_n               : in  std_logic;
         timestamp_in           : in  std_logic_vector (63 downto 0);
         bc0_in                 : in  sl;
         timestamp_out          : out std_logic_vector (63 downto 0);
         bc0_out                : out sl;
         dataReady_in           : in  std_logic_vector (0 downto 0);
         dataReady_out          : out std_logic_vector (0 downto 0);
         FIFO_0                 : in  std_logic_vector (13 downto 0);
         FIFO_1                 : in  std_logic_vector (13 downto 0);
         FIFO_2                 : in  std_logic_vector (13 downto 0);
         FIFO_3                 : in  std_logic_vector (13 downto 0);
         FIFO_4                 : in  std_logic_vector (13 downto 0);
         FIFO_5                 : in  std_logic_vector (13 downto 0);
         FIFO_6                 : in  std_logic_vector (13 downto 0);
         FIFO_7                 : in  std_logic_vector (13 downto 0);
         FIFO_8                 : in  std_logic_vector (13 downto 0);
         FIFO_9                 : in  std_logic_vector (13 downto 0);
         FIFO_10                : in  std_logic_vector (13 downto 0);
         FIFO_11                : in  std_logic_vector (13 downto 0);
         onflag_0               : out std_logic_vector (0 downto 0);
         onflag_1               : out std_logic_vector (0 downto 0);
         onflag_2               : out std_logic_vector (0 downto 0);
         onflag_3               : out std_logic_vector (0 downto 0);
         onflag_4               : out std_logic_vector (0 downto 0);
         onflag_5               : out std_logic_vector (0 downto 0);
         onflag_6               : out std_logic_vector (0 downto 0);
         onflag_7               : out std_logic_vector (0 downto 0);
         onflag_8               : out std_logic_vector (0 downto 0);
         onflag_9               : out std_logic_vector (0 downto 0);
         onflag_10              : out std_logic_vector (0 downto 0);
         onflag_11              : out std_logic_vector (0 downto 0);
         amplitude_0            : out std_logic_vector (16 downto 0);
         amplitude_1            : out std_logic_vector (16 downto 0);
         amplitude_2            : out std_logic_vector (16 downto 0);
         amplitude_3            : out std_logic_vector (16 downto 0);
         amplitude_4            : out std_logic_vector (16 downto 0);
         amplitude_5            : out std_logic_vector (16 downto 0);
         amplitude_6            : out std_logic_vector (16 downto 0);
         amplitude_7            : out std_logic_vector (16 downto 0);
         amplitude_8            : out std_logic_vector (16 downto 0);
         amplitude_9            : out std_logic_vector (16 downto 0);
         amplitude_10           : out std_logic_vector (16 downto 0);
         amplitude_11           : out std_logic_vector (16 downto 0);
         s_axi_CTRL_BUS_AWVALID : in  std_logic;
         s_axi_CTRL_BUS_AWREADY : out std_logic;
         s_axi_CTRL_BUS_AWADDR  : in  std_logic_vector (5-1 downto 0);
         s_axi_CTRL_BUS_WVALID  : in  std_logic;
         s_axi_CTRL_BUS_WREADY  : out std_logic;
         s_axi_CTRL_BUS_WDATA   : in  std_logic_vector (32-1 downto 0);
         s_axi_CTRL_BUS_WSTRB   : in  std_logic_vector (32/8-1 downto 0);
         s_axi_CTRL_BUS_ARVALID : in  std_logic;
         s_axi_CTRL_BUS_ARREADY : out std_logic;
         s_axi_CTRL_BUS_ARADDR  : in  std_logic_vector (5-1 downto 0);
         s_axi_CTRL_BUS_RVALID  : out std_logic;
         s_axi_CTRL_BUS_RREADY  : in  std_logic;
         s_axi_CTRL_BUS_RDATA   : out std_logic_vector (32-1 downto 0);
         s_axi_CTRL_BUS_RRESP   : out std_logic_vector (1 downto 0);
         s_axi_CTRL_BUS_BVALID  : out std_logic;
         s_axi_CTRL_BUS_BREADY  : in  std_logic;
         s_axi_CTRL_BUS_BRESP   : out std_logic_vector (1 downto 0));
   end component;

   signal inputValid : sl;

   signal outputValidInt : sl;

   signal fcMsgTimestampSlv        : slv(FC_TIMESTAMP_SIZE_C-1 downto 0);
   signal fcMsgTimestampDelayedSlv : slv(FC_TIMESTAMP_SIZE_C-1 downto 0);

   signal fifoIn : Slv14Array(11 downto 0);

   signal channelHits       : slv(11 downto 0);
   signal channelAmplitudes : slv17Array(11 downto 0);

   signal bc0Out : sl;

   signal rstL : sl;

   signal daqDataInt          : TsS30xlThresholdTriggerDaqType;
   signal syncAxilReadMaster  : AxiLiteReadMasterType;
   signal syncAxilReadSlave   : AxiLiteReadSlaveType;
   signal syncAxilWriteMaster : AxiLiteWriteMasterType;
   signal syncAxilWriteSlave  : AxiLiteWriteSlaveType;

begin

   U_AxiLiteAsync_1 : entity surf.AxiLiteAsync
      generic map (
         TPD_G         => TPD_G,
         COMMON_CLK_G  => false,
         PIPE_STAGES_G => 0)
      port map (
         sAxiClk         => axilClk,              -- [in]
         sAxiClkRst      => axilRst,              -- [in]
         sAxiReadMaster  => axilReadMaster,       -- [in]
         sAxiReadSlave   => axilReadSlave,        -- [out]
         sAxiWriteMaster => axilWriteMaster,      -- [in]
         sAxiWriteSlave  => axilWriteSlave,       -- [out]
         mAxiClk         => fcClk185,             -- [in]
         mAxiClkRst      => fcRst185,             -- [in]
         mAxiReadMaster  => syncAxilReadMaster,   -- [out]
         mAxiReadSlave   => syncAxilReadSlave,    -- [in]
         mAxiWriteMaster => syncAxilWriteMaster,  -- [out]
         mAxiWriteSlave  => syncAxilWriteSlave);  -- [in]   

   inputValid        <= fcMsgTimestamp.valid;
   fcMsgTimestampSlv <= toSlv(fcMsgTimestamp);

   FifoIn(0)  <= fcTsMsg(0).adc(0) & fcTsMsg(0).tdc(0);  -- [IN]
   FifoIn(1)  <= fcTsMsg(0).adc(1) & fcTsMsg(0).tdc(1);  -- [IN]
   FifoIn(2)  <= fcTsMsg(0).adc(2) & fcTsMsg(0).tdc(2);  -- [IN]
   FifoIn(3)  <= fcTsMsg(0).adc(3) & fcTsMsg(0).tdc(3);  -- [IN]
   FifoIn(4)  <= fcTsMsg(0).adc(4) & fcTsMsg(0).tdc(4);  -- [IN]
   FifoIn(5)  <= fcTsMsg(0).adc(5) & fcTsMsg(0).tdc(5);  -- [IN]
   FifoIn(6)  <= fcTsMsg(1).adc(0) & fcTsMsg(1).tdc(0);  -- [IN]
   FifoIn(7)  <= fcTsMsg(1).adc(1) & fcTsMsg(1).tdc(1);  -- [IN]
   FifoIn(8)  <= fcTsMsg(1).adc(2) & fcTsMsg(1).tdc(2);  -- [IN]
   FifoIn(9)  <= fcTsMsg(1).adc(3) & fcTsMsg(1).tdc(3);  -- [IN]
   FifoIn(10) <= fcTsMsg(1).adc(5) & fcTsMsg(1).tdc(4);  -- [IN]
   FifoIn(11) <= fcTsMsg(1).adc(4) & fcTsMsg(1).tdc(5);  -- [IN]

   rstL <= not fcRst185;


   U_ts_s30xl_threshold_trigger_hw_1 : ts_s30xl_threshold_trigger_hw
      port map (
         ap_clk                 => fcClk185,                  -- [IN]
         ap_rst_n               => rstL,                  -- [IN]
         timestamp_in           => fcMsgTimestampSlv,         -- [IN]
         bc0_in                 => fcTsMsg(0).bc0,            -- [in]
         timestamp_out          => fcMsgTimestampDelayedSlv,  -- [OUT]
         bc0_out                => bc0Out,                    -- [out]
         dataReady_in(0)        => inputValid,                -- [IN]
         dataReady_out(0)       => outputValidInt,            -- [OUT]
         FIFO_0                 => fifoIn(0),                 -- [IN]
         FIFO_1                 => fifoIn(1),                 -- [IN]
         FIFO_2                 => fifoIn(2),                 -- [IN]
         FIFO_3                 => fifoIn(3),                 -- [IN]
         FIFO_4                 => fifoIn(4),                 -- [IN]
         FIFO_5                 => fifoIn(5),                 -- [IN]
         FIFO_6                 => fifoIn(6),                 -- [IN]
         FIFO_7                 => fifoIn(7),                 -- [IN]
         FIFO_8                 => fifoIn(8),                 -- [IN]
         FIFO_9                 => fifoIn(9),                 -- [IN]
         FIFO_10                => fifoIn(10),                -- [IN]
         FIFO_11                => fifoIn(11),                -- [IN]
         onflag_0(0)            => channelHits(0),            -- [OUT]
         onflag_1(0)            => channelHits(1),            -- [OUT]
         onflag_2(0)            => channelHits(2),            -- [OUT]
         onflag_3(0)            => channelHits(3),            -- [OUT]
         onflag_4(0)            => channelHits(4),            -- [OUT]
         onflag_5(0)            => channelHits(5),            -- [OUT]
         onflag_6(0)            => channelHits(6),            -- [OUT]
         onflag_7(0)            => channelHits(7),            -- [OUT]
         onflag_8(0)            => channelHits(8),            -- [OUT]
         onflag_9(0)            => channelHits(9),            -- [OUT]
         onflag_10(0)           => channelHits(10),           -- [OUT]
         onflag_11(0)           => channelHits(11),           -- [OUT]
         amplitude_0            => channelAmplitudes(0),      -- [OUT]
         amplitude_1            => channelAmplitudes(1),      -- [OUT]
         amplitude_2            => channelAmplitudes(2),      -- [OUT]
         amplitude_3            => channelAmplitudes(3),      -- [OUT]
         amplitude_4            => channelAmplitudes(4),      -- [OUT]
         amplitude_5            => channelAmplitudes(5),      -- [OUT]
         amplitude_6            => channelAmplitudes(6),      -- [OUT]
         amplitude_7            => channelAmplitudes(7),      -- [OUT]
         amplitude_8            => channelAmplitudes(8),      -- [OUT]
         amplitude_9            => channelAmplitudes(9),      -- [OUT]
         amplitude_10           => channelAmplitudes(10),     -- [OUT]
         amplitude_11           => channelAmplitudes(11),     -- [OUT]
         s_axi_CTRL_BUS_AWVALID => syncAxilWriteMaster.awvalid,
         s_axi_CTRL_BUS_AWREADY => syncAxilWriteSlave.awready,
         s_axi_CTRL_BUS_AWADDR  => syncAxilWriteMaster.awaddr(4 downto 0),
         s_axi_CTRL_BUS_WVALID  => syncAxilWriteMaster.wvalid,
         s_axi_CTRL_BUS_WREADY  => syncAxilWriteSlave.wready,
         s_axi_CTRL_BUS_WDATA   => syncAxilWriteMaster.wdata,
         s_axi_CTRL_BUS_WSTRB   => syncAxilWriteMaster.wstrb,
         s_axi_CTRL_BUS_ARVALID => syncAxilReadMaster.arvalid,
         s_axi_CTRL_BUS_ARREADY => syncAxilReadSlave.arready,
         s_axi_CTRL_BUS_ARADDR  => syncAxilReadMaster.araddr(4 downto 0),
         s_axi_CTRL_BUS_RVALID  => syncAxilReadSlave.rvalid,
         s_axi_CTRL_BUS_RREADY  => syncAxilReadMaster.rready,
         s_axi_CTRL_BUS_RDATA   => syncAxilReadSlave.rdata,
         s_axi_CTRL_BUS_RRESP   => syncAxilReadSlave.rresp,
         s_axi_CTRL_BUS_BVALID  => syncAxilWriteSlave.bvalid,
         s_axi_CTRL_BUS_BREADY  => syncAxilWriteMaster.bready,
         s_axi_CTRL_BUS_BRESP   => syncAxilWriteSlave.bresp);

   daqDataInt.valid      <= outputValidInt;
   daqDataInt.bc0        <= bc0Out;
   daqDataInt.timestamp  <= toFcTimestamp(fcMsgTimestampDelayedSlv, outputValidInt);
   daqDataInt.hits       <= channelHits;
   daqDataInt.amplitudes <= channelAmplitudes;

   daqData <= daqDataInt;
   gtData  <= toTriggerData(daqDataInt);

end architecture rtl;


