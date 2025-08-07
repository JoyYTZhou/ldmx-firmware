// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   LdmxBuilder.cc
  \brief  Ldmx event/fragment builder
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
 * ---------- --- -------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.02.12 jjr Moved to namespace ldmx::builder::client
 * 2018.11.30 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/Configuration.hh"
#include "LdmxBuilder.hh"

#include "Parameters.hh"
#include "Contributions.hh"
#include "TriggerReceiver.hh"
#include "TrackerReceiver.hh"
#include "RssiReceiver.hh"
#include "Builder.hh"

#include <cinttypes>
#include <cstdio>
#include <iostream>



static int  launch_tcp_builder  (ldmx::builder::client::Configuration const  &eb);
static int  launch_rssi_builder (ldmx::builder::client::Configuration const &cfg);

/* ---------------------------------------------------------------------- *//*!

  \brief main program for the LDMX Event/Fragment Builder

\* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   Parameters prms (argc, argv);

   ldmx::builder::client::Configuration cfg (static_cast<int>(prms.m_type),
                                             prms.m_nfragments);
   prms.configure (&cfg);


   if (prms.m_type == Parameters::ConnectionType::TcpIp)
   {
      int    status = launch_tcp_builder (cfg);
      return status;
   }

   else if (prms.m_type == Parameters::ConnectionType::Rssi)
   {
      int    status = launch_rssi_builder (cfg);
      return status;
   }

   else
   {
      fprintf (stderr,
               "LdmxBuilder: unrecognized transport protocol %d\n",
               static_cast<int>(prms.m_type));
      return -1;
   }

}
/* ---------------------------------------------------------------------- */



#include "TcpConnections.hh"

/* ---------------------------------------------------------------------- *//*!

  \brief Constructs the TcpIp version of the builder

  \param[in] prms The configuration parameters

\* ---------------------------------------------------------------------- */
static int launch_tcp_builder (ldmx::builder::client::Configuration const &cfg)
{
   ///char const *daqHost = dscs[0].m_name.c_str ();
   ///uint16_t    udpPort = prms.m_port;

   ///printf ("DaqHost:Port = %s:%d\n", daqHost, udpPort);

   ///TcpConnections connections (udpPort, 0x8000, 1);
   ///connections.open    ();
   ////connections.connect ();
   return 0;
}
/* ---------------------------------------------------------------------- */


class Contributor;

#include "ldmx/utl/List.hh"
#include "ldmx/utl/Timeout.hh"


#define REPORT_LDMX_BUILDER 0
#define REPORT_POST         0
#define ANNOUNCE_POST       1




/* ====================================================================== */
namespace ldmx    {
namespace builder {
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: builder                                                   */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */


namespace client { namespace contribution { class Contribution; }}


namespace ldmx_builder
{

   #if REPORT_LDMX_BUILDER

   inline static void report (uint32_t valid, uint32_t needed)
   {
      printf ("LdmxBuilder::Valid set = %8.8" PRIx32 " %8.8" PRIx32 "\n",
              valid, needed);
      return;
   }

   inline static void reportTimeout (uint64_t timeout)
   {
      printf ("\nLdmxBuilder::Timeout set to = %" PRId64 "\n", timeout);
      return;
   }

   inline static void reportTimedout (uint32_t inHand)
   {
      printf ("LdmxBuilder::promote  promoting %8.8" PRIx32 "\n", inHand);
      return;
   }


   inline static void reportAppending (uint32_t newSet)
   {
      printf ("LdmxBuilder::appendW newSet = %8.8" PRIx32 "\n", newSet);
      return;
   }


   inline static void reportSeeding (uint32_t needed, uint32_t inHand)
   {
      printf ("LdmxBuilder::Contribution set needed = %8.8" PRIx32
                                         ": inHand = %8.8" PRIx32 "\n",
              needed, inHand);
      return;
   }

   inline static void reportSeeded (uint32_t needed)
   {
      printf ("LdmxBuilder::build still needs = %8.8" PRIx32 "\n", needed);
      return;
   }


   #else

