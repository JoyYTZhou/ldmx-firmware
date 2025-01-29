// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_TRIGGERBATCH_HH__
#define __LDMX_DATA_CLIENT_TRIGGERBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/TriggerBatch.hh
  \brief  Defines catalogues the Tirgger events in a Trigger batch
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
 * 2019.03.25 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {

   class TriggerHeader;
   class TriggerEvent;
   class TriggerEventLong;
   class TriggerTrailer;

} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldms                                                       */
/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to catalogue the trigger events in a trigger batch
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerBatch
{
public:
   TriggerBatch     ();

public:
   TriggerHeader          *getHeader    ();
   TriggerHeader    const *getHeader    () const;
   TriggerEvent     const *getEvent     (int            idx) const;
   TriggerEventLong const *getEventLong (int            idx) const;
   TriggerTrailer   const *getTrailer   (std::size_t nbytes) const;
   int                     getNevents   (std::size_t nbytes) const;

public:
   static int    size (int nevents);
   static int maxSize (int nevents);


public:
   void        print () const;
   static void print (TriggerBatch const *batch);
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/client/TriggerHeader.hh"
#include "ldmx/data/client/TriggerEvent.hh"
#include "ldmx/data/client/TriggerTrailer.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor the collection of trigger events in one trigger batch

  \param[in] buffer The buffer containing the trigger batch
  \param[in] nbytes The number of bytes in the trigger batch

                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerBatch::TriggerBatch ()
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of events in the trigger batch based on its size
  \return The number of events in the trigger batch based on its size

  \param[in] nbytes  The size of the trigger batch

  \par
   For checking purposes, this can be compared with the number of events
   as reported by the trigger header.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int TriggerBatch::getNevents (std::size_t nbytes) const
{
   TriggerHeader const *hdr = getHeader ();
   bool              isLong = hdr->getTimestampPresent ();

   int  eventSize = isLong 
                  ? (sizeof (ldmx::data::common::TriggerEventShort)
                  +  sizeof (ldmx::data::common::TriggerEventExtention))
                  :  sizeof (ldmx::data::common::TriggerEventShort);

   int    nevents = (nbytes 
                  -  sizeof (ldmx::data::common::TriggerHeader) 
                  -  sizeof (ldmx::data::common::TriggerTrailer))
                  / eventSize;

   return nevents;
}
/* ---------------------------------------------------------------------- */

              

/* ---------------------------------------------------------------------- */
inline int TriggerBatch::maxSize (int nevents)
{
   int    size =            sizeof (ldmx::data::common::TriggerHeader)
               + nevents * (sizeof (ldmx::data::common::TriggerEventShort)
                         +  sizeof (ldmx::data::common::TriggerEventExtention))
               +            sizeof (ldmx::data::common::TriggerTrailer);
   return size;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the trigger  header
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerHeader const *TriggerBatch::getHeader () const
{
   TriggerHeader const *hdr = reinterpret_cast<decltype (hdr)>(this);
   return               hdr;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the trigger  header
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerHeader *TriggerBatch::getHeader ()
{
   TriggerHeader *hdr = reinterpret_cast<decltype (hdr)>(this);
   return         hdr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the specified trigger event

  \param[in]  ievent The event number to fetch
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerEventLong const *TriggerBatch::getEventLong (int ievent) const
 {
    TriggerHeader     const *hdr = getHeader ();
    TriggerEventLong  const *evt = reinterpret_cast<decltype (evt)>(hdr + 1);

    return evt + ievent;
 }
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the trigger trailer based on the size of
         this trigger batch

  \param[in] nbytes  The size of this trigger batch
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerTrailer const *TriggerBatch::getTrailer (std::size_t nbytes) const
{
   uint8_t const         *p8 = reinterpret_cast<decltype(p8)>(this);
   TriggerTrailer const *tlr = reinterpret_cast<decltype(tlr)>(p8 
                                                       + nbytes 
                       - sizeof (ldmx::data::common::TriggerTrailer));
   return tlr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the trigger batch of events to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TriggerBatch::print () const
{
   ////print (this);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
