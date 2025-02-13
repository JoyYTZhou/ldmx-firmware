// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/Configuration.cc
  \brief  LDMX builder server emulation configuration
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
 * 2019.01.11 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/server/Configuration.hh"


/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace server  {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the LDMX Event Builder

\* ---------------------------------------------------------------------- */
Configuration::Configuration (int type) :
   m_type   (  type)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void Configuration::setServerIp (const char *serverIp)
{
   m_ip = serverIp;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Adds the connection descriptor for one RSSI contributor

  \param[in] name The IP name of the contributor, not necessarily 
                  NUL-termimnated
  \param[in]  len The length of the IP name of the contributor
  \param[in] port The port number

\* ---------------------------------------------------------------------- */
void Configuration::addPort (uint16_t port)
{
   m_ports.push_back (port);
}
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: server                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */
