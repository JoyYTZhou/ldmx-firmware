// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/RssiConnection.cc
  \brief  RSSI connection for 1 contributor to the LDMX builder
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
 * 2018.12.19 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "RssiConnection.hh"
#include "RssiContributor.hh"


#include <rogue/protocols/udp/Core.h>
#include <rogue/protocols/udp/Server.h>
#include <rogue/protocols/rssi/Server.h>
#include <rogue/protocols/packetizer/CoreV2.h>

#include "rogue/protocols/rssi/Transport.h"
#include "rogue/protocols/rssi/Application.h"
#include <rogue/protocols/packetizer/Application.h>
#include <rogue/protocols/packetizer/Transport.h>
#include <rogue/Helpers.h>

#include <string.h>

class RssiContributor;

/* ---------------------------------------------------------------------- *//*!

  \brief The RssiConnnection constructor

  \param[in] serverId Server identifier
  \param[in]       ip The IP address as the usual dotted string, 
                      \e e.g. 192.168.2.110
  \param[in]  udpPort The UDP port number
  \param[in]  nframes The number of jumbo frames to allocate for receiving.
                                                                          */
/* ---------------------------------------------------------------------- */
RssiConnection::RssiConnection (int                         serverId,
                                char const                       *ip,
                                uint16_t                     udpPort,
                                int                          nframes)
{
   construct (serverId, ip, udpPort, nframes);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The RssiConnnection constructor

  \param[in] serverId Server identifier
  \param[i n]      ip The IP address as the usual dotted string, 
                      \e e.g. 192.168.2.110
  \param[in]  udpPort The UDP port number
  \param[in]  nframes The number of jumbo frames to allocate for receiving.
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiConnection::construct (int   serverId,
                                char const *ip, 
                                uint16_t  port, 
                                int   nbuffers)
{
   printf ("DaqServer:Port = %s:%d\n", ip, port);
   int ipLen  = strlen (ip);
   m_ip       = reinterpret_cast<decltype(m_ip)>(malloc (ipLen + 1));
   m_serverId = serverId;
   m_port     = port;
   strcpy (m_ip, ip);


   // Create the UDP server, jumbo = true
   m_udp  = rogue::protocols::udp::Server::create (port, true);


   // Make enough room for 16 outstanding buffers
   m_udp->setRxBufferCount(nbuffers); 


   // RSSI
   m_rssi = rogue::protocols::rssi::Server::create(m_udp->maxPayload());


   // Packetizer, ibCrc = false, obCrc = true
   m_pack = rogue::protocols::packetizer::CoreV2::create (false, true, true);


   // Connect the RSSI engine to the UDP server
   /// --- streamConnectBiDir(m_udp, m_rssi->transport());
   rogueStreamConnectBiDir(m_udp, m_rssi->transport());


   // Connect the RSSI engine to the packetizer
   /// --- streamConnectBiDir (m_rssi->application(), m_pack->transport());
   rogueStreamConnectBiDir (m_rssi->application(), m_pack->transport());


   // Create a test source and connect to channel 1 of the packetizer
   //std::shared_ptr<RssiContributor> 
   //     send = std::make_shared<RssiContributor>();
   //streamConnect (send, m_pack->application(0));

   return;
}
/* ---------------------------------------------------------------------- */






/* ---------------------------------------------------------------------- *//*!

   \brief Destructor for an RssiConnection

\* ---------------------------------------------------------------------- */
RssiConnection::~RssiConnection ()
{
   ///printf ("Destructor for %s:%d\n", m_ip, m_port);
   if (m_ip) free (m_ip);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Enables the establishment of the RSSI connection

\* ---------------------------------------------------------------------- */

void RssiConnection::start ()
{
   m_rssi->start();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Connect a contributor to the RssiConnection

  \param[in] receiver The RSSI receiver.  This must be a subclass of
                      rogue::interfaces::stream::Slave 
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiConnection::connect (std::shared_ptr<RssiContributor> contributor)
{
   /// --- streamConnect (contributor, m_pack->application(0));
   rogueStreamConnect (contributor, m_pack->application(0));
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Checks if the connection has been established

\* ---------------------------------------------------------------------- */
bool RssiConnection::isConnected () const
{
   bool   connected= m_rssi->getOpen ();
   return connected;
}
/* ---------------------------------------------------------------------- */
