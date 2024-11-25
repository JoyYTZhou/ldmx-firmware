-------------------------------------------------------------------------------
-- Title      : Testbench for design "zCCM"
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

library surf;
use surf.StdRtlPkg.all;

library ruckus;
use ruckus.BuildInfoPkg.all;

library axi_soc_ultra_plus_core;
use axi_soc_ultra_plus_core.AxiSocUltraPlusPkg.all;

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;

library ldmx_ts;
use ldmx_ts.zCCM_Pkg.ALL;

----------------------------------------------------------------------------------------------------

entity zCCMfullTb is

end entity zCCMfullTb;

----------------------------------------------------------------------------------------------------

architecture sim of zCCMfullTb is

   -------------------------------------------------------------------------------------------------
   -- Dummy Module (for VCS)
   -------------------------------------------------------------------------------------------------
   component DummyModule
      port(
         clk     : in std_logic;
         rst     : in std_logic;
         inPort  : in std_logic;
         outPort : out std_logic);
   end component;

   -------------------------------------------------------------------------------------------------
   -- Shared generics
   -------------------------------------------------------------------------------------------------
   constant TPD_G        : time          := 0.2 ns;
   constant BUILD_INFO_G : BuildInfoType := BUILD_INFO_C;

   -------------------------------------------------------------------------------------------------
   -- Bittware generics
   -------------------------------------------------------------------------------------------------
   constant SIMULATION_G         : boolean                     := true;
   constant ROGUE_SIM_EN_G       : boolean                     := true;
   constant ROGUE_SIM_PORT_NUM_G : natural range 1024 to 49151 := 11000;

   -------------------------------------------------------------------------------------------------
   -- Signals
   -------------------------------------------------------------------------------------------------
   signal sfpMgtRxP          : slv(3 downto 0) := (others => '0');
   signal sfpMgtRxN          : slv(3 downto 0) := (others => '0');
   signal sfpMgtTxP          : slv(3 downto 0) := (others => '0');
   signal sfpMgtTxN          : slv(3 downto 0) := (others => '0');
   --
   signal beamClkP           : sl := '0';
   signal beamClkN           : sl := '1';
   signal clkGenMgtClkP      : sl := '0';
   signal clkGenMgtClkN      : sl := '1';
   signal socClkRefToClkGenP : sl := '0';
   signal socClkRefToClkGenN : sl := '1';
   signal mgtRefClk1P        : sl := '0';
   signal mgtRefClk1N        : sl := '1';
   --
   signal Synth_i2c_SCL      : sl := '0';
   signal Synth_i2c_SDA      : sl := '0';
   signal Jitter_i2c_SCL     : sl := '0';
   signal Jitter_i2c_SDA     : sl := '0';

   signal RM_i2c_SDA         : slv(5 downto 0) := (others => '0');
   signal RM_i2c_SCL         : slv(5 downto 0) := (others => '0');

   signal SFP_i2c_SCL        : slv(3 downto 0) := (others => '0');
   signal SFP_i2c_SDA        : slv(3 downto 0) := (others => '0');

