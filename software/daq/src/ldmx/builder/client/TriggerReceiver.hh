// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_TRIGGERRECEIVER_HH__
#define __LDMX_BUILDER_CLIENT_TRIGGERRECEIVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TriggerReceiver.hh
  \brief  Reads trigger messages from the builder server emulator
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

   class TriggerBatch;
}
}
}

/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \class TriggerReceiver
  \brief Composes and sends a trigger message
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerReceiver
{
public:
   TriggerReceiver ();
   TriggerReceiver (ldmx::builder::client::Configuration const &cfg,
                    Contributions                             *ctbs,
                    int                                      ctbBeg);

public:
   void                                           start ();
   uint32_t                             getContributors () const;
   ldmx::data::client::TriggerBatch const *triggerBatch () const;
   ldmx::data::client::TriggerBatch       *triggerBatch ();

public:
   static void receiver (TriggerReceiver *receiver);


public:
   ContributionSubset 
            m_contributions;  /*!< The subset of trigger contributions    */
   ldmx::utl::FixedPacket
                      m_fpa;  /*!< Pool for trigger batch messages        */
   std::thread     m_thread;  /*!< The receiving thread                   */
   int             m_tiNBuf;  /*1< The number of bytes in the buffer      */
   uint8_t         *m_tiBuf;  /*!< The receiving buffer                   */
   int                 m_fd;  /*!< FD to receive trigger messages         */
   int           m_nbatches;  /*!< # of trigger batches received          */
   int            m_nevents;  /*!< # of trigger events received           */
};
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor
                                                                          */
/* ----------------------------v------------------------------------------ */
inline TriggerReceiver::TriggerReceiver () 
 {
    return; 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline uint32_t TriggerReceiver::getContributors () const
{
   return m_contributions.getSet ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Returns a pointer to the read trigger batch
                                                                          */
/* ---------------------------------------------------------------------- */
inline const ldmx::data::client::TriggerBatch
                                *TriggerReceiver::triggerBatch () const
{
   return reinterpret_cast<const ldmx::data::client::TriggerBatch *>(m_tiBuf);
}




/* ---------------------------------------------------------------------- *//*!

  \brief Returns a pointer to the read trigger batch
                                                                          */
/* ---------------------------------------------------------------------- */
inline ldmx::data::client::TriggerBatch
                          *TriggerReceiver::triggerBatch ()
{
   return reinterpret_cast<ldmx::data::client::TriggerBatch *>(m_tiBuf);
}
/* ====================================================================== */

#endif
