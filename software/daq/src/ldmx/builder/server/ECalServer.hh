// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_ECALSERVER_HH__
#define __LDMX_BUILDER_SERVER_ECALSERVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/ECALServer.hh
  \brief  Composes and sends ECAL batches for the builder server
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

  \class ECALServer
  \brief Composes and sends a ECAL message
                                                                          */
/* ---------------------------------------------------------------------- */
class ECALServer
{
public:
   ECALServer (ldmx::builder::server::Configuration const &cfg);

public:
   void  composeHeader ();
   bool  addEvent      (uint64_t timestamp);
   int   sendBatch     ();
   bool  declare       (uint64_t timestamp);
   void  reset         ();
   void  reconnect     ();

public:
   const char      *m_path;  /*!< The ECAL pipe name                   */
   int                m_fd;  /*!< FD to send ECAL messages             */
   int        m_ntrgEvents;  /*!< # of ECAL events/message             */
   int           m_nevents;  /*!< # events in the current message         */
   uint32_t  m_blockNumber;  /*!< # of ECAL messages                   */
   uint32_t  m_eventNumber;  /*!< # of ECAL events                     */
   uint8_t          *m_cur;  /*!< Current pointer                         */
   uint8_t m_buffer[8*256];  /*!< Buffer to hold the ECALs             */
};
/* ---------------------------------------------------------------------- */


#endif
