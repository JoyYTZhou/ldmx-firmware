// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TrackerGenerator.cc
  \brief  Generates a simulated trigger batch of events
  \author Sophie Middleton

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



#include "ldmx/data/server/TrackerGenerator.hh"

#include "ldmx/data/server/TrackerBatch.hh"
#include "ldmx/data/server/TrackerHeader.hh"
#include "ldmx/data/server/TrackerEvent.hh"



#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tracker generator

  \param[in] rceAddress The sourcing RCE
                                                                          */
/* ---------------------------------------------------------------------- */
TrackerGenerator::TrackerGenerator () :
   m_batchSeqNumber (0),
   m_eventSeqNumber (0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets the batcher's and any other internal context

  \param[in:out] batch Pointer to the destinaion Tracker batch

  \note
   It is the responsibility of the user to clear the previous batch
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerGenerator::reset (ldmx::data::server::TrackerBatch *batch)
{
   batch->reset ();
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add the batch header

  \param[in:out] batch Pointer to the destinaion Tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerGenerator::addHeader (ldmx::data::server::TrackerBatch *batch,
                                  uint8_t                       blockSize,
                                  bool                   timestampPresent)
{
   using namespace ldmx::data::server;


   TrackerHeader *hdr __attribute__ ((unused))
                       = new (batch) TrackerHeader (blockSize,
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

  \param[in:out]     batch Pointer to the destinaion Tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
int TrackerGenerator::addEvent (ldmx::data::server::TrackerBatch   *batch,
                                uint8_t                              type,
                                uint64_t                        timestamp)
{
   using namespace ldmx::data::server;


   // -----------------------------------
   // Create a new event for this batcher
   // -----------------------------------
   if (m_timestampPresent)
   {
      TrackerEventLong *evt __attribute__ ((unused))
                            = new (batch) TrackerEventLong (type,
                                                            m_eventSeqNumber,
                                                            timestamp);
   }
   else
   {
      TrackerEventShort *evt __attribute__ ((unused))
         = new (batch) TrackerEventShort (type,
                                          m_eventSeqNumber);
   }


   auto nevents = batch->getNevents ();

   return nevents;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Quick, no-frills Tracker batch generator

  \param[in:out]    batch Pointer to the destinaion Tracker batch
  \param[in]      nevents Number events in the Tracker batch

                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerGenerator::generate (ldmx::data::server::TrackerBatch *batch,
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
