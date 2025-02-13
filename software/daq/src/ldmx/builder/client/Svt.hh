// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CONTRIBUTION_SVT_HH__
#define __LDMX_BUILDER_CLIENT_CONTRIBUTION_SVT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Svt.hh
  \brief  The SVT data contribution to an LDMX Event 
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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.02.12 jjr Moved to namespace hps::builder::client
 * 2019.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Contribution.hh"
#include <memory>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace rogue      {
namespace protocols  {
namespace batcher    {

   class CoreV1;
}
}
}
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief The SVT data contribution

  \par
   As data contribution is received from the server, all the context 
   needed to describe the this incoming data is committed to this class
                                                                          */
/* ---------------------------------------------------------------------- */
class Svt : public Contribution
{
public:
   Svt (uint32_t   nbytes,
        uint32_t sequence,
        uint64_t  rcvTime,
        unsigned   evtIdx,
        std::shared_ptr<rogue::protocols::batcher::CoreV1> core) :
      Contribution (nbytes, sequence, rcvTime),
      m_core                            (core),
      m_evtIdx                        (evtIdx)
   {
      return; 
   }

public:
   std::shared_ptr<rogue::protocols::batcher::CoreV1> m_core;
   unsigned                                         m_evtIdx;
};
/* ====================================================================== */

#endif

