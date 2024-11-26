-------------------------------------------------------------------------------
-- Company    : SLAC National Accelerator Laboratory
-------------------------------------------------------------------------------
-- Description: Fast Control Package
--
-------------------------------------------------------------------------------
-- This file is part of 'LDMX'.
-- It is subject to the license terms in the LICENSE.txt file found in the
-- top-level directory of this distribution and at:
--    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
-- No part of 'LDMX', including this file,
-- may be copied, modified, propagated, or distributed except according to
-- the terms contained in the LICENSE.txt file.
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

library surf;
use surf.StdRtlPkg.all;

package FcPkg is

   -------------------------------------------------------------------------------------------------
   -- Fast Control Messages sent on PGPFC are 80 bits, with fields defined here
   -- FcMessageType encodes the fields into a record
   -------------------------------------------------------------------------------------------------
   constant FC_LEN_C              : natural := 64;
   subtype MSG_TYPE_RANGE_C is natural range 63 downto 62;
   subtype BUNCH_CNT_RANGE_C is natural range 61 downto 56;
   subtype RUN_STATE_RANGE_C is natural range 59 downto 56;
   constant STATE_CHANGED_INDEX_C : natural := 60;
   subtype PULSE_ID_RANGE_C is natural range 55 downto 0;

   constant RUN_STATE_RESET_C    : slv(3 downto 0) := "0000";
   constant RUN_STATE_IDLE_C     : slv(3 downto 0) := "0001";
   constant RUN_STATE_BC0_C      : slv(3 downto 0) := "0010";
   constant RUN_STATE_PRESTART_C : slv(3 downto 0) := "0011";
   constant RUN_STATE_RUNNING_C  : slv(3 downto 0) := "0100";
   constant RUN_STATE_STOPPED_C  : slv(3 downto 0) := "0101";

   constant MSG_TYPE_TIMING_C : slv(1 downto 0) := toSlv(0, 2);
   constant MSG_TYPE_ROR_C    : slv(1 downto 0) := toSlv(1, 2);

   type FcMessageType is record
      valid        : sl;
      msgType      : slv(1 downto 0);
      bunchCount   : slv(5 downto 0);
      runState     : slv(3 downto 0);
      stateChanged : sl;
      pulseID      : slv(55 downto 0);
      message      : slv(FC_LEN_C-1 downto 0);
   end record;

   constant FC_MSG_INIT_C : FcMessageType := (
      valid        => '0',
      msgType      => (others => '0'),
      -- reserved => (others => '0'),
      bunchCount   => (others => '0'),
      runState     => (others => '0'),
      stateChanged => '0',
      pulseID      => (others => '0'),
      message      => (others => '0')
      );

   function toSlv (msg          : FcMessageType) return slv;
   function toFcMessage (vector : slv(FC_LEN_C-1 downto 0); valid : sl := '1') return FcMessageType;

   -------------------------------------------------------------------------------------------------
   -- Readout Request Fields
   -------------------------------------------------------------------------------------------------
   type FcTimestampType is record
      valid      : sl;
      bunchCount : slv(5 downto 0);
      pulseId    : slv(55 downto 0);
   end record FcTimestampType;

   constant FC_TIMESTAMP_INIT_C : FcTimestampType := (
      valid      => '0',
      bunchCount => (others => '0'),
      pulseId    => (others => '0'));

   constant FC_TIMESTAMP_SIZE_C : integer := 64;

   type FcTimestampArray is array (natural range <>) of FcTimestampType;

   function toSlv (
      fcTimestamp : FcTimestampType)
      return slv;

   function toFcTimestamp (
      vector : slv(FC_TIMESTAMP_SIZE_C-1 downto 0);
      valid  : sl := '1')
      return FcTimestampType;

   function "+"(lhs : FcTimestampType; rhs : integer) return FcTimestampType;
   function "-"(lhs : FcTimestampType; rhs : integer) return FcTimestampType;


   -------------------------------------------------------------------------------------------------
   -- The Fast control receiver block outputs a bus of fast control data on this record
   -------------------------------------------------------------------------------------------------
   type FcBusType is record
      -- FC Rx status
      rxLinkStatus : sl;

      -- Placed on bus with each TM received
      pulseStrobe  : sl;
      pulseId      : slv(55 downto 0);
      runState     : slv(3 downto 0);
      stateChanged : sl;

      -- These are counted based on Timing messages
      bunchStrobePre : sl;              -- Pulsed 1 cycle before bunchCount increments
      bunchStrobe    : sl;              -- Pulsed on cycle that bunchCount increments
      bunchCount     : slv(5 downto 0);
      subCount       : slv(2 downto 0);
      bc0            : sl;

      -- 185 MHz counter from T0
      runTime : slv(63 downto 0);

      -- Readout request data placed on this bus as received
      readoutRequest : FcTimestampType;

      -- All FC messages placed on this bus as they are received
      -- Usefull for propagating raw messages (as on tracker)
      fcMsg : FcMessageType;
   end record FcBusType;

   constant FC_BUS_INIT_C : FcBusType := (
      rxLinkStatus   => '0',
      pulseStrobe    => '0',
      pulseId        => (others => '0'),
      runState       => (others => '0'),
      stateChanged   => '0',
      bunchStrobePre => '0',
      bunchStrobe    => '0',
      bunchCount     => (others => '0'),
      subCount       => (others => '0'),
      bc0            => '0',
      runTime        => (others => '0'),
      readoutRequest => FC_TIMESTAMP_INIT_C,
      fcMsg          => FC_MSG_INIT_C);


   -------------------------------------------------------------------------------------------------
   -- Fast control feedback
   -- Only busy for now but more could be added
   -------------------------------------------------------------------------------------------------
   type FcFeedbackType is record
      busy : sl;
   end record FcFeedbackType;

   constant FC_FB_INIT_C : FcFeedbackType := (
      busy => '0');


