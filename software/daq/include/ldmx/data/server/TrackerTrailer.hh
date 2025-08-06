// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_TRACKERTRAILER_HH__
#define __LDMX_DATA_SERVER_TRACKERTRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TrackerTrailer.hh
  \brief  Defines filling the tracker trailer structure.
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


#include "ldmx/data/common/TrackerTrailer.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Fill the tracker trailer
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerTrailer : public ldmx::data::common::TrackerTrailer
{
public:
   TrackerTrailer ();
   TrackerTrailer (uint32_t w32);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tracker trailer

  \note
   There is one tracker trailer for each batch of tracker events.
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerTrailer::TrackerTrailer ()
{
   m_w32 = ldmx::data::common::TrackerTrailer::W32;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tracker trailer

  \note
   There is one tracker trailer for each batch of tracker events.

  \param[in] w32  The tracker trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerTrailer::TrackerTrailer (uint32_t w32)
{
   m_w32 = w32;
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
