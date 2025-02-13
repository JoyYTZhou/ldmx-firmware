/**
 *-----------------------------------------------------------------------------
 * Title      : RogueCoda API Header File
 * ----------------------------------------------------------------------------
 * File       : RogueCoda.h
 * Created    : 2019-03-29
 * ----------------------------------------------------------------------------
 * Description:
 * Public API header file for RogueCoda library
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

#ifndef __ROGUE_CODA_H__
#define __ROGUE_CODA_H__

#ifdef __cplusplus
extern "C" {
#endif

// Structure to hold RogueCoda Data
struct RogueCodaData;

// Initialize RogueCoda Data
struct RogueCodaData * rogueCodaInit();

// Delete RogueCoda Data
void rogueCodaDelete(struct RogueCodaData *rcd);

// Process download sequence
int rogueCodaDownload(struct RogueCodaData *rcd, const char *confFile, const char *usrString);

// Process prestart sequence
int rogueCodaPrestart(struct RogueCodaData *rcd);

// Process go sequence
int rogueCodaGo(struct RogueCodaData *rcd);

// Call with trigger information, return events count in trigger
int rogueCodaTrigger(struct RogueCodaData *rcd, unsigned int *trigData, unsigned int trigLength);

// Process trigger data, update passed pointer and return size
int rogueCodaEvent(struct RogueCodaData *rcd, unsigned int *eventBuffer, unsigned int idx);

// Called per event for book keeping, and calibration updates
// Return |= 0x01 if config should be added
// Return |= 0x10 if calibration run is ended.
int rogueCodaUpdate(struct RogueCodaData *rcd, unsigned int syncFlag);

// Copy configuration to event buffer and return size
int rogueCodaConfig(struct RogueCodaData *rcd, unsigned int *eventbuffer);

// Process end sequence
int rogueCodaEnd(struct RogueCodaData *rcd);

// Process close
void rogueCodaClose(struct RogueCodaData *rcd);

#ifdef __cplusplus
}
#endif

#endif
