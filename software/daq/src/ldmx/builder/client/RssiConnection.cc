// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RssiConnection.cc
  \brief  RSSI connection for 1 client of LDMX event builder
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
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
 * 2019.02.12 jjr Moved to namespace ldmx::builder::client
 * 2018.12.19 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "RssiConnection.hh"
#include "RssiContributor.hh"
#include "ldmx/builder/client/ConnectionDsc.hh"


#include <rogue/protocols/udp/Core.h>
#include <rogue/protocols/udp/Client.h>
#include <rogue/protocols/rssi/Client.h>
#include <rogue/protocols/packetizer/CoreV2.h>

#include "rogue/protocols/rssi/Transport.h"
#include "rogue/protocols/rssi/Application.h"
#include <rogue/protocols/packetizer/Application.h>
#include <rogue/protocols/packetizer/Transport.h>

#include <string.h>

#include <iostream>

RssiConnection::RssiConnection (ldmx::builder::client::CfgContributor const &cfgCtb)
{
   construct (cfgCtb);
   return;
}


/* ---------------------------------------------------------------------- *//*!

  \brief The RssiConnnection constructor

  \param[in]      ip The IP address as the usual dotted string, 
                     \e e.g. 192.168.2.110
  \param[in] udpPort The UDP port number
  \param[in] nframes The number of jumbo frames to allocate for receiving.
                                                                          */
/* ---------------------------------------------------------------------- */
void RssiConnection::construct (ldmx::builder::client::CfgContributor const &cfg)
{
   // Commit the ip name and port number
   char const *ip = cfg.m_trnStr;
   int      ipLen = strlen (ip);
   m_ip = reinterpret_cast<decltype(m_ip)>(malloc (ipLen + 1));
   strcpy (m_ip, ip);

   m_port    = cfg.m_port;
   m_timeout = cfg.m_timeout;

   std::cout << "IP.port " << m_ip << ':'
             << m_port << " frames: " << cfg.m_nbuf << std::endl;


   // Create the UDP client, jumbo = true
   m_udp  = rogue::protocols::udp::Client::create(ip, cfg.m_port, true);



   // Make enough room for 'nframes' outstanding buffers
   m_udp->setRxBufferCount (cfg.m_nbuf); 


   // RSSI
   m_rssi = rogue::protocols::rssi::Client::create(m_udp->maxPayload());


   // Packetizer, srpEn = false ibCrc = false, obCrc = true
   m_pack = rogue::protocols::packetizer::CoreV2::create (false, true, true);


   // Connect the RSSI engine to the UDP client
   /// --- m_udp ->setSlave  (m_rssi->transport ());
   /// --- m_rssi->transport ()->setSlave   (m_udp);
   m_udp ->addSlave  (m_rssi->transport ());
   m_rssi->transport ()->addSlave   (m_udp);

   

   // Connect the RSSI engine to the packetizer
   /// --- m_rssi->application ()->setSlave (m_pack->transport());
   /// --- m_pack->transport   ()->setSlave (m_rssi->application());

   m_rssi->application ()->addSlave (m_pack->transport());
   m_pack->transport   ()->addSlave (m_rssi->application());


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
   /// --- m_pack->application(0)->setSlave (contributor);
   m_pack->application(0)->addSlave (contributor);
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
