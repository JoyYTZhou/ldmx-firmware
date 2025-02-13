// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CONNECTION_DSC_HH__
#define __LDMX_BUILDER_CLIENT_CONNECTION_DSC_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/ConnectionDsc.hh
  \brief  Contains all the information needed to establish either any
          TCP/IP or RSSI connection
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
 * 2019.02.12 jjr Moved into hps::builder::client namespace
 * 2019.01.08 jjr Moved into hps::eb::client namespace
 * 2018.12.20 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/CfgContributor.hh"
#include <cinttypes>
#include <string>


/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to hold the IP address in string and binary formats plus
          the port number

\* ---------------------------------------------------------------------- */
class ConnectionDsc
{
public:
   ConnectionDsc (char const *name, int len, uint16_t port);

public:
   std::string   m_name;
   uint32_t        m_ip;
   uint16_t      m_port;
};
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */


#endif
