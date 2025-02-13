// -*-Mode: C++;-*-

#ifndef _LDMX_BUILDER_SERVER_RSSI_CONNECTION_HH_
#define _LDMX_BUILDER_SERVER_RSSI_CONNECTION_HH_

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/RssiConnection.hh
  \brief  RSSI connection for 1 contributor to LDMX event builder
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
 * 2019.01.07 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <memory>
#include <cinttypes>


/* ====================================================================== */
/* Forward References                                                     */
/* ---------------------------------------------------------------------- */

namespace rogue     {
namespace protocols {
      namespace udp        { class Server; }
      namespace rssi       { class Server; }
      namespace packetizer { class CoreV2; }
}}

class RssiContributor;
/* ====================================================================== */



/* ---------------------------------------------------------------------- *//*!

  \brief Class to =captures the sequence needed to establish an RSSI 
         connection
                                                                          */
/* ---------------------------------------------------------------------- */
class RssiConnection
{
public:
   RssiConnection () : m_ip (0), m_port (0) { return; }

   RssiConnection (int                              serverId,
                   char const                            *ip, 
                   uint16_t                          udpPort, 
                   int                               nframes);
   ~RssiConnection ();

public:
   void construct (int                              serverId,
                   char const                            *ip, 
                   uint16_t                          udpPort, 
                   int                               nframes);

   void  start   ();
   void  connect (std::shared_ptr<RssiContributor> contributor);

   bool  isConnected () const;

public:
   std::shared_ptr<rogue::protocols::udp::Server>         m_udp;
   std::shared_ptr<rogue::protocols::rssi::Server>       m_rssi;
   std::shared_ptr<rogue::protocols::packetizer::CoreV2> m_pack;
   char                                                   *m_ip;
   uint16_t                                          m_serverId;                  
   uint16_t                                              m_port;
};
/* ---------------------------------------------------------------------- */


#endif
