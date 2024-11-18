library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library surf;
use surf.StdRtlPkg.all;
use surf.AxiLitePkg.all;
use surf.AxiStreamPkg.all;
use surf.EthMacPkg.all;

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;
use ldmx_tdaq.DaqPkg.all;

entity DaqEventFormatter is

   generic (
      TPD_G                     : time                  := 1 ns;
      SUBSYSTEM_ID_G            : slv(7 downto 0);
      CONTRIBUTOR_ID_G          : slv(7 downto 0);
      RAW_AXIS_CFG_G            : AxiStreamConfigType;
      EVENT_FIFO_PAUSE_THRESH_G : integer               := 1;
      EVENT_FIFO_ADDR_WIDTH_G   : integer range 4 to 48 := 8;
      EVENT_FIFO_SYNTH_MODE_G   : string                := "inferred";
      EVENT_FIFO_MEMORY_TYPE_G  : string                := "block"
      );

   port (
      -- TS Trig Data and Timing
      fcClk185 : in sl;
      fcRst185 : in sl;
      fcBus    : in FcBusType;

      -- AXI-Lite Interface (axilClk domain)
      axilClk         : in  sl;
      axilRst         : in  sl;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType;

      -- Streaming interface
      axisClk         : in  sl;
      axisRst         : in  sl;
      rawAxisMaster   : in  AxiStreamMasterType;
      rawAxisCtrl     : out AxiStreamCtrlType;
      eventAxisMaster : out AxiStreamMasterType;
      eventAxisSlave  : in  AxiStreamSlaveType);

end entity DaqEventFormatter;

architecture rtl of DaqEventFormatter is

   type StateType is (
      WAIT_ROR_S,
      DO_DATA_S);

   type RegType is record
      state            : StateType;
      rorFifoRdEn      : sl;
      burn             : sl;
      burnCount        : slv(7 downto 0);
      rawFifoAxisSlave : AxiStreamSlaveType;
      eventAxisMaster  : AxiStreamMasterType;
   end record RegType;

   constant REG_INIT_C : RegType := (
      state            => WAIT_ROR_S,
      rorFifoRdEn      => '0',
      burn             => '0',
      burnCount        => (others => '0'),
      rawFifoAxisSlave => AXI_STREAM_SLAVE_INIT_C,
      eventAxisMaster  => axiStreamMasterInit(DAQ_EVENT_AXIS_CONFIG_C));

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   signal rawFifoAxisMaster : AxiStreamMasterType;
   signal rorFifoTimestamp  : FcTimestampType;
   signal eventAxisCtrl     : AxiStreamCtrlType;

   signal fifoRst     : sl;
   signal fifoRstSync : sl;

   signal eventFifoAxisMaster : AxiStreamMasterType;
   signal eventFifoAxisSlave  : AxiStreamSlaveType;

   signal eventBatchedAxisMaster : AxiStreamMasterType;
   signal eventBatchedAxisSlave  : AxiStreamSlaveType;


