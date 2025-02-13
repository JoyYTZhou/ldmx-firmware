// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Parameters.cc
  \brief  LDMX builder command line parameter extraction
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
 * 2019.03.29 jjr Added number of trigger events/batch
 * 2019.03.18 jjr Added number of RSSI frame and SVT events/batch
 * 2019.01.08 jjr Moved into hps::builder::client namespace
 * 2019.01.08 jjr Moved into hps::eb::client namespace
 * 2018.12.21 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/builder/client/Configuration.hh"
#include "ldmx/builder/client/CfgContributor.hh"


#include "Parameters.hh"


#include <getopt.h>
#include <cstdio>
#include <iostream>
#include <string.h>
#include <cinttypes>


/* ---------------------------------------------------------------------- */  
static uint16_t getPort (const char *src, int *len, uint16_t port);


/* ---------------------------------------------------------------------- *//*!

  \brief Extracts the command line parameters

\* ---------------------------------------------------------------------- */
Parameters::Parameters (int argc, char *const argv[])
{
   static struct option options[] = 
   {
      { "none",      no_argument, 0, static_cast<int>(ConnectionType::None)  },
      { "rssi",      no_argument, 0, static_cast<int>(ConnectionType::Rssi)  },
      { "tcpip",     no_argument, 0, static_cast<int>(ConnectionType::TcpIp) },
      { "batch",         required_argument, 0, 'E' },
      { "events",        required_argument, 0, 'e' },
      { "srcs",          required_argument, 0, 's' },
      { "port",          required_argument, 0, 'p' },
      { "rssiFrames",    required_argument, 0, 'r' },
      { "nfragments",    required_argument, 0, 'f' },
      { "triggerBatches",required_argument, 0, 'T' },
      { "triggerEvents", required_argument, 0, 't' },
      { "triggerPipe",   required_argument, 0, 'P' },
      { "timeout",       required_argument, 0,  1  },
      { 0, 0, 0, 0}
   };

   int      type          = static_cast<int>(ConnectionType::None);
   char    *srcs          = NULL;
   uint16_t port          = 8090;
   int nfragments         =   10; /* Number of output events to buffer     */ 

   int timeout            =    2;

   int ntrgBatches        =    8; /* Nubmer of trigger batches             */
   int ntrgEventsPerBatch =  128; /* NUmber of trigger events per batch    */
   int nrssiFrames        =   16; /* Number input RSSI frames to buffer    */

   int nsvtBatches        =    8;
   int nsvtEventsPerBatch =   64;
   int nsvtEvents         =   nsvtBatches*nsvtEventsPerBatch; 
   char const 
          *triggerPipe = "/tmp/ldmxTriggerPipe";

   while (1)
   {
      int option_index = 0;
      int c = getopt_long (argc, argv,
                           "e:E:t:T:p:P:f:r:s:", 
                           options, &option_index);

      if (c == -1)
      {
         break;
      }

      if (option_index  <= 2) type = options[option_index].val;
      else
      {
         switch (c)
         {
         case  0 : {                                               break; }
         case  1 : {  timeout            = strtoul (optarg, 0, 0); break; }
         case 'e': {  nsvtEvents         = strtoul (optarg, 0, 0); break; }
         case 'f': {  nfragments         = strtoul (optarg, 0, 0); break; } 
         case 'p': {  port               = strtoul (optarg, 0, 0); break; }
         case 'r': {  nrssiFrames        = strtoul (optarg, 0, 0); break; }
         case 'T': {  ntrgBatches        = strtoul (optarg, 0, 0); break; }
         case 't': {  ntrgEventsPerBatch = strtoul (optarg, 0, 0); break; }
         case 'P': {  triggerPipe        = optarg;                 break; }
         case 's': {  srcs               = optarg;                 break; }
         }
      }
   }

   m_type               = static_cast<decltype(m_type)>(type);
   m_triggerPipe        = triggerPipe;
   m_srcs               = srcs;
   m_port               = port;
   m_toConnection       = timeout;
   m_level              = ldmx::builder::Logging::Level::Error;
   m_nfragments         = nfragments;
   m_ntrgBatches        = ntrgBatches;
   m_ntrgEventsPerBatch = ntrgEventsPerBatch;
   m_nsvtEvents         = nsvtEvents;
   m_nrssiFrames        = nrssiFrames;

   //printf ("Type = %d\n", type);
   //printf ("Srcs = %s\n", srcs);
   //printf ("Port = %d\n", port);


   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*~

  \brief Translates the command line extracted parameterization to the
         universal configuation

  \param[in] cfg  The universal configuration
                                                                          */
/* ---------------------------------------------------------------------- */
int Parameters::configure (ldmx::builder::client::Configuration *cfg) const
{
   // ------------------------------------------------------------------
   // NOTE:
   // -----
   // The number of input SVT events to buffer is rounded to an integral
   // number of SVT events in a SVT batch
   // ------------------------------------------------------------------
   translate_srcs (cfg);
   cfg->m_triggerPipe       = m_triggerPipe;
   cfg->m_connectionTimeout = m_toConnection;
   cfg->m_level             = m_level;
   cfg->m_ntrgBatches       = m_ntrgBatches;
   cfg->m_ntrgEvents        = m_ntrgEventsPerBatch * m_ntrgBatches;
   cfg->m_nrssiFrames       = m_nrssiFrames;
   cfg->m_nsvtEvents        = m_nsvtEvents;
   cfg->m_nfragments        = m_nfragments;

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Parses the list of IP sources, forming a vector of IP pairs, 
          where a pair is the original name plus the binary IP 
          representation
  \return The number of IP pairs
  
  \param[in] ips The vector of IP pairs to fill
                                                                          */
/* ---------------------------------------------------------------------- */
int Parameters::translate_srcs (ldmx::builder::client::Configuration *cfg) const
{
   const char *src = m_srcs;

   uint16_t port = m_port;

   while (1)
   {
      const char *end = strchr (src, ',');
      int         len = (end == NULL) ? strlen (src) : end - src;

      if (len)
      {
         uint16_t lclPort = getPort (src, &len, port);
         cfg->addContributor 
             (src, 
              len,
              lclPort, 
              m_toConnection,
              m_nrssiFrames, 
              m_nsvtEvents);
      }

      if (end == NULL) break;
      src     = end + 1;
   }


   /*
   std::vector<ldmx::builder::client::ConnectionDsc> const 
        *dscs = &cfg->m_connectionDscs;

   for (std::vector<ldmx::builder::client::ConnectionDsc>::const_iterator 
        it = dscs->begin(); 
        it != dscs->end(); 
      ++it)
   {
      std::cout << ' ' << it->m_name << " -> " << std::hex << it->m_ip << std::endl;
   }
   std::cout << '\n';
   */

   return cfg->m_contributors.size ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Checks if the IP source string includes a port number. This will
          be the integer that occurs after any potential ':' character
  \return The port number

  \param[in]     src The IP source string
  \param[inout]  *len The length of the IP source string
  \param[in] port The default port number to use if the IP source string 
                  does not contain its own port number

\* ---------------------------------------------------------------------- */
static uint16_t getPort (const char *src, int *len, uint16_t port)
{
   for (int idx = *len - 1; idx >= 0; --idx)
   {
      if (src[idx] == ':') 
      {
         port = strtoul (&src[idx + 1], NULL, 0);
        *len = idx;
         break;
      }
   }

   return port;
}
/* ---------------------------------------------------------------------- */

