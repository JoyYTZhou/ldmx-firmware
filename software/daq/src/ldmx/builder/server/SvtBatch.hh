// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_TRIGGERSERVER_HH__
#define __LDMX_BUILDER_SERVER_TRIGGERSERVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TriggerServer.hh
  \brief  Composes and sends SVT event batches for the builder server
          emulator
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.21 jjr Adapted from the HPS version
 * 2019.03.19 jjr Created
 *
\* ---------------------------------------------------------------------- */


class SvtGenerator  : public  ldmx::data::server::SvtBatch
{
public:
   SvtGenerator (uint8_t rceAddress);

   static const size_t NBytes = 32 * 1024;

   void addHeader  (ldmx::data::server::SvtBatch &batch,);
   int  addEvent   (ldmx::data::server::SvtBatch &batch,
                    int                   nmultisamples);
   
public:
   uint32_t           m_batchSeqNumber;
   uint32_t           m_eventSeqNumber;
   uint32_t                m_curEvents;
   uint8_t                m_rceAddress;

};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Svt generator
                                                                          */
/* ---------------------------------------------------------------------- */
SvtGenerator::SvtGenerator (uint8_t rceAddress) :
   m_batchSeqNumber (0),
   m_eventSeqNumber (0),
   m_curEvents      (0),
   m_rceAddress     (rceAddress) 
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Add the batch header
                                                                          */
/* ---------------------------------------------------------------------- */
void SvtGenerator::addHeader  (ldmx::data::server::SvtBatch &batch)
{
   using namespace ldmx::data::server;
   
   // Clear the old batch out
   batch.reset ();

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

  \param[in] nmultisamples  The number of multi-sample data records
                                                                          */
/* ---------------------------------------------------------------------- */
int SvtGenerator::addEvent (ldmx::data::server::SvtBatch   &batch,
                            int                     nmultisamples)
{
   int febAddress    = 0x55;
   int hybridAddress = 0x44;


   // -----------------------------------
   // Create a new event for this batcher
   // -----------------------------------
   SvtEvent    evt (*batch);


   // ------------------------
   // Add the SVT event header
   // ------------------------
   SvtHeader *eHdr __attribute__ ((unused))
      = new (&evt) SvtHeader (m_eventSeqNumber, rceAddress);
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
         samples[isample] = (ievt << 8) | (ims << 4) | (isample);
      } 


      // ----------------------------------------
      // Create the SVT multi-sample summary word
      // ----------------------------------------
      uint8_t apvChannel = ims;
      uint8_t apvNumber  = ievt;
      bool     readError = false;
      bool          tail = false;
      bool          head = false;
      bool    filterFlag =  true;
      uint32_t   summary = SvtMultiSample::composeSummary (rceAddress,
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

   /// printf ("Allocated trailer %2d @ %p\n", ievt, (void *)eTlr);


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


   return m_curEvents += 1;
}
/* ---------------------------------------------------------------------- */
   
#endif   


