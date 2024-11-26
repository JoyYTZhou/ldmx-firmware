-------------------------------------------------------------------------------
-- Title      : ROR Data Delay
-------------------------------------------------------------------------------
-- Company    : SLAC National Accelerator Laboratory
-- Platform   : 
-- Standard   : VHDL'93/02
-------------------------------------------------------------------------------
-- Description: Delays data by readout request latency
-- dataOut updates with rising edge of fcBus.bunchStrobe once aligned.
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

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;

entity RorDaqDataDelay is

   generic (
      TPD_G          : time    := 1 ns;
      DATA_WIDTH_G   : integer := 16;
      DELAY_OFFSET_G : integer := 0;
      MEMORY_TYPE_G  : string  := "distributed");
   port (
      fcClk185     : in  sl;
      fcRst185     : in  sl;
      fcBus        : in  FcBusType;
      timestampIn  : in  FcTimestampType;
      dataIn       : in  slv(DATA_WIDTH_G-1 downto 0);
      aligned      : out sl;
      delay        : out slv(7 downto 0);
      timestampOut : out FcTimestampType;
      dataOut      : out slv(DATA_WIDTH_G-1 downto 0));
end entity RorDaqDataDelay;

architecture rtl of RorDaqDataDelay is

   type StateType is (
      INIT_S,
      WAIT_BC0_ID_S,
      WAIT_BC0_DATA_S,
      WAIT_ROR_S,
      POSITIVE_OFFSET_S,
      NEGATIVE_OFFSET_S,
      ALIGNED_S);

   -- fcClk185 signals
   type RegType is record
      state        : StateType;
      count        : integer range 0 to abs(DELAY_OFFSET_G)+1;
      bc0Timestamp : FcTimestampType;
      fifoWrEn     : sl;
      fifoRdEn     : sl;
      fifoRst      : sl;
      aligned      : sl;
      delay        : slv(7 downto 0);
   end record;

   constant REG_INIT_C : RegType := (
      state        => INIT_S,
      count        => 0,
      bc0Timestamp => FC_TIMESTAMP_INIT_C,
      fifoWrEn     => '0',
      fifoRdEn     => '0',
      fifoRst      => '0',
      aligned      => '0',
      delay        => (others => '0'));

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   signal delayTmp : slv(7 downto 0);

begin

   U_FcTimestampFifo_1 : entity ldmx_tdaq.FcTimestampFifo
      generic map (
         TPD_G           => TPD_G,
         GEN_SYNC_FIFO_G => true,
         SYNTH_MODE_G    => "inferred",
         MEMORY_TYPE_G   => MEMORY_TYPE_G,
         ADDR_WIDTH_G    => 8,
         DATA_WIDTH_G    => DATA_WIDTH_G,
         AUTO_WRITE_G    => false)
      port map (
         rst         => r.fifoRst,      -- [in]
         wrClk       => fcClk185,       -- [in]
         wrEn        => rin.fifoWrEn,   -- [in]
         wrFull      => open,           -- [out]
         wrTimestamp => timestampIn,    -- [in]
         wrData      => dataIn,         -- [in]
         wrCount     => delayTmp,       -- [out]
         rdClk       => fcClk185,       -- [in]
         rdEn        => rin.fifoRdEn,   -- [in]
         rdCount     => open,           -- [out]
         rdTimestamp => timestampOut,   -- [out]
         rdData      => dataOut,        -- [out]
         rdValid     => open);          -- [out]


   comb : process (delayTmp, fcBus, fcRst185, r, timestampIn) is
      variable v         : RegType := REG_INIT_C;
      variable timestamp : FcTimestampType;
   begin
      v := r;

      v.fifoWrEn := '0';
      v.fifoRdEn := '0';
      v.fifoRst  := '0';
      v.count    := 0;

      timestamp.valid      := '1';
      timestamp.bunchCount := fcBus.bunchCount;
      timestamp.pulseId    := fcBus.pulseId;

      -- Sample delay on each bunch strobe
      if (fcBus.bunchStrobe = '1') then
         v.delay := delayTmp;
      end if;

      case r.state is
         when INIT_S =>
            v.aligned := '0';
            v.fifoRst := '1';
            v.state   := WAIT_BC0_ID_S;

         when WAIT_BC0_ID_S =>
            if (fcBus.pulseStrobe = '1' and
                fcBus.stateChanged = '1') then
               if (fcBus.runState = RUN_STATE_BC0_C) then
                  v.bc0Timestamp := timestamp;
                  v.state        := WAIT_BC0_DATA_S;
               else
                  v.state := INIT_S;
               end if;
            end if;

         when WAIT_BC0_DATA_S =>
            -- Wait until BC0 data arrives then start writing into FIFO 
            if (timestampIn = r.bc0Timestamp) then
               v.fifoWrEn := '1';
               v.state    := WAIT_ROR_S;
            end if;

            if (fcBus.runState /= RUN_STATE_BC0_C) then
               v.state := INIT_S;
            end if;


         when WAIT_ROR_S =>
            -- Write data into fifo as it arrives
            if (timestampIn.valid = '1') then
               v.fifoWrEn := '1';
            end if;

            -- Readout request during alignment is the BC0 RoR
            if (fcBus.readoutRequest.valid = '1') then
               if (DELAY_OFFSET_G = 0) then
                  v.state := ALIGNED_S;
               elsif (DELAY_OFFSET_G < 0) then
                  v.state := NEGATIVE_OFFSET_S;
               else
                  v.state := POSITIVE_OFFSET_S;
               end if;
            end if;

            -- Any state change should send it back to unaligned
            if (fcBus.stateChanged = '1') then
               v.state := INIT_S;
            end if;

         when NEGATIVE_OFFSET_S =>
            -- For negative offset
            -- Allow abs(offset) timestamps of data into the fifo without reading out
            v.count := r.count;
            if (timestampIn.valid = '1') then
               v.fifoWrEn := '1';
               v.count    := r.count + 1;
               if (r.count = ((-1) * DELAY_OFFSET_G)-1) then
                  v.state := ALIGNED_S;
               end if;
            end if;


         when POSITIVE_OFFSET_S =>
            -- if positive offset
            -- Allow abs(offset) fifo reads while not writing to the fifo
            v.count := r.count;
            if (fcBus.bunchStrobePre = '1') then
               v.fifoRdEn := '1';
               v.count    := r.count + 1;
               if (r.count = DELAY_OFFSET_G-1) then
                  v.state := ALIGNED_S;
               end if;
            end if;


         when ALIGNED_S =>
            v.aligned := '1';
            -- Continue writing data as it arrives
            if (timestampIn.valid = '1') then
               v.fifoWrEn := '1';
            end if;

            -- Read data on each bunch strobe
            if (fcBus.bunchStrobePre = '1') then
               v.fifoRdEn := '1';
            end if;

            if (fcBus.stateChanged = '1' and fcBus.runState = RUN_STATE_RESET_C) then
               v.state   := INIT_S;
               v.aligned := '0';
            end if;

         when others => null;
      end case;

      if (fcRst185 = '1') then
         v := REG_INIT_C;
      end if;

      aligned <= r.aligned;
      delay   <= r.delay;

      rin <= v;

   end process;

   seq : process (fcClk185) is
   begin
      if (rising_edge(fcClk185)) then
         r <= rin after TPD_G;
      end if;
   end process seq;

end architecture rtl;
