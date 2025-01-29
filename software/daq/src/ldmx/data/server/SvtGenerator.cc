// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/SvtGenerator.cc
  \brief  Generates a simulated SVT batch of events
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



#include "ldmx/data/server/SvtGenerator.hh"
#include "ldmx/data/server/SvtBatch.hh"
#include "ldmx/data/server/SvtBatchHeader.hh"
#include "ldmx/data/server/SvtEvent.hh"
#include "ldmx/data/server/SvtHeader.hh"
#include "ldmx/data/server/SvtMultiSample.hh"
#include "ldmx/data/server/SvtTrailer.hh"
#include "ldmx/data/server/SvtBatchTail.hh"


#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Svt generator

  \param[in] rceAddress The sourcing RCE
                                                                          */
/* ---------------------------------------------------------------------- */
SvtGenerator::SvtGenerator (uint8_t rceAddress) :
   m_batchSeqNumber (0),
   m_eventSeqNumber (0),
   m_rceAddress     (rceAddress) 
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets the batcher's and any other internal context

  \param[in:out] batch Pointer to the destinaion SVT batch 

  \note 
   It is the responsibility of the user to clear the previous batch
                                                                          */
/* ---------------------------------------------------------------------- */
void SvtGenerator::reset (ldmx::data::server::SvtBatch *batch)
{
   batch->reset ();
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add the batch header

  \param[in:out] batch Pointer to the destinaion SVT batch 
                                                                          */
/* ---------------------------------------------------------------------- */
void SvtGenerator::addHeader (ldmx::data::server::SvtBatch *batch)
{
   using namespace ldmx::data::server;
   

   SvtBatchHeader *bHdr __attribute__ ((unused))
                 = new (batch) SvtBatchHeader (m_batchSeqNumber);

   /// printf ("Allocate batch %2d @ %p\n", m_batchSequence, (void *)bHdr);


   m_batchSeqNumber += 1;

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add an event with \a nmultisamples data records
  \return The number of events in the current batch

  \param[in:out]     batch Pointer to the destinaion SVT batch 
  \param[in] nmultisamples The number of multi-sample data records
                                                                          */
/* ---------------------------------------------------------------------- */
int SvtGenerator::addEvent (ldmx::data::server::SvtBatch   *batch,
                            int                     nmultisamples)
{
   using namespace ldmx::data::server;

   int    febAddress    = 0x55;
   int    hybridAddress = 0x44;
   uint32_t eventNumber = batch->getNevents ();


   // -----------------------------------
   // Create a new event for this batcher
   // -----------------------------------
   SvtEvent    evt (batch);


   // ------------------------
   // Add the SVT event header
   // ------------------------
   SvtHeader *eHdr __attribute__ ((unused))
      = new (&evt) SvtHeader (m_eventSeqNumber, m_rceAddress);
   /// printf ("Allocated event %2d @ %p\n", m_eventSeqNumber, (void *)eHdr);
   m_eventSeqNumber += 1;


   // --------------------------------
   // Create the SVT multi-sample data
   // --------------------------------
   uint16_t samples[6];
   for (int ims = 0; ims < nmultisamples; ++ims)
   {
      // -------------------------------------
      // Makeup some fake data for the samples
      // -------------------------------------
      for (int isample = 0; isample < 6; ++isample)
      {
         samples[isample] = (eventNumber << 8) | (ims << 4) | (isample);
      } 


      // ----------------------------------------
      // Create the SVT multi-sample summary word
      // ----------------------------------------
      uint8_t apvChannel = ims;
      uint8_t apvNumber  = eventNumber;
      bool     readError = false;
      bool          tail = false;
      bool          head = false;
      bool    filterFlag =  true;
      uint32_t   summary = SvtMultiSample::composeSummary (m_rceAddress,
                                                           febAddress,
                                                           apvChannel,
                                                           apvNumber,
                                                           hybridAddress,
                                                           readError,
                                                           tail,
                                                           head,
                                                           filterFlag);

      // -------------------------------------
      // Add this SVT multi-sample data record
      // -------------------------------------
      SvtMultiSample *msData __attribute__ ((unused)) 
         = new (&evt) SvtMultiSample (samples, summary);

      /// printf (
      ///  "Allocate multisample data %2d @ %p  summary: %8.8" PRIx32 "\n", 
      /// ims, (void *)msData, summary);
   }


   // -----------------------------------
   // Add the SVT event trailer
   // This will add any necessary padding
   // -----------------------------------
   SvtTrailer *eTlr __attribute__ ((unused))
      = new (&evt) SvtTrailer (nmultisamples, 0);

   /// printf ("Allocated trailer %2d @ %p\n", eventNumber, (void *)eTlr);


   // ------------------------------------------------------
   // Add the batch trailer
   // ---------------------
   // The byte size is specified to be number of valid bytes
   // in the event. This count excludes any padding needed
   // to reach the AXI stream bit size boundary
   // ------------------------------------------------------
   uint32_t nvalid = evt.getNValid ();
   /// printf ("NValid = %8" PRIx32 "\n", nvalid);

   SvtBatchTail *bTail __attribute__ ((unused))
                 = new (batch) SvtBatchTail (nvalid, false);
   /// printf ("BatchTail = %p\n", bTail);      

   auto nevents = batch->getNevents ();

   return nevents;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Quick, no-frills SVT batch generator

  \param[in:out]    batch Pointer to the destinaion SVT batch 
  \param[in]      nevents Number events in the SVT batch
  \param[in] nmultisample Number of multi-sample data records in each
                          event
                                                                          */
/* ---------------------------------------------------------------------- */
void SvtGenerator::generate (ldmx::data::server::SvtBatch *batch,
                             int                         nevents,
                             int                   nmultisamples)
{
   using namespace ldmx::data::server;


   addHeader (batch);

   for (int ievt = 0; ievt < nevents; ++ievt)
   {
      addEvent (batch, nmultisamples);
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */
