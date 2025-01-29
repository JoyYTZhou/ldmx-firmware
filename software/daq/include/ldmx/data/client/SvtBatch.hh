// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_SVTBATCH_HH__
#define __LDMX_DATA_CLIENT_SVTBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/SvtBatch.hh
  \brief  Defines catalogues the SVT events in a SVT batch
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
 * 2019.02.28 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/client/SvtEvent.hh"
#include <vector>
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {

   class SvtBatchHeader;

} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to catalogue the SVT events in a SVT batch
                                                                          */
/* ---------------------------------------------------------------------- */
class SvtBatch : public std::vector<SvtEvent>
{
public:
   SvtBatch     (int eventCount);
   int populate (uint8_t const *ptr, uint32_t nbytes);

public:
   SvtBatchHeader const *getHeader () const;

public:
   void        print () const;
   static void print (SvtBatch const *batch);
   
public:
   SvtBatchHeader const *m_header;  /*!< The SVT batch header             */
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/client/SvtBatchTail.hh"
#include "ldmx/data/common/SvtBatchTail.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor the collection of SVT events in one SVT batch

  \param[in] eventCount The initialize number of SVT events to allocate

                                                                          */
/* ---------------------------------------------------------------------- */
   inline SvtBatch::SvtBatch (int eventCount) :
      std::vector<SvtEvent>()
{
   reserve (eventCount);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Populate the SVT events

  \param[in]    ptr Pointer to the batch memory
  \param[in] nbytes The number of bytes in the batch memory
                                                                          */
/* ---------------------------------------------------------------------- */
inline int SvtBatch::populate (uint8_t const *ptr, uint32_t nbytes)
{
   // Locate the batch header
   m_header = reinterpret_cast<decltype(m_header)>(ptr);
   /// printf ("SvtBatch %4" PRId32 " @ %p\n", nbytes, (void *)ptr);


   ptr    += nbytes;
   nbytes -= sizeof (ldmx::data::common::SvtBatchTail);
   ptr    -= sizeof (ldmx::data::common::SvtBatchTail);


   while (nbytes)
   {
      // -----------------------------------------------------------------
      // Locate the batch tail record and retrieve the length of the event
      // -----------------------------------------------------------------
      SvtBatchTail const *tail = 
                     reinterpret_cast<decltype (tail)>(ptr);
      /// printf ("SvtBatch::populate tail @ %p %8.8" PRIx32 "\n",
      ///         (void *)tail, *((uint32_t const *)tail));


      // ----------------------------------------------------------
      // Get the number of bytes in the header + data sections
      // Add on the size of the trailer to get the total event size
      // ----------------------------------------------------------
      uint32_t evtBytes = tail->getNBytes ();
      /// printf ("SvtBatch::populate evtBytes = %4" PRIx32 " @ %p\n",
      ///          evtBytes, ptr);


      // -----------------------------------------------
      // Construct the description of the event in place
      // -----------------------------------------------
      emplace (end(), ptr, evtBytes);


      // ------------------------------------------
      // Reduce the number of bytes left to process
      // ------------------------------------------
      nbytes -= evtBytes + sizeof (ldmx::data::common::SvtBatchTail);
      ptr    -= evtBytes + sizeof (ldmx::data::common::SvtBatchTail);
      /// printf ("SvtBatch:populate %d @ %p\n", (int)nbytes, (void *)ptr);
   }

   int    nevents = size ();
   return nevents;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the SvtBatch header
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtBatchHeader const *SvtBatch::getHeader () const
 {
    return m_header;
 }
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the SVT batch of events to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtBatch::print () const
{
   print (this);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
