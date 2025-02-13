// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_RSSI_CONNECTION_HH__
#define __LDMX_BUILDER_CLIENT_RSSI_CONNECTION_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RssiConnection.hh
  \brief  RSSI connection for 1 client of LDMX event builder
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


#include <memory>
#include <cinttypes>


/* ====================================================================== */
/* Forward References                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx { namespace builder { namespace client { class CfgContributor; }}}

namespace rogue {
   namespace protocols {
      namespace udp        { class Client; }
      namespace rssi       { class Client; }
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
   RssiConnection () : m_ip (0), m_timeout (0), m_port (0) { return; }
   RssiConnection (ldmx::builder::client::CfgContributor const &cfgCtb);

   void construct (ldmx::builder::client::CfgContributor const &cfgCtb);

   ~RssiConnection ();

public:
   void  start    ();

   void  connect  (std::shared_ptr<RssiContributor>   contributor);

   bool  isConnected () const;

public:
   std::shared_ptr<rogue::protocols::udp::Client>           m_udp;
   std::shared_ptr<rogue::protocols::rssi::Client>         m_rssi;
   std::shared_ptr<rogue::protocols::packetizer::CoreV2>   m_pack;
   char                                                     *m_ip;
   uint32_t                                             m_timeout;
   uint16_t                                                m_port;
};
/* ---------------------------------------------------------------------- */


#endif


