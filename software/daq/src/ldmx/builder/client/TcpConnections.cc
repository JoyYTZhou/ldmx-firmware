// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/TcpConnections.cc
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
 * 2019.02.12 jjr Moved to namespace hps::builder;:client
 * 2018.12.14 jjr Created
 *
\* ---------------------------------------------------------------------- */

#include "TcpConnections.hh"
#include <cinttypes>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>


TcpConnections::TcpConnections (int           port, 
                                int        rcvSize, 
                                int        nodelay) :
    m_port    (   port),
    m_rcvSize( rcvSize),
    m_nodelay (nodelay)
{
   printf ("TpcConnections: configuring\n");
   return;
}


int TcpConnections::open ()
{
   int     listenFd;

   struct sockaddr_in srvAdr;
   printf  ("Listening socket\n");


   // Init structures
   listenFd = socket (AF_INET,  SOCK_STREAM,  0);
   memset (&srvAdr, 0, sizeof (srvAdr));

   printf ("Got listening socket %d\n", listenFd);

   srvAdr.sin_family      = AF_INET;
   srvAdr.sin_addr.s_addr = INADDR_ANY;
   srvAdr.sin_port        = htons(m_port);


   if (bind (listenFd, (struct sockaddr *) &srvAdr, sizeof(srvAdr)) < 0) 
   {
      printf ("ControlServer::startListen -> Failed to bind socket %5d\n",
              m_port);
      return -1;
   }
   

   //m_clientFd = connect_client (listenFd, port, rcvSize, nodelay);
   m_listenFd = listenFd;
   return listenFd;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
int TcpConnections::connect ()
{
    socklen_t cliLen;
    int        cliFd;
    struct sockaddr_in  cliAddr;


    // Start listen
    printf ("Listening for a connection on port %5d\n", m_port);
    listen (m_listenFd, 5);
    
   
    cliLen = sizeof (cliAddr);
    cliFd  = accept (m_listenFd, (struct sockaddr *)&cliAddr, &cliLen);
    if(cliFd < 0)
    {
       puts ("Error on Accept");
    }    
    else
    {
       puts ("Accepted connection from client");
       setsockopt (cliFd, SOL_SOCKET, SO_RCVBUF, &m_rcvSize, sizeof (int));


       int iss;
       iss = setsockopt (cliFd, SOL_TCP, TCP_NODELAY, &m_nodelay, sizeof(int));
       if (iss < 0)
       {
          printf ("Error setting NODELAY flag %d\n", h_errno);
       }

    }

    return cliFd;
}



#if 0
/* ---------------------------------------------------------------------- *//*!

  \brief  Tries to reconnect to the client after a disconnect
  \return The reconnected socket fd

  \parampin]    nread  The last return value from rcv
  \param[in] clientFd  The old client socket fd (to be closed)
  \param[in] serverFd  The socket fd to accept the new connection
  \param[in]     prms  The connection parameters
  \param[in]      msg  A string indicating the reason for disconnecting
                                                                          */
/* ---------------------------------------------------------------------- */
static int reconnect_client (ssize_t     nread,
                             int      clientFd,
                             int      serverFd,
                             Prms const  *prms,
                             char  const  *msg)
{
   if (nread == 0)
   {
      printf ("Disconnect while %s\n"
              "Try reconnecting\n",  msg);

      close (clientFd);
      clientFd = connect_client (serverFd, 
                                 prms->portNumber, 
                                 prms->rcvSize,
                                 prms->nodelay);
   }
   else if (nread < 0)
   {
      printf ("Receive error while %s, quitting\n", msg);
      exit (-1);
   }

   
   return clientFd;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Print a couple of the critical options for the specified socket

  \param[in] fd  The socket's fd
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_socketopts (int fd)
{
    int       rcvSize;
    int       nodelay;
    socklen_t optSize;

    /* Retrieve the socket's receive buffer size */
    optSize = sizeof (rcvSize);
    getsockopt (fd, SOL_SOCKET, SO_RCVBUF, &rcvSize, &optSize);


    /* Retrieve whether the ACK's are delayed (Nagle Algorithm) or not */
    optSize = sizeof (nodelay);
    getsockopt (fd, SOL_TCP, TCP_NODELAY, &nodelay, &optSize);


    printf ("SO_RCVBUF   = %8.8x\n", rcvSize);
    printf ("TCP_NODELAY = %8d\n",   nodelay);

    return;
}
/* ---------------------------------------------------------------------- */

#endif

/*
static int     open_client       (int         portno, 
                                  int        rcvSize, 
                                  int        nodelay, 
                                  int        *servFd);


static int     reconnect_client  (ssize_t      nread,
                                  int       clientFd,
                                  int        serverFd,
//                                  Prms const   *prms,
                                  char const    *msg);

static void    print_socketopts  (int             fd);
*/