begin

   -- Fast Control Refclk
   U_ClkRst_REFCLK : entity surf.ClkRst
      generic map (
         CLK_PERIOD_G      => 5.3848 ns,  -- 185.714285 MHz = 5.3846 ns
         CLK_DELAY_G       => 1 ns,
         RST_START_DELAY_G => 0 ns,
         RST_HOLD_TIME_G   => 5 us,
         SYNC_RESET_G      => true)
      port map (
         clkP => clkGenMgtClkP,
         clkN => clkGenMgtClkN);

   U_zCCM_kria : entity work.zCCM_kria
      generic map (
         TPD_G                => TPD_G,
         SIMULATION_G         => SIMULATION_G,
         ROGUE_SIM_EN_G       => ROGUE_SIM_EN_G,
         ROGUE_SIM_PORT_NUM_G => ROGUE_SIM_PORT_NUM_G,
         BUILD_INFO_G         => BUILD_INFO_G)
      port map (
         MCLK_BUF_SEL            => open,               -- [out]
         MCLK_REF_P              => '0',                -- [in]
         MCLK_REF_N              => '1',                -- [in]
         MCLK_FROM_SOC_P         => open,               -- [out]
         MCLK_FROM_SOC_N         => open,               -- [out]
         BCR_FROM_SOC_P          => open,               -- [out]
         BCR_FROM_SOC_N          => open,               -- [out]
         LED_FROM_SOC_P          => open,               -- [out]
         LED_FROM_SOC_N          => open,               -- [out]

         -- clock pins to ASICs
         BEAMCLK_P               => beamClkP,           -- [out] (clean clock output ?)
         BEAMCLK_N               => beamClkN,           -- [out] (clean clock output ?)
         CLKGEN_MGTCLK_AC_P      => clkGenMgtClkP,      -- [in]  (input refclk)
         CLKGEN_MGTCLK_AC_N      => clkGenMgtClkN,      -- [in]  (input refclk)
         CLKGEN_CLK0_TO_SOC_AC_P => '0',                -- [in]
         CLKGEN_CLK0_TO_SOC_AC_N => '1',                -- [in]
         SOC_CLKREF_TO_CLKGEN_P  => socClkRefToClkGenP, -- [out] (clean clock output)
         SOC_CLKREF_TO_CLKGEN_N  => socClkRefToClkGenN, -- [out] (clean clock output)
         MGTREFCLK1_AC_P         => mgtRefClk1P,        -- [in]  (clean clock input)
         MGTREFCLK1_AC_N         => mgtRefClk1N,        -- [in]  (clean clock input)
         SYNTH_TO_SOC_AC_P       => '0',                -- [in]
         SYNTH_TO_SOC_AC_N       => '1',                -- [in]

         -- clock ASIC control
         Synth_Control_INTR      => '0',                -- [in]
         Synth_Control_LOS_XAXB  => '0',                -- [in]
         Synth_Control_LOL       => '0',                -- [in]
         Synth_Control_RST       => open,               -- [out]
         Jitter_Control_INTR     => '0',                -- [in]
         Jitter_Control_LOS_XAXB => '0',                -- [in]
         Jitter_Control_LOL      => '0',                -- [in]
         Jitter_Control_RST      => open,               -- [out]

         Synth_i2c_SCL           => Synth_i2c_SCL,      -- [inout]
         Synth_i2c_SDA           => Synth_i2c_SDA,      -- [inout]
         Jitter_i2c_SCL          => Jitter_i2c_SCL,     -- [inout]
         Jitter_i2c_SDA          => Jitter_i2c_SDA,     -- [inout]

         -- RM control signals
         RM_control_PGOOD        => (others => '1'),   -- [in]
         RM_control_PEN          => open,              -- [out]
         RM_control_RESET        => open,              -- [out]

         RM_i2c_SDA              => RM_i2c_SDA,        -- [inout]
         RM_i2c_SCL              => RM_i2c_SCL,        -- [inout]

         -- SFP data and control signals
         SFP_MGT_RX_P            => sfpMgtRxP,         -- [in]
         SFP_MGT_RX_N            => sfpMgtRxN,         -- [in]
         SFP_MGT_TX_P            => sfpMgtTxP,         -- [out]
         SFP_MGT_TX_N            => sfpMgtTxN,         -- [out]
         SFP_control_RX_LOS      => (others => '0'),   -- [in]
         SFP_control_TX_FAULT    => (others => '0'),   -- [in]
         SFP_control_MOD_ABS     => (others => '0'),   -- [in]
         SFP_control_TX_DIS      => open,              -- [out]

         SFP_i2c_SCL             => SFP_i2c_SCL,       -- [inout]
         SFP_i2c_SDA             => SFP_i2c_SDA);      -- [inout]

   -- dummy module for vcs (it has to be vhdl+verilog in some vcs versions)
   Dummy_inst: DummyModule
      port map(
         clk     => '1',
         rst     => '1',
         inPort  => '0',
         outPort => open);

   -- fiber loopback
   sfpMgtRxP <= sfpMgtTxP;
   sfpMgtRxN <= sfpMgtTxN;

   -- clock loopback (in place of jitter cleaner of actual board)
   mgtRefClk1P <= socClkRefToClkGenP;
   mgtRefClk1N <= socClkRefToClkGenN;

end architecture sim;

----------------------------------------------------------------------------------------------------