end FcPkg;

package body FcPkg is

   function toSlv (msg : FcMessageType) return slv is
      variable retVar : slv(FC_LEN_C-1 downto 0);
   begin
      retVar                   := (others => '0');
      retVar(MSG_TYPE_RANGE_C) := msg.msgType;

      if (msg.msgType = MSG_TYPE_ROR_C) then
         -- if RoR, transmit the bunch counter
         retVar(BUNCH_CNT_RANGE_C) := msg.bunchCount;
      else
         -- if non-RoR, transmit the state
         retVar(RUN_STATE_RANGE_C)     := msg.runState;
         retVar(STATE_CHANGED_INDEX_C) := msg.stateChanged;
      end if;

      retVar(PULSE_ID_RANGE_C) := msg.pulseID;

      return retVar;
   end function;

   function toFcMessage (vector : slv(FC_LEN_C-1 downto 0); valid : in sl := '1') return FcMessageType is
      variable retVar : FcMessageType;
   begin
      retVar         := FC_MSG_INIT_C;
      retVar.valid   := valid;
      retVar.msgType := vector(MSG_TYPE_RANGE_C);
                                        -- no latches are inferred because retVar is initialized
                                        -- right below the 'begin'

                                        -- check the message type
      if (retVar.msgType = MSG_TYPE_ROR_C) then
                                        -- if RoR, grab the bunch count
         retVar.bunchCount := vector(BUNCH_CNT_RANGE_C);
      else
                                        -- if non-RoR, grab the state
         retVar.runState     := vector(RUN_STATE_RANGE_C);
         retVar.stateChanged := vector(STATE_CHANGED_INDEX_C);
      end if;

      retVar.pulseID := vector(PULSE_ID_RANGE_C);
      retVar.message := vector;

      return retVar;
   end function;

   function toSlv (
      fcTimestamp : FcTimestampType)
      return slv is
      variable ret : slv(FC_TIMESTAMP_SIZE_C-1 downto 0);
   begin
      ret              := (others => '0');
      ret(63 downto 8) := fcTimestamp.pulseId;
      ret(5 downto 0)  := fcTimestamp.bunchCount;
      return ret;
   end function toSlv;

   function toFcTimestamp (
      vector : slv(FC_TIMESTAMP_SIZE_C-1 downto 0);
      valid  : sl := '1')
      return FcTimestampType is
      variable ret : FcTimestampType;
   begin
      ret.valid      := valid;
      ret.pulseId    := vector(63 downto 8);
      ret.bunchCount := vector(5 downto 0);
      return ret;
   end function toFcTimestamp;

   -- Function to add an integer to FcTimestampType, supporting both positive and negative rhs
   function "+"(lhs : FcTimestampType; rhs : integer) return FcTimestampType is
      variable result           : FcTimestampType := lhs;
      variable temp_bunch_count : integer;
      variable pulse_adjustment : integer;
   begin
      -- Convert bunchCount to an integer and add rhs
      temp_bunch_count :=conv_integer(result.bunchCount) + rhs;

      -- Calculate adjustments for pulseId based on positive or negative temp_bunch_count
      if temp_bunch_count >= 0 then
         -- Positive case: increment pulseId for every rollover (40 counts per rollover)
         pulse_adjustment := temp_bunch_count / 40;
         temp_bunch_count := temp_bunch_count mod 40;
      else
         -- Negative case: decrement pulseId for every "borrow" needed
         pulse_adjustment := (temp_bunch_count - 39) / 40;  -- Adjust for downward division in VHDL
         temp_bunch_count := (temp_bunch_count mod 40 + 40) mod 40;  -- Wrap into [0, 39] range
      end if;

      -- Update bunchCount and pulseId with the calculated adjustments
      result.bunchCount := toSlv(temp_bunch_count, result.bunchCount'length);
      result.pulseId    := result.pulseId + pulse_adjustment;

      return result;
   end function "+";

   -- Function to subtract an integer from FcTimestampType by calling "+" with -rhs
   function "-"(lhs : FcTimestampType; rhs : integer) return FcTimestampType is
   begin
      -- Call the "+" function with -rhs
      return lhs + (-rhs);
   end function "-";

end package body FcPkg;
