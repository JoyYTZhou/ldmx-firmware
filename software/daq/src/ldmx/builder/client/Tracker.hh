// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CONTRIBUTION_TRACKER_HH__
#define __LDMX_BUILDEr_CLIENT_CONTRIBUTION_TRACKER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Tracker.hh
  \brief  The tracker contribution to an LDMX Event
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




/* ---------------------------------------------------------------------- *\
 *
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.02.12 jjr Moved to namespace hps::builder::client
 * 2019.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/client/TrackerBatch.hh"
#include "Contribution.hh"

#include <memory.h>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
// <none>
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief The tracker contribution

  \par
   The tracker contribution serves as the seed for the event
                                                                          */
/* ---------------------------------------------------------------------- */
class Tracker : public Contribution
{
public:
   Tracker (uint32_t     nbytes, 
            uint32_t m_sequence,
            uint64_t    rcvTime,
            bool           last) :
      Contribution (nbytes, m_sequence, rcvTime)
   {
      return;
   }

   Tracker (uint32_t                                  nbytes, 
            uint32_t                              m_sequence,
            uint64_t                                 rcvTime,
            ldmx::data::client::TrackerEventLong const  *evt,
            bool                                        last) :

      Contribution (nbytes, m_sequence, rcvTime),
      m_event (*evt)
   {
      return;
   }


   Tracker ()  { return; }

public:
   ldmx::data::client::TrackerEventLong m_event;
};
/* ====================================================================== */


#endif