   inline static void report          (uint32_t   valid,
                                       uint32_t  needed) { return; }
   inline static void reportTimeout   (uint64_t timeout) { return; }
   inline static void reportTimedout  (uint32_t  inHand) { return; }
   inline static void reportAppending (uint32_t  newSet) { return; }
   inline static void reportSeeding   (uint32_t  needed,
                                       uint32_t  inHand) { return; }
   inline static void reportSeeded    (uint32_t  needed) { return; }

   #endif
}


namespace post
{
   #if ANNOUNCE_POST

   inline static void announce (uint32_t sequence)
   {
      printf ("Posting fragment #: %8.8" PRIx32 "\n", sequence);
      return;
   }

   #else

   inline static void announce (uint32_t sequence) { return;

   #endif



   #if REPORT_POST

   inline static void reportTiming (int id,
                                    int ctbId,
                                    uint32_t sequence,
                                    uint64_t timestamp,
                                    uint64_t earliest,
                                    uint64_t lastest)
   {

      printf ("Timestamp[%d:%d.%5.5" PRIx32 "] = "
              "%16.16" PRIx64 " earliest:latest = %16.16" PRIx64 ":"
              "%16.16" PRIx64 " detla = %" PRId64 "\n",
              id, ctbId, sequence, timestamp, earliest, latest, latest - earliest);
      return;
   }


   inline static void reportStatus (uint32_t status)
   {
      printf ("Returning status = %d\n", status);
      return;
   }


   #else

   inline static void reportTiming (int             id,
                                    int          ctbId,
                                    uint32_t  sequence,
                                    uint64_t timestamp,
                                    uint64_t  earliest,
                                    uint64_t   lastest) { return; }

   inline static void reportStatus (uint32_t   status) { return; }

   #endif
}



/* ---------------------------------------------------------------------- *//*!

  \brief The posting class
                                                                          */
/* ---------------------------------------------------------------------- */
class MyBuilder : public Builder
{
public:
   MyBuilder () = delete;
   MyBuilder (uint32_t expected, int nfragments, Contributions *ctbs) :
      Builder   (expected, nfragments, ctbs),
      m_timeout (100 * 1000 * 1000)
   {
      return;
   }

public:
   int post (Fragment *fragment) override;

