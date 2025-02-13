// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_RSSI_CONTRIBUTOR_HH__
#define __LDMX_BUILDER_CLIENT_RSSI_CONTRIBUTOR_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RssiContributor.hh
  \brief  RSSI contributor of LDMX event builder
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


#include "RssiConnection.hh"
#include "ldmx/utl/ListsInterlocked.hh"
#include "ldmx/utl/FixedPacket.hh"
#include <rogue/interfaces/stream/Slave.h>
///#include <rogue/interfaces/stream/Frame.h>
#include <memory>
#include <time.h>


/* ---------------------------------------------------------------------- */
/* Forward References                                                     */
/* ---------------------------------------------------------------------- */
namespace rogue  { namespace interfaces { namespace stream { class Frame; }}}

namespace ldmx   { namespace utl     { class FixedPacket;  }}
namespace ldmx   { namespace builder { namespace client { class CfgContributor; }}}


   class RssiConnection;
   class  Contributions; 
   class   Contribution;

/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Class defining one RSSI Contributor to the LDMX event builder
                                                                          */
/* ---------------------------------------------------------------------- */
class RssiContributor : public rogue::interfaces::stream::Slave 
{
public:
   RssiContributor () = delete;
   
   RssiContributor  (int                                        serverId,
                     ldmx::builder::client::CfgContributor const &cfgCtb,
                     Contributions                        *contributions,
                     int                                      packetSize);
   
   static std::shared_ptr<RssiContributor>  
          construct (int                                        serverId,
                     ldmx::builder::client::CfgContributor const &cfgCtb,
                     Contributions                        *contributions,
                     int                                      packetSize);

   void acceptFrame (std::shared_ptr<rogue::interfaces::stream::Frame> frame);
   bool isConnected () const;

   /* ------------------------------------------------------------------- *//*!

      \brief Class to monitor and accumulate statistics about the 
             reception of incoming frames
                                                                          */
   /* ------------------------------------------------------------------- */
   class Statistics
   {
   public:
      Statistics ();
      Statistics (Statistics volatile &stats);

   public:
      void construct  () volatile;

   public:
      static void    gettime (struct timespec          *ts);
      static void    gettime (struct timespec volatile *ts);
      static int64_t subtime (struct timespec const *start,
                              struct timespec const  *stop);

   public:
      static void print_title ();
      static void print       (RssiContributor::Statistics const *cur,
                               RssiContributor::Statistics const *prv,
                               bool                            opened,
                               char                               eol);

   public:
      struct timespec m_time;  /*!< The timestamp of these statistics     */
      uint32_t     m_rxCount;  /*!< Count of the incoming events          */
      uint64_t     m_rxBytes;  /*!< Total number of bytes received        */
      uint32_t      m_rxLast;  /*!< Number of bytes in last event seen    */
      uint32_t    m_packDrop;  /*!< Number of dropped packets             */
      uint32_t    m_rssiDrop;  /*!< Number of dropped rssi packets        */
   };
   /* ------------------------------------------------------------------- */


public:
   RssiConnection        m_connection; /*!< The connection information    */
   Contributions     *m_contributions; /*!< Current contributions         */
   ldmx::utl::ListsInterlocked::List
                              *m_list; /*!< The list of contributions     */
   ldmx::utl::FixedPacket       m_fpa; /*!< Fixed packet pool             */
   Statistics volatile        m_stats; /*!< The shared statistics         */
   int                           m_id; /*!< Countributor identifier       */
};
/* ---------------------------------------------------------------------- */
#endif
