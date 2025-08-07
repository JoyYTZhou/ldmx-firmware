// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_TRACKERBATCH_HH__
#define __LDMX_DATA_CLIENT_TRACKERBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/TrackerBatch.hh
  \brief  Defines catalogues the Tracker events in a Tracker batch
  \author Sophie Middelton


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

   class TrackerHeader;
   class TrackerEvent;
   class TrackerEventLong;
   class TrackerTrailer;

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

  \brief  Class to catalogue the tracker events in a tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerBatch
{
public:
   TrackerBatch     ();

public:
   TrackerHeader          *getHeader    ();
   TrackerHeader    const *getHeader    () const;
   TrackerEvent     const *getEvent     (int            idx) const;
   TrackerEventLong const *getEventLong (int            idx) const;
   TrackerTrailer   const *getTrailer   (std::size_t nbytes) const;
   int                     getNevents   (std::size_t nbytes) const;

public:
   static int    size (int nevents);
   static int maxSize (int nevents);


public:
   void        print () const;
   static void print (TrackerBatch const *batch);
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/client/TrackerHeader.hh"
#include "ldmx/data/client/TrackerEvent.hh"
#include "ldmx/data/client/TrackerTrailer.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor the collection of tracker events in one tracker batch

  \param[in] buffer The buffer containing the tracker batch
  \param[in] nbytes The number of bytes in the tracker batch

                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerBatch::TrackerBatch ()
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of events in the tracker batch based on its size
  \return The number of events in the tracker batch based on its size

  \param[in] nbytes  The size of the tracker batch

  \par
   For checking purposes, this can be compared with the number of events
   as reported by the tracker header.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int TrackerBatch::getNevents (std::size_t nbytes) const
{
   TrackerHeader const *hdr = getHeader ();
   bool              isLong = hdr->getTimestampPresent ();

   int  eventSize = isLong
                  ? (sizeof (ldmx::data::common::TrackerEventShort)
                  +  sizeof (ldmx::data::common::TrackerEventExtention))
                  :  sizeof (ldmx::data::common::TrackerEventShort);

   int    nevents = (nbytes
                  -  sizeof (ldmx::data::common::TrackerHeader)
                  -  sizeof (ldmx::data::common::TrackerTrailer))
                  / eventSize;

   return nevents;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline int TrackerBatch::maxSize (int nevents)
{
   int    size =            sizeof (ldmx::data::common::TrackerHeader)
               + nevents * (sizeof (ldmx::data::common::TrackerEventShort)
                         +  sizeof (ldmx::data::common::TrackerEventExtention))
               +            sizeof (ldmx::data::common::TrackerTrailer);
   return size;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the tracker  header
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerHeader const *TrackerBatch::getHeader () const
{
   TrackerHeader const *hdr = reinterpret_cast<decltype (hdr)>(this);
   return               hdr;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the tracker  header
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerHeader *TrackerBatch::getHeader ()
{
   TrackerHeader *hdr = reinterpret_cast<decltype (hdr)>(this);
   return         hdr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the specified tracker event

  \param[in]  ievent The event number to fetch
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerEventLong const *TrackerBatch::getEventLong (int ievent) const
 {
    TrackerHeader     const *hdr = getHeader ();
    TrackerEventLong  const *evt = reinterpret_cast<decltype (evt)>(hdr + 1);

    return evt + ievent;
 }
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the tracker trailer based on the size of
         this tracker batch

  \param[in] nbytes  The size of this tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerTrailer const *TrackerBatch::getTrailer (std::size_t nbytes) const
{
   uint8_t const         *p8 = reinterpret_cast<decltype(p8)>(this);
   TrackerTrailer const *tlr = reinterpret_cast<decltype(tlr)>(p8
                                                       + nbytes
                       - sizeof (ldmx::data::common::TrackerTrailer));
   return tlr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the tracker batch of events to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TrackerBatch::print () const
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