   uint64_t m_timeout; /*!< A timeout value for use on the next fragment  */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
int MyBuilder::post (Fragment *fragment)
{
   post::announce (fragment->m_sequence);

   uint64_t earliest = ~(0LL);
   uint64_t   latest =      0;

   uint32_t set = fragment->m_present;
   while (set)
   {
      int id = __builtin_ctz (set);
      auto const *ctb = fragment->m_ctbs[id];

      uint64_t timestamp = ctb->m_rcvTime;

      if (timestamp <= earliest) earliest = timestamp;
      if (timestamp >=   latest) latest   = timestamp;


      post::reportTiming (id,       ctb->m_id,  ctb->m_sequence,
                          timestamp, earliest,           latest);



      set &= ~(1 << id);
   }

   uint64_t delta = latest - earliest;


   delta    *= 5;
   m_timeout = (delta < 1000*1000) ? (1000 * 1000) : delta;


   delete fragment;
   //int status = Builder::post (fragment);
   //post::reportStatus (status);

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static int launch_rssi_builder (ldmx::builder::client::Configuration const &cfg)
{
   // Total up the number of contributions
   int ncontributions = 1                             //  Control contribution
                      + 1                           //  Trigger contribution
                      + 1                           //  Tracker contribution
                      + cfg.m_contributors.size (); //  SVT     contributions


   // Construct the synchonization class for incoming messages and data
   Contributions ctbs (ncontributions);


   //// Construct the control receiver at contribution = 0
   //// ControlReceiver controlReceiver (cfg, &ctbs, 0);


   // Construct the trigger receiver at contribution = 1
   TriggerReceiver trgReceiver (cfg, &ctbs, 1);

   // Construct the tracker receiver at contribution = 2
   TrackerReceiver trkReceiver (cfg, &ctbs, 2);

   // Construct the SVT    receivers at contribuion = 3 - 3 + nSVT receivers
   RssiReceiver    svtReceiver (cfg, &ctbs, 3);


   // Start/Enable the trigger receiver
   trgReceiver.start ();
   printf ("Trigger started\n");

   // Start/Enable the tracker receiver
   trkReceiver.start ();
   printf ("Tracker started\n");

   // Start/Enable the SVT contributor's connections
   svtReceiver.start  ();


   // Check that all connections started
   auto missing = svtReceiver.waitForConnections ();


   // Report any missing contributors and abort if any
   if (missing)
   {
      fprintf (stderr,
               "Aborting::missing contributors = %8.8" PRIx32 "\n",
               missing);
      return -1;
   }



#if 1

   ldmx::utl::List lists[32];
   uint32_t expected = svtReceiver.getContributors ();
//                     | trgReceiver.getContributors ();
   uint32_t   needed = expected;
   uint32_t   inHand = 0;


   // Configure the builder for the set of all possible contributors
   MyBuilder builder (expected, cfg.m_nfragments, &ctbs);


   while (1)
   {
      uint32_t valid;

      // Transfer the active lists of list to the local lists
      if (inHand == 0)
      {
         // ---------------------------------------------------
         // If looking for all contributors, no timeout
         // Note that the arrival of the next event is entirely
         // dependent on the trigger rate, so there is no
         // sensible timeout until some contribution arrives
         //
         // Eventually will add control lists so the 0 will
         // no longer be 0.  Therefore, need to trim the inHand
         // so only the list expected.
         // ---------------------------------------------------
         valid   = ctbs.appendW (lists, 0, needed);

         ldmx_builder::report (valid, needed);

         inHand  = expected &   valid;
         needed  = expected & ~inHand;
      }
      else
      {
         // --------------------------------------------------------
         // Awaiting for an partially complete fragment to complete.
         // Here a timeout is warranted.  All contributors should
         // be reasonably prompt in promoting their contributions.
         // --------------------------------------------------------
         builder.m_timeout = 1000 * 1000 * 1000;
         ldmx_builder::reportTimeout (builder.m_timeout);


         ldmx::utl::Timeout timeout (builder.m_timeout);
         uint32_t valid  = ctbs.appendW (lists, 0, needed, &timeout);
         uint32_t newSet = valid & expected;

         // Check for timeout
         if (newSet == 0)
         {
            // -----------------------------------
            // Timed out, promote what is in hand.
            // -----------------------------------
            ldmx_builder::reportTimedout (inHand);
            needed = 0;
         }
         else
         {
            // ---------------------------
            // Got some new contributions.
            // Add to the inHand set
            // ---------------------------
            ldmx_builder::reportAppending (newSet);
            inHand |= newSet;
            needed  = expected & ~inHand;
         }
      }

      ldmx_builder::reportSeeding (needed, inHand);


      // ------------------------------------------------------------
      // Attempt build only after all needed contributors are present
      //
      // Note: this is in a loop, so as long as no more contributors
      // are needed, just keep attempting to build fragments.
      // ------------------------------------------------------------
      while  (needed == 0)
      {
         uint64_t timestamp = ldmx::utl::Timestamp::current ();
         uint32_t   emptied = builder.build (lists, inHand, timestamp);

         ldmx_builder::reportSeeded (needed);

         inHand &= ~emptied;
         needed  =  expected & ~inHand;
      }

   }
   #endif


   trgReceiver.m_thread.join ();
   return 0;
}
/* ---------------------------------------------------------------------- */



#if 0
/* ---------------------------------------------------------------------- *//*!

  \brief  Primitive hex dump routine

  \param[in]   d Pointer to the data to be dumped
  \param[in]   n The number of 64-bit words to dump
                                                                          */
/* ---------------------------------------------------------------------- */
static void dump (uint64_t const *d, int n)
{
   for (int idx = 0; idx < n; idx++)
   {
      if ( (idx & 0x3) == 0) printf ("%2x:", idx);

      printf (" %16.16" PRIx64 ,  d[idx]);

      if ((idx & 0x3) == 3) putchar ('\n');
   }

   if (n & 0x3) putchar ('\n');

   return;
}
/* ---------------------------------------------------------------------- */
#endif
