// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TriggerGenerator.cc
  \brief  Generates a simulated trigger batch of events
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the
   terms contained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.03.05 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/data/server/TriggerGenerator.hh"

#include "ldmx/data/server/TriggerBatch.hh"
#include "ldmx/data/server/TriggerHeader.hh"
#include "ldmx/data/server/TriggerEvent.hh"



#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Trigger generator

  \param[in] rceAddress The sourcing RCE
                                                                          */
/* ---------------------------------------------------------------------- */
TriggerGenerator::TriggerGenerator () :
   m_batchSeqNumber (0),
   m_eventSeqNumber (0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets the batcher's and any other internal context

  \param[in:out] batch Pointer to the destinaion Trigger batch 

  \note 
   It is the responsibility of the user to clear the previous batch
                                                                          */
/* ---------------------------------------------------------------------- */
void TriggerGenerator::reset (ldmx::data::server::TriggerBatch *batch)
{
   batch->reset ();
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add the batch header

  \param[in:out] batch Pointer to the destinaion Trigger batch 
                                                                          */
/* ---------------------------------------------------------------------- */
void TriggerGenerator::addHeader (ldmx::data::server::TriggerBatch *batch,
                                  uint8_t                       blockSize,
                                  bool                   timestampPresent)
{
   using namespace ldmx::data::server;
   

   TriggerHeader *hdr __attribute__ ((unused))
                       = new (batch) TriggerHeader (blockSize,
                                                    m_batchSeqNumber,
                                                    timestampPresent);

   /// printf ("Allocate batch %2d @ %p\n", m_batchSequence, (void *)bHdr);


   m_batchSeqNumber  += 1;
   m_timestampPresent = timestampPresent;

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add an event with \a nmultisamples data records
  \return The number of events in the current batch

  \param[in:out]     batch Pointer to the destinaion Trigger batch 
                                                                          */
/* ---------------------------------------------------------------------- */
int TriggerGenerator::addEvent (ldmx::data::server::TriggerBatch   *batch,
                                uint8_t                              type,
                                uint64_t                        timestamp)
{
   using namespace ldmx::data::server;


   // -----------------------------------
   // Create a new event for this batcher
   // -----------------------------------
   if (m_timestampPresent)
   {
      TriggerEventLong *evt __attribute__ ((unused))
                            = new (batch) TriggerEventLong (type, 
                                                            m_eventSeqNumber,
                                                            timestamp);
   }
   else
   {
      TriggerEventShort *evt __attribute__ ((unused))
         = new (batch) TriggerEventShort (type,
                                          m_eventSeqNumber);
   }


   auto nevents = batch->getNevents ();

   return nevents;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Quick, no-frills Trigger batch generator

  \param[in:out]    batch Pointer to the destinaion Trigger batch 
  \param[in]      nevents Number events in the Trigger batch

                                                                          */
/* ---------------------------------------------------------------------- */
void TriggerGenerator::generate (ldmx::data::server::TriggerBatch *batch,
                                 int                             nevents)
{
   using namespace ldmx::data::server;


   addHeader (batch, nevents, false);

   for (int ievt = 0; ievt < nevents; ++ievt)
   {
      addEvent (batch, 0, 0);
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
