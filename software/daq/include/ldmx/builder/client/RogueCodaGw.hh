// -*-Mode: C++;-*-

/**
 *-----------------------------------------------------------------------------
 * Title      : Rogue Coda Gateway
 * ----------------------------------------------------------------------------
 * File       : RogueCodaGw.h
 * Created    : 2010-03-29
 * ----------------------------------------------------------------------------
 * Description:
 * Rogue Coda Gateway
 * ----------------------------------------------------------------------------
 * This file is part of the LDMX software platform. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 *    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of the LDMX software platform, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#ifndef __ROGUE_CODA_GW_H__
#define __ROGUE_CODA_GW_H__

#include <cinttypes>


#ifndef LDMX_EMULATION
#include <rogue/RogueSMemFunctions.h>
#endif


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
class          Gateway;
namespace ldmx    {
namespace builder { 
namespace client  {
   class Configuration; 
}}}
/* ====================================================================== */



#define ROOTNAME "CodaDummyRoot"


//! Rogue CODA Gateway
class RogueCodaGw 
{
public:
   RogueCodaGw();
  ~RogueCodaGw();


public:
   // Process download sequence
   int32_t download (ldmx::builder::client::Configuration const &cfg);
   int32_t download (const char *confFile,     const char *usrString);

   
   // Process prestart sequence
   int32_t prestart();


   // Process go sequence
   int32_t go();


   // Call with trigger information, return events count in trigger
   int32_t trigger(uint32_t *trigData, uint32_t trigLength);
   

   // Process trigger data, update passed pointer and return size
   int32_t event(uint32_t *eventBuffer, uint32_t index);
   

   // Called per event for book keeping, and calibration updates
   // Return |= 0x01 if config should be added
   // Return |= 0x10 if calibration run is ended.
   int32_t update(uint32_t syncFlag);


   // Copy configuration to event buffer and return size
   int32_t config(uint32_t *eventBuffer);
   
   // Process end sequence
   int32_t end();


   // Process close
   void close();


private:
   #ifndef LDMX_EMULATION
   RogueControlMem *smem_;
   int32_t smemFd_;
   #endif

   // Helper function to update coda state
   int32_t codaState ( const char *state, const char *arg );
      
   // Convert calibration state to injection value
   static uint32_t getCalLevel(uint32_t levelIn);

   uint32_t lastEventNumber_;
   uint32_t *trigData_;
   uint32_t trigLength_;
   uint32_t trigBlkSize_;
   uint32_t runType_;
   uint32_t calGroup_;
   uint32_t calDelay_;
   uint32_t calLevel_;
   uint32_t writeConfig_;


private:
   Gateway *m_gw;
};


#endif
