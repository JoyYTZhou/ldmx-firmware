// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/ConnectionDsc.cc
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
 * 2019.02.12 jjr Moved into hps/builder/client namespace
 * 2019.01.08 jjr Moved into hps/eb/client namespace
 * 2018.12.20 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/ConnectionDsc.hh"
#include <netdb.h>


/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- *//*!

  \brief Converts the IP name (a string) to it binary IP address and 
         stores the name,ip address pair

  \param[in]  name The IP name (not necessarily NUL-terminated
  \param[in]   len The length, in characters of \a name

\* ---------------------------------------------------------------------- */
ConnectionDsc::ConnectionDsc (char const *name, int len, uint16_t port) :
   m_name (name, len),
   m_ip   (0),
   m_port (port)
{
   uint32_t ip = 0;
   struct hostent *he = gethostbyname (m_name.c_str ());
   if (he == NULL)
   {
      herror ("IPpair: gethostname failed");
      m_ip = 0;
      return;
   }
   struct in_addr **addr_list =  (struct in_addr **) he->h_addr_list;
   
   ip = addr_list[0]->s_addr;
   
   //hostname_to_ip (const_cast<char *>(m_name.c_str()), &ip);
   //printf ("Name->ip %s -> %8.8x\n", m_name.c_str (), ip);
   
   m_ip = ip;
}
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */
