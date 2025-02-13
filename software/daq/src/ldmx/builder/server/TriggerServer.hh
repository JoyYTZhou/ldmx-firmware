// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_TRIGGERSERVER_HH__
#define __LDMX_BUILDER_SERVER_TRIGGERSERVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TriggerServer.hh
  \brief  Composes and sends trigger batches for the builder server 
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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.03.19 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cstdint>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace builder {
namespace server  {

   class Configuration;
}
}
}
/* ====================================================================== */




/* ---------------------------------------------------------------------- *//*!

  \class TriggerServer
  \brief Composes and sends a trigger message
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerServer
{
public:
   TriggerServer (ldmx::builder::server::Configuration const &cfg);

public:
   void  composeHeader ();
   bool  addEvent      (uint64_t timestamp);
   int   sendBatch     ();
   bool  declare       (uint64_t timestamp);
   void  reset         ();
   void  reconnect     ();

public:
   const char      *m_path;  /*!< The trigger pipe name                   */
   int                m_fd;  /*!< FD to send trigger messages             */
   int        m_ntrgEvents;  /*!< # of trigger events/message             */
   int           m_nevents;  /*!< # events in the current message         */
   uint32_t  m_blockNumber;  /*!< # of trigger messages                   */
   uint32_t  m_eventNumber;  /*!< # of trigger events                     */
   uint8_t          *m_cur;  /*!< Current pointer                         */
   uint8_t m_buffer[8*256];  /*!< Buffer to hold the triggers             */
};
/* ---------------------------------------------------------------------- */


#endif
