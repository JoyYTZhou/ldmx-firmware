// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_TCPCONNECTIONS_HH__
#define __LDMX_BUILDER_CLIENT_TCPCONNECTIONS_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/TcpConnections.hh
  \brief  Maintains the list of and active connections
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
 * 2018.12.14 jjr Created
 *
\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class TcpConnections
  \brief Maintains the list of and active connections

\* ---------------------------------------------------------------------- */
class TcpConnections
{
public:
   TcpConnections (int port,
                   int rcvSize,
                   int nodelay);


   int open    ();
   int connect ();

private:
   int     m_port;
   int  m_rcvSize;
   int  m_nodelay;
   int m_listenFd;
   int m_clientFd;
};
/* ---------------------------------------------------------------------- */

#endif


