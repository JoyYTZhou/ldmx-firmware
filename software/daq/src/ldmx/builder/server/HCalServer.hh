// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_HCALSERVER_HH__
#define __LDMX_BUILDER_SERVER_HCALSERVER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/HCALServer.hh
  \brief  Composes and sends HCAL batches for the builder server
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

  \class HCALServer
  \brief Composes and sends a HCAL message
                                                                          */
/* ---------------------------------------------------------------------- */
class HCALServer
{
public:
   HCALServer (ldmx::builder::server::Configuration const &cfg);

public:
   void  composeHeader ();
   bool  addEvent      (uint64_t timestamp);
   int   sendBatch     ();
   bool  declare       (uint64_t timestamp);
   void  reset         ();
   void  reconnect     ();

public:
   const char      *m_path;  /*!< The HCAL pipe name                   */
   int                m_fd;  /*!< FD to send HCAL messages             */
   int        m_nhclEvents;  /*!< # of HCAL events/message             */
   int           m_nevents;  /*!< # events in the current message         */
   uint32_t  m_blockNumber;  /*!< # of HCAL messages                   */
   uint32_t  m_eventNumber;  /*!< # of HCAL events                     */
   uint8_t          *m_cur;  /*!< Current pointer                         */
   uint8_t m_buffer[8*256];  /*!< Buffer to hold the HCALs             */
};
/* ---------------------------------------------------------------------- */


#endif
