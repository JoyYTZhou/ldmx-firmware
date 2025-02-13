// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_RSSIRECEIVER_HH__
#define __LDMX_BUILDER_CLIENT_RSSIRECEIVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RssiReceiver.hh
  \brief  Receives contributions from RSSI contributors
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

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
 * 2018.12.19 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ContributionSubset.hh"
#include "ldmx/utl/FixedPacket.hh"

#include <cstdint>
#include <memory>


class RssiContributor;

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx { namespace builder { namespace client { class Configuration; }}}


class Contributions;

namespace ldmx { namespace utl                        { class   FixedPacket;  }}
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief      Receives contributions from RSSI contributors
                                                                          */
/* ---------------------------------------------------------------------- */
class RssiReceiver
{
public:
   RssiReceiver ();
   RssiReceiver (ldmx::builder::client::Configuration const &cfg,
                 Contributions                             *ctbs,
                 int                                      ctbBeg);
  ~RssiReceiver ();

   const static int NContributors = 8;

public:
   void     start ();
   uint32_t waitForConnections  ();
   uint32_t missingContributors () const
   {
      return m_contributions.getSet () & ~m_mconnected;
   }

   uint32_t getContributors () const
   {
      return m_contributions.getSet ();
   }

   uint32_t getConnected () const
   {
      return m_mconnected;
   }


public:
   ContributionSubset                             m_contributions;
   uint32_t                                          m_mconnected;
   int32_t                                    m_connectionTimeout;
   std::shared_ptr<RssiContributor> m_contributors[NContributors];
};
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline RssiReceiver::RssiReceiver ()
{
   return;
}
/* ====================================================================== */

#endif