begin

   fifoRst <= toSl(fcBus.runState = RUN_STATE_RESET_C);

   U_Synchronizer_1 : entity surf.Synchronizer
      generic map (
         TPD_G => TPD_G)
      port map (
         clk     => axisClk,            -- [in]
         rst     => axisRst,            -- [in]
         dataIn  => fifoRst,            -- [in]
         dataOut => fifoRstSync);       -- [out]

   -------------------------------------------------------------------------------------------------
   -- Reformat stream to standard DAQ data width
   -------------------------------------------------------------------------------------------------
   U_AxiStreamFifoV2_RAW_FIFO : entity surf.AxiStreamFifoV2
      generic map (
         TPD_G               => TPD_G,
         PIPE_STAGES_G       => 0,
         SLAVE_READY_EN_G    => false,
--          VALID_THOLD_G          => VALID_THOLD_G,
--          VALID_BURST_MODE_G     => VALID_BURST_MODE_G,
         GEN_SYNC_FIFO_G     => true,
         FIFO_FIXED_THRESH_G => true,
         FIFO_PAUSE_THRESH_G => 2**7-2,
         FIFO_ADDR_WIDTH_G   => 7,
         SYNTH_MODE_G        => "inferred",
         MEMORY_TYPE_G       => "distributed",
         SLAVE_AXI_CONFIG_G  => RAW_AXIS_CFG_G,
         MASTER_AXI_CONFIG_G => EMAC_AXIS_CONFIG_C)
      port map (
         sAxisClk    => axisClk,                -- [in]
         sAxisRst    => axisRst,                -- [in]
         sAxisMaster => rawAxisMaster,          -- [in]
         sAxisSlave  => open,                   -- [out]
         sAxisCtrl   => rawAxisCtrl,            -- [out]
         mAxisClk    => axisClk,                -- [in]
         mAxisRst    => fifoRstSync,            -- [in]
         mAxisMaster => rawFifoAxisMaster,      -- [out]
         mAxisSlave  => rin.rawFifoAxisSlave);  -- [in]

   U_FcTimestampFifo_1 : entity ldmx_tdaq.FcTimestampFifo
      generic map (
         TPD_G           => TPD_G,
         GEN_SYNC_FIFO_G => false,
         SYNTH_MODE_G    => "inferred",
         MEMORY_TYPE_G   => "distributed",
         ADDR_WIDTH_G    => 7)
      port map (
         rst         => fifoRst,               -- [in]
         wrClk       => fcClk185,              -- [in]
         wrFull      => open,                  -- [out]
         wrTimestamp => fcBus.readoutRequest,  -- [in]
         rdClk       => axisClk,               -- [in]
         rdEn        => r.rorFifoRdEn,         -- [in]
         rdTimestamp => rorFifoTimestamp,      -- [out]
         rdValid     => open);                 -- [out]

   comb : process (axisRst, eventAxisCtrl, r, rawFifoAxisMaster, rorFifoTimestamp) is
      variable v : RegType;
   begin
      v := r;

      v.eventAxisMaster := axiStreamMasterInit(EMAC_AXIS_CONFIG_C);
      v.rorFifoRdEn     := '0';

      case r.state is
         when WAIT_ROR_S =>
            -- Got a ROR, write the header
            if (rorFifoTimestamp.valid = '1') then
               v.burn := eventAxisCtrl.pause;

               v.rorFifoRdEn                          := '1';
               v.eventAxisMaster.tValid               := not eventAxisCtrl.pause;
               v.eventAxisMaster.tData                := (others => '0');
               v.eventAxisMaster.tData(7 downto 0)    := r.burnCount;
               v.eventAxisMaster.tData(15 downto 8)   := SUBSYSTEM_ID_G;
               v.eventAxisMaster.tData(23 downto 16)  := CONTRIBUTOR_ID_G;
               v.eventAxisMaster.tData(127 downto 64) := toSlv(rorFifoTimestamp);

               if (eventAxisCtrl.pause = '1' and r.burnCount /= X"FF") then
                  v.burnCount := r.burnCount + 1;
               end if;
               if (eventAxisCtrl.pause = '0') then
                  v.burnCount := (others => '0');
               end if;
               v.state := DO_DATA_S;
            end if;

         when DO_DATA_S =>
            -- Write Data after header until tLast
            v.rawFifoAxisSlave.tReady := rawFifoAxisMaster.tValid;
            v.eventAxisMaster         := rawFifoAxisMaster;
            if (v.eventAxisMaster.tValid = '1' and r.burn = '1') then
               v.eventAxisMaster.tValid := '0';
            end if;
            if (rawFifoAxisMaster.tValid = '1' and rawFifoAxisMaster.tLast = '1') then
               v.state := WAIT_ROR_S;
            end if;

      end case;

      if (fifoRstSync = '1') then
         v.burnCount := (others => '0');
      end if;

      -- Reset
      if (axisRst = '1') then
         v := REG_INIT_C;
      end if;

      -- Register the variable for next clock cycle
      rin <= v;

   end process;

   seq : process (axisClk) is
   begin
      if (rising_edge(axisClk)) then
         r <= rin after TPD_G;
      end if;
   end process seq;


   U_AxiStreamFifoV2_EVENT_FIFO_1 : entity surf.AxiStreamFifoV2
      generic map (
         TPD_G               => TPD_G,
         PIPE_STAGES_G       => 0,
         SLAVE_READY_EN_G    => false,
--         VALID_THOLD_G       => 0,
--          VALID_BURST_MODE_G     => VALID_BURST_MODE_G,
         GEN_SYNC_FIFO_G     => true,
--         FIFO_FIXED_THRESH_G => true,
         FIFO_PAUSE_THRESH_G => EVENT_FIFO_PAUSE_THRESH_G,
         FIFO_ADDR_WIDTH_G   => EVENT_FIFO_ADDR_WIDTH_G,
         SYNTH_MODE_G        => EVENT_FIFO_SYNTH_MODE_G,
         MEMORY_TYPE_G       => EVENT_FIFO_MEMORY_TYPE_G,
         SLAVE_AXI_CONFIG_G  => EMAC_AXIS_CONFIG_C,
         MASTER_AXI_CONFIG_G => EMAC_AXIS_CONFIG_C)
      port map (
         sAxisClk    => axisClk,              -- [in]
         sAxisRst    => axisRst,              -- [in]
         sAxisMaster => r.eventAxisMaster,    -- [in]
         sAxisSlave  => open,                 -- [out]
         sAxisCtrl   => eventAxisCtrl,        -- [out]
         mAxisClk    => axisClk,              -- [in]
         mAxisRst    => axisRst,              -- [in]
         mAxisMaster => eventBatchedAxisMaster,  -- [out]
         mAxisSlave  => eventBatchedAxisSlave);  -- [in]

   U_AxiStreamBatcherAxil_1 : entity surf.AxiStreamBatcherAxil
      generic map (
         TPD_G                        => TPD_G,
         COMMON_CLOCK_G               => true,
         MAX_NUMBER_SUB_FRAMES_G      => 200,
         SUPER_FRAME_BYTE_THRESHOLD_G => 8192,
         MAX_CLK_GAP_G                => 256,
         AXIS_CONFIG_G                => EMAC_AXIS_CONFIG_C,
         INPUT_PIPE_STAGES_G          => 1,
         OUTPUT_PIPE_STAGES_G         => 1)
      port map (
         axisClk         => axisClk,                 -- [in]
         axisRst         => axisRst,                 -- [in]
         idle            => open,                    -- [out]
         sAxisMaster     => eventFifoAxisMaster,     -- [in]
         sAxisSlave      => eventFifoAxisSlave,      -- [out]
         mAxisMaster     => eventBatchedAxisMaster,  -- [out]
         mAxisSlave      => eventBatchedAxisSlave,   -- [in]
         axilClk         => axilClk,                 -- [in]
         axilRst         => axilRst,                 -- [in]
         axilReadMaster  => axilReadMaster,          -- [in]
         axilReadSlave   => axilReadSlave,           -- [out]
         axilWriteMaster => axilWriteMaster,         -- [in]
         axilWriteSlave  => axilWriteSlave);         -- [out]

   -------------------------------------------------------------------------------------------------
   -- Final FIFO
   -------------------------------------------------------------------------------------------------
   U_AxiStreamFifoV2_EVENT_FIFO_2 : entity surf.AxiStreamFifoV2
      generic map (
         TPD_G               => TPD_G,
         PIPE_STAGES_G       => 0,
         SLAVE_READY_EN_G    => false,
         VALID_THOLD_G       => 0,
--          VALID_BURST_MODE_G     => VALID_BURST_MODE_G,
         GEN_SYNC_FIFO_G     => true,
--         FIFO_FIXED_THRESH_G => true,
--         FIFO_PAUSE_THRESH_G => EVENT_FIFO_PAUSE_THRESH_G,
         FIFO_ADDR_WIDTH_G   => EVENT_FIFO_ADDR_WIDTH_G,
         SYNTH_MODE_G        => EVENT_FIFO_SYNTH_MODE_G,
         MEMORY_TYPE_G       => EVENT_FIFO_MEMORY_TYPE_G,
         SLAVE_AXI_CONFIG_G  => EMAC_AXIS_CONFIG_C,
         MASTER_AXI_CONFIG_G => EMAC_AXIS_CONFIG_C)
      port map (
         sAxisClk    => axisClk,                 -- [in]
         sAxisRst    => axisRst,                 -- [in]
         sAxisMaster => eventBatchedAxisMaster,  -- [in]
         sAxisSlave  => eventBatchedAxisSlave,   -- [out]
         sAxisCtrl   => open,                    -- [out]
         mAxisClk    => axisClk,                 -- [in]
         mAxisRst    => axisRst,                 -- [in]
         mAxisMaster => eventAxisMaster,         -- [out]
         mAxisSlave  => eventAxisSlave);         -- [in]   

end architecture rtl;
