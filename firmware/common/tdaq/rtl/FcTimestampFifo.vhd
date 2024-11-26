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

library ldmx_tdaq;
use ldmx_tdaq.FcPkg.all;

entity FcTimestampFifo is

   generic (
      TPD_G           : time                  := 1 ns;
      GEN_SYNC_FIFO_G : boolean               := false;
      SYNTH_MODE_G    : string                := "inferred";
      MEMORY_TYPE_G   : string                := "block";
      ADDR_WIDTH_G    : integer range 4 to 48 := 4;
      DATA_WIDTH_G    : natural               := 0;
      AUTO_WRITE_G    : boolean               := true  -- Write when timestampIn.valid=1 instead of wrEn
      );

   port (
      rst : in sl;

      wrClk       : in  sl;
      wrEn        : in  sl                           := '0';
      wrFull      : out sl;
      wrTimestamp : in  FcTimestampType;
      wrData      : in  slv(DATA_WIDTH_G-1 downto 0) := (others => '0');
      wrCount     : out slv(ADDR_WIDTH_G-1 downto 0);

      rdClk       : in  sl;
      rdEn        : in  sl;
      rdCount     : out slv(ADDR_WIDTH_G-1 downto 0);
      rdTimestamp : out FcTimestampType;
      rdData      : out slv(DATA_WIDTH_G-1 downto 0);
      rdValid     : out sl);

end entity FcTimestampFifo;

architecture rtl of FcTimestampFifo is

   constant FIFO_WIDTH_C : integer := FC_TIMESTAMP_SIZE_C + DATA_WIDTH_G;

   signal fifoDin   : slv(FIFO_WIDTH_C-1 downto 0);
   signal fifoDout  : slv(FIFO_WIDTH_C-1 downto 0);
   signal fifoValid : sl;
   signal fifoWrEn  : sl;

begin

   fifoWrEn <= wrTimestamp.valid when AUTO_WRITE_G else wrEn;

   -- Convert to SLV for FIFO
   fifoDin <= toSlv(wrTimestamp) & wrData;

   U_Fifo_1 : entity surf.Fifo
      generic map (
         TPD_G           => TPD_G,
         FWFT_EN_G       => true,
         GEN_SYNC_FIFO_G => GEN_SYNC_FIFO_G,
         SYNTH_MODE_G    => SYNTH_MODE_G,
         MEMORY_TYPE_G   => MEMORY_TYPE_G,
         PIPE_STAGES_G   => 0,
         DATA_WIDTH_G    => FIFO_WIDTH_C,
         ADDR_WIDTH_G    => ADDR_WIDTH_G)
      port map (
         rst           => rst,          -- [in]
         wr_clk        => wrClk,        -- [in]
         wr_en         => fifoWrEn,     -- [in]
         din           => fifoDin,      -- [in]
         wr_data_count => wrCount,      -- [out]
         full          => wrFull,       -- [out]
         rd_clk        => rdClk,        -- [in]
         rd_en         => rdEn,         -- [in]
         dout          => fifoDout,     -- [out]
         rd_data_count => rdCount,      -- [out]
         valid         => fifoValid);   -- [out]

   rdValid     <= fifoValid;
   rdTimestamp <= toFcTimestamp(fifoDout(FIFO_WIDTH_C-1 downto DATA_WIDTH_G), fifoValid);
   rdData      <= fifoDout(DATA_WIDTH_G-1 downto 0);

end architecture rtl;
