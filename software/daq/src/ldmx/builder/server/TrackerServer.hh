// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_TRACKERSERVER_HH__
#define __LDMX_BUILDER_SERVER_TRACKERSERVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TrackerServer.hh
  \brief  Composes and sends tracker batches for the builder server
          emulator
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

  \class TrackerServer
  \brief Composes and sends a tracker message
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerServer
{
public:
   TrackerServer (ldmx::builder::server::Configuration const &cfg);

public:
   void  composeHeader ();
   bool  addEvent      (uint64_t timestamp);
   int   sendBatch     ();
   bool  declare       (uint64_t timestamp);
   void  reset         ();
   void  reconnect     ();

public:
   const char      *m_path;  /*!< The tracker pipe name                   */
   int                m_fd;  /*!< FD to send tracker messages             */
   int        m_ntrkEvents;  /*!< # of tracker events/message             */
   int           m_nevents;  /*!< # events in the current message         */
   uint32_t  m_blockNumber;  /*!< # of tracker messages                   */
   uint32_t  m_eventNumber;  /*!< # of tracker events                     */
   uint8_t          *m_cur;  /*!< Current pointer                         */
   uint8_t m_buffer[8*256];  /*!< Buffer to hold the trackers             */
};
/* ---------------------------------------------------------------------- */


#endif
