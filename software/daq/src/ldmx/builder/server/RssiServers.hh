// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_RSSISERVERS_HH__
#define __LDMX_BUILDER_SERVER_RSSISERVERS_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/RssiServers.hh
  \brief  Class for the collection RSSI LDMX event bulder server emulators
          builder server emulator
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
 * 2019.11.21 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <memory>
#include <cinttypes>


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

class RssiContributor;
/* ====================================================================== */



/* ---------------------------------------------------------------------- *//*!

  \class RssiServers
  \brief The collection of RSSI servers

\* ---------------------------------------------------------------------- */
class RssiServers
{
public:
   RssiServers (ldmx::builder::server::Configuration const &cfg);
  ~RssiServers ();

   const static int NServers = 8;        /*!< The maximum number of servers */

public:
   void      start              ();
   uint32_t  waitForConnections ();
   uint32_t  missingServers     ()
   {
      return m_mservers & ~m_connected;
   }

   uint32_t generateEvent      (unsigned short int nmultsamples);
   void     sendBatch          (uint32_t                 ctbSet);

public:
   const char                    *m_ip;  /*!< IP of client fragment builder */
   int32_t                  m_nservers;  /*!< Number  of  SVT data servers  */
   uint32_t                 m_mservers;  /*!< Bit mask of SVT data servers  */
   uint32_t                m_connected;  /*!< Bit mask of connected servers */
   uint32_t           m_batchSeqNumber;  /*!< The SVT batch sequence number */
   uint32_t           m_eventSeqNumber;  /*!< The SVT event sequence number */
   std::shared_ptr<RssiContributor> 
                   m_servers[NServers];  /*!< The SVT servers               */
};
/* ---------------------------------------------------------------------- */

#endif
