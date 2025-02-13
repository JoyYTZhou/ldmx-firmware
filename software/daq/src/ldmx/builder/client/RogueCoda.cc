/**
 *-----------------------------------------------------------------------------
 * Title      : RogueCoda Public API
 * ----------------------------------------------------------------------------
 * File       : RogueCoda.cpp
 * Created    : 2019-03-29
 * ----------------------------------------------------------------------------
 * Description:
 * Public API for RogueCoda library
 * ----------------------------------------------------------------------------
 * This file is part of the HPS software platform. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 *    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of the HPS software platform, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/


#include "ldmx/builder/client/RogueCoda.h"
#include "ldmx/builder/client/RogueCodaGw.hh"

#include <new>
#include <stdlib.h>


// Structure to hold RogueCoda Data
struct RogueCodaData {
   RogueCodaGw m_gw;
};


// Initialize RogueCoda Data
struct RogueCodaData * rogueCodaInit() {
   struct RogueCodaData * ret;

   ret = reinterpret_cast<RogueCodaData *>(malloc(sizeof(struct RogueCodaData)));
   new (&ret->m_gw) RogueCodaGw ();
   return ret;
}


// Delete RogueCoda Data
void rogueCodaDelete(struct RogueCodaData *rcd) {
   free(rcd);
}


// Process download sequence
int32_t rogueCodaDownload(struct RogueCodaData *rcd, 
                          const char      *confFile, 
                          const char     *usrString) {
   return rcd->m_gw.download(confFile,usrString);
}


// Process prestart sequence
int32_t rogueCodaPrestart(struct RogueCodaData *rcd) {
   return rcd->m_gw.prestart();
}


// Process go sequence
int32_t rogueCodaGo(struct RogueCodaData *rcd) {
   return rcd->m_gw.go();
}


// Call with trigger information, return events count in trigger
int32_t rogueCodaTrigger(struct RogueCodaData *rcd,
                         uint32_t        *trigData, 
                         uint32_t       trigLength) {
   return rcd->m_gw.trigger(trigData,trigLength);
}


// Process trigger data, update passed pointer and return size
int32_t rogueCodaEvent(struct RogueCodaData *rcd,
                       uint32_t     *eventBuffer,
                       unsigned int        index) {
   return rcd->m_gw.event(eventBuffer,index);
}


// Called per event for book keeping, and calibration updates
// Return |= 0x01 if config should be added
// Return |= 0x10 if calibration run is ended.
int32_t rogueCodaUpdate(struct RogueCodaData *rcd, 
                        uint32_t         syncFlag) {
   return rcd->m_gw.update(syncFlag);
}


// Copy configuration to event buffer and return size
int32_t rogueCodaConfig(struct RogueCodaData *rcd, 
                        uint32_t     *eventBuffer) {
   return rcd->m_gw.config(eventBuffer);
}


// Process end sequence
int32_t rogueCodaEnd(struct RogueCodaData *rcd) {
   return rcd->m_gw.end();
}


// Process close
void rogueCodaClose(struct RogueCodaData *rcd) {
   rcd->m_gw.close();
}
