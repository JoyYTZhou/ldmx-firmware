// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_TRACKERRECEIVER_HH__
#define __LDMX_BUILDER_CLIENT_TRACKERRECEIVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TrackerReceiver.hh
  \brief  Reads tracker messages from the builder server emulator
  \author Sophie Middleton

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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.03.21 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ContributionSubset.hh"
#include "ldmx/utl/FixedPacket.hh"

#include <thread>
#include <cstdint>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace builder {
namespace client  {

   class Configuration;
}
}
}


namespace ldmx    {
namespace data    {
namespace client  {

   class TrackerBatch;
}
}
}

/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \class TrackerReceiver
  \brief Composes and sends a tracker message
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerReceiver
{
public:
   TrackerReceiver ();
   TrackerReceiver (ldmx::builder::client::Configuration const &cfg,
                    Contributions                             *ctbs,
                    int                                      ctbBeg);

public:
   void                                           start ();
   uint32_t                             getContributors () const;
   ldmx::data::client::TrackerBatch const *trackerBatch () const;
   ldmx::data::client::TrackerBatch       *trackerBatch ();

public:
   static void receiver (TrackerReceiver *receiver);


public:
   ContributionSubset
            m_contributions;  /*!< The subset of tracker contributions    */
   ldmx::utl::FixedPacket
                      m_fpa;  /*!< Pool for tracker batch messages        */
   std::thread     m_thread;  /*!< The receiving thread                   */
   int             m_tiNBuf;  /*1< The number of bytes in the buffer      */
   uint8_t         *m_tiBuf;  /*!< The receiving buffer                   */
   int                 m_fd;  /*!< FD to receive tracker messages         */
   int           m_nbatches;  /*!< # of tracker batches received          */
   int            m_nevents;  /*!< # of tracker events received           */
};
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor
                                                                          */
/* ----------------------------v------------------------------------------ */
inline TrackerReceiver::TrackerReceiver ()
 {
    return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline uint32_t TrackerReceiver::getContributors () const
{
   return m_contributions.getSet ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Returns a pointer to the read tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
inline const ldmx::data::client::TrackerBatch
                                *TrackerReceiver::trackerBatch () const
{
   return reinterpret_cast<const ldmx::data::client::TrackerBatch *>(m_tiBuf);
}




/* ---------------------------------------------------------------------- *//*!

  \brief Returns a pointer to the read tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
inline ldmx::data::client::TrackerBatch
                          *TrackerReceiver::trackerBatch ()
{
   return reinterpret_cast<ldmx::data::client::TrackerBatch *>(m_tiBuf);
}
/* ====================================================================== */

#endif
