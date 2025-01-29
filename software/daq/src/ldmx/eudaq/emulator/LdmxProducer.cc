// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/rc/LdmxProducer.cc
  \brief  LDMX specific (fake) data producerrun class
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
 * 2021.04.16 jjr Saved lock file path, used to remove the lock file upon
 *                termination or reset
 * 2021.04.10 jjr Cloned from eudaq::Ex0Producer.cc
 *
\* ---------------------------------------------------------------------- */


#include "eudaq/Producer.hh"

#include "ldmx/builder/client/Configuration.hh"
#include "ldmx/builder/client/CfgParser.hh"

#include "../src/ldmx/builder/client/Contributions.hh"
#include "../src/ldmx/builder/client/TriggerReceiver.hh"
#include "../src/ldmx/builder/client/RssiReceiver.hh"
#include "../src/ldmx/builder/client/Builder.hh"

#include <cinttypes>
#include <cstdio>
#include <iostream>



#include <iostream>
#include <iomanip>
#include <fstream>
#include <ratio>
#include <chrono>
#include <thread>
#include <random>

#ifndef _WIN32
#include <sys/file.h>
#endif


/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static void print (int                     plane_id,
                   uint8_t                  x_pixel,
                   uint8_t                  y_pixel,
                   uint32_t                position,
                   uint32_t                  signal,
                   std::vector<uint8_t> const &data);

static int  launch_rssi_builder
                  (ldmx::builder::client::Configuration const &cfg);
/* ====================================================================== */



/* ====================================================================== */
/* Definition: LdmxProducer                                               */
/* ---------------------------------------------------------------------- *//*!

  \brief Implements a (fake) LDMX data producer within the EUDAQ framework
                                                                          */
/* ---------------------------------------------------------------------- */
class LdmxProducer : public eudaq::Producer
{
public:
   LdmxProducer(const std::string & name, const std::string & runcontrol);

   void DoInitialise () override;
   void  DoConfigure () override;
   void   DoStartRun () override;
   void    DoStopRun () override;
   void  DoTerminate () override;
   void      DoReset () override;
   void      RunLoop () override;

   // ------------------------------------------------------------------
   //  Identifies this class when registering to the main eudaq:Factory.
   // ------------------------------------------------------------------
   static const uint32_t m_id_factory = eudaq::cstr2hash("LdmxProducer");

private:
   int launch_rssi_builder (ldmx::builder::client::Configuration const &cfg);
   
private:
   bool                             m_flag_ts;
   bool                             m_flag_tg;
   uint32_t                        m_plane_id;
   FILE                          *m_file_lock;
   std::string               m_file_lock_path;
   std::chrono::milliseconds        m_ms_busy;
   bool                         m_exit_of_run;
   ldmx::builder::client::Configuration m_cfg;
   Contributions                      *m_ctbs;
   TriggerReceiver             *m_trgReceiver;
   RssiReceiver                *m_svtReceiver;
};
/* ---------------------------------------------------------------------- */
/* END DEFINITION: LdmxProducer                                           */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: LdmxProducer                                           */
/* ---------------------------------------------------------------------- *//*!

  \brief Registers LdmxProducer with the EUDAQ framework
                                                                          */
/* ---------------------------------------------------------------------- */
namespace
{
   auto dummy0 = eudaq::Factory<eudaq::Producer>::
                 Register<LdmxProducer,
                          const std::string &,
                          const std::string &> (LdmxProducer::m_id_factory);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief The LDMX specific (fake) Data Producer
                                                                          */
/* ---------------------------------------------------------------------- */
LdmxProducer::LdmxProducer (const std::string       &name, 
                            const std::string &runcontrol) :
   eudaq::Producer (name, runcontrol),
   m_file_lock                    (0),
   m_file_lock_path              (""),
   m_exit_of_run              (false)
{
   std::cout << "Ldmx    Producer name: " <<       name << std::endl
             << "     Run Control name: " << runcontrol << std::endl;
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Method executed upon run initialization
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::DoInitialise ()
{
   // ---------------------------
   // This is the <ldmx>.ini file
   // ---------------------------
   auto               ini = GetInitConfiguration ();
   m_file_lock_path = ini->Get ("LDMX_DEV_LOCK_PATH",
                                "ldmxlockfile.txt");

   std::cout << "Producer::DoInitialise lock path: "
             << m_file_lock_path.c_str() << std::endl;
      
   m_file_lock = fopen (m_file_lock_path.c_str(), "a");


   enum class ConnectionType
   {
      None = 0,
      Tcp  = 1,
      Rssi = 2
   };
   
   // Get the configuration information
   //auto conf = GetConfiguration ();

  
  // -----------------------------------------------------------------
  // Introductory message -- should be removed, was just for debugging
  // -----------------------------------------------------------------   
  std::cout << "Iniatializing Data Producer" << std::endl;


  ldmx::builder::client::Configuration &cfg = m_cfg;

  std::string cfgFile = ini->Get ("LDMX_CONFIGURATION", "");
  CfgParser cfgParser(cfgFile.c_str());
  cfgParser.parse (&cfg);

  #if 1
  constexpr static auto Level = ldmx::builder::Logging::Level::Error;

  cfg.m_name               = "LdmxDataEmulator";
  cfg.m_type               = static_cast<int>(ConnectionType::Rssi);
  cfg.m_nfragments         = ini->Get ("LDMX_NFRAGMENTS",                   10);

  ///std::string ctbs         = ini->Get ("LDMX_SOURCES",                      "");
  ///std::cout << "Ctbs = " << ctbs << std::endl;

  
  cfg.m_connectionTimeout  = ini->Get ("LDMX_CONNECTION_TIMEOUT",           10);
  cfg.m_triggerPipe        = ini->Get ("LDMX_TRIGGER_PIPE",
                                        "/tmp/triggerPipe").c_str();


  cfg.m_ntrgBatches        = ini->Get ("LDMX_NTRIGGER_BATCHES",             8);
  cfg.m_ntrgEventsPerBatch = ini->Get ("LDMX_NTRIGGER_EVENTS_PER_BATCH",  128);
  cfg.m_ntrgEvents         = cfg.m_ntrgEventsPerBatch * cfg.m_ntrgBatches;

  cfg.m_nrssiFrames        = ini->Get ("LDMX_NRSSI_FRAMES",                16);    

  cfg.m_nsvtBatches        = ini->Get ("LDMX_NSVT_BATCHES",               8);  
  cfg.m_nsvtEventsPerBatch = ini->Get ("LDMX_NSVT_EVENTS_PER_BATCH",     64);
  cfg.m_nsvtEvents         = cfg.m_nsvtEventsPerBatch * cfg.m_nsvtBatches;

  cfg.m_level              = static_cast<decltype(Level)>
                             (ini->Get ("LDMX_LEVEL",    static_cast<int>(Level)));
#endif   
  cfg.print ();

#ifndef _WIN32
   if (flock (fileno(m_file_lock), LOCK_EX | LOCK_NB))
   {
      // -- Failed to get lock
      EUDAQ_THROW ("unable to lock the lockfile: " + m_file_lock_path);
   }
  #endif

   
   return;  
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Method executed upon run configuration

  \par
   As opposed to the initialization step, which is a one time call upon
   startup, the run configuration can be executed at the start of every
   run.
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::DoConfigure ()
{
   std::cout << "Producer::DoConfigure" << std::endl;

   auto &cfg = m_cfg;

      // Total up the number of contributions
   int ncontributions = 1                             //  Control contribution
                      + 1                           //  Trigger contribution
                      + cfg.m_contributors.size (); //  SVT     contributions
                      

   // Construct the synchonization class for incoming messages and data
   m_ctbs = new Contributions (ncontributions);


   //// Construct the control receiver at contribution = 0
   //// ControlReceiver controlReceiver (cfg, m_ctbs, 0);


   // Construct the trigger receiver at contribution = 1
   m_trgReceiver = new TriggerReceiver (cfg, m_ctbs, 1);


   // Construct the SVT    receivers at contribuion = 2 - 2 + nSVT receivers
   m_svtReceiver = new RssiReceiver    (cfg, m_ctbs, 2);



   // ----------------------------
   // This is the <ldmx>.conf file
   // ----------------------------
   auto conf = GetConfiguration ();
   //conf->Print (std::cout);
   
   m_plane_id = conf->Get ("LDMX_PLANE_ID", 0);
   m_ms_busy  = std::chrono::milliseconds(conf->Get ("LDMX_DURATION_BUSY_MS",
                                                     1000));

   m_flag_ts  = conf->Get ("LDMX_ENABLE_TIMESTAMP",     0);
   m_flag_tg  = conf->Get ("LDMX_ENABLE_TRIGGERNUMBER", 0);
   
   if(!m_flag_ts && !m_flag_tg)
   {
      EUDAQ_WARN ("Both Timestamp and TriggerNumber are disabled. "
                  "Now, Timestamp is enabled by default");
      
      m_flag_ts = false;
      m_flag_tg =  true;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Method to be executed on start of run
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::DoStartRun ()
{
   std::cout << "Producer::DoStartRun" << std::endl;

   m_exit_of_run = false;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Method to be executed on stopping of run
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::DoStopRun ()
{
   std::cout << "Producer::DoStopRun" << std::endl;
      
   m_exit_of_run = true;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Method to be executed on a reset
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::DoReset ()
{
   std::cout << "Producer::Reset" << std::endl;
   
   m_exit_of_run = true;
   if (m_file_lock)
   {
      #ifndef _WIN32
         flock (fileno (m_file_lock), LOCK_UN);
      #endif
         
      fclose(m_file_lock);
      remove(m_file_lock_path.c_str());
      m_file_lock = 0;
  }
   
  m_ms_busy     = std::chrono::milliseconds();
  m_exit_of_run = false;

  return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Method to be executed upon EUDAQ termination
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::DoTerminate()
{
   std::cout << "Producer::DoTerminate" << std::endl;
   
   m_exit_of_run = true;
   
   if (m_file_lock)
   {
      fclose(m_file_lock);
      remove(m_file_lock_path.c_str());
      m_file_lock = 0;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  The acquistion loop
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxProducer::RunLoop ()
{
   std::cout << "Producer::RunLoop" << std::endl;

   //launch_rssi_builder (m_cfg);
   //return;

   auto  tp_start_run = std::chrono::steady_clock::now();
   uint32_t trigger_n = 0;
   uint8_t    x_pixel = 16;
   uint8_t    y_pixel = 16;
   std::random_device   rd;

   // ------------------------------------
   // Initialize a random number generator
   // ------------------------------------
   std::mt19937 gen (rd ());

   
   // ------------------------------------------------------------
   // Request a flat distribution for both the position of the hit
   // and its signal strength
      // ------------------------------------------------------------
   std::uniform_int_distribution<uint32_t> position (0, x_pixel*y_pixel-1);
   std::uniform_int_distribution<uint32_t> signal   (0, 255);

   auto str_plane_id = std::to_string (m_plane_id);

   // ------------------------
   // Generate fake event loop
   // ------------------------
   while (!m_exit_of_run)
   {
      // -----------------------------------------------------------
      // Create a new event and set its tag as Plane ID = <plane_id>
      // -----------------------------------------------------------
      auto             ev = eudaq::Event::MakeUnique ("LdmxRaw");
      ev->SetTag("Plane ID", str_plane_id);

      
      // ------------------------------------------------------
      // Set the time to wait till generation of the next event
      // ------------------------------------------------------
      auto     tp_trigger = std::chrono::steady_clock::now ();
      auto tp_end_of_busy = tp_trigger + m_ms_busy;

      
      // ----------------------------------------------------
      // Set the beginning and ending timestamp of this event
      // ----------------------------------------------------
      if(m_flag_ts)
      {
         std::chrono::nanoseconds du_ts_beg_ns (tp_trigger     - tp_start_run);
         std::chrono::nanoseconds du_ts_end_ns (tp_end_of_busy - tp_start_run);
         ev->SetTimestamp (du_ts_beg_ns.count (), du_ts_end_ns.count ());
      }

      // ----------------------
      // Set the trigger number
      // ----------------------
      if (m_flag_tg)
      {
         ev->SetTriggerN (trigger_n);
      }

      // -----------------------------------------
      // Initialize an empty hit vector
      // Generate 1 signal hit 's' at position 'p'
      // -----------------------------------------
      std::vector<uint8_t> hit (x_pixel * y_pixel, 0);
      auto p = position (gen);
      auto s = signal   (gen);
      hit[p] = s;

      // ------------------------------------------------------
      // Create the data event
      //     0: X_PIXEL size (16)
      //     1: Y_PIXEL size (16_
      // 2-257: The hits, only postion 'p' has a non-zero value
      // ------------------------------------------------------
      std::vector<uint8_t> data;
      data.push_back (x_pixel);
      data.push_back (y_pixel);
      data.insert    (data.end(), hit.begin(), hit.end());


      // --------------------------------
      // Simple display of the fake event
      // --------------------------------
      print (m_plane_id, x_pixel, y_pixel, p, s, data);


      // -----------------------------------
      // Add this block of data to the event
      // Send it to the data collector
      // -----------------------------------
      uint32_t block_id = m_plane_id;
      ev->AddBlock (block_id,  data);
      SendEvent    (std::move  (ev));

      // ------------------
      // Next trigger/event
      // ------------------
      trigger_n++;
      std::this_thread::sleep_until (tp_end_of_busy);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void print (int                     plane_id,
                   uint8_t                  x_pixel,
                   uint8_t                  y_pixel,
                   uint32_t                position,
                   uint32_t                  signal,
                   std::vector<uint8_t> const &data)
{
         
   // ---------------------------
   // Simple display of the event
   // ---------------------------
   std::cout << "Plane:X:Y  "
             << plane_id                       << ':'
             << static_cast<uint16_t>(x_pixel) << ':'
             << static_cast<uint16_t>(y_pixel) << "   "
             << "hit[" << std::setw(3) << position    << "] = " << signal
             << std::endl;
      
   int idx = -2;
   for (auto x: data)
   {
      if (x)
      {
         std::cout << "data[" << std::setw (3) << idx << "] = "
                   << (int)x  << std::endl;
      }

      idx += 1;
   }

   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */


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
int LdmxProducer::launch_rssi_builder (ldmx::builder::client::Configuration const &cfg)
{
   Contributions          &ctbs = *m_ctbs;
   TriggerReceiver &trgReceiver = *m_trgReceiver;
   RssiReceiver    &svtReceiver = *m_svtReceiver;

   // Start/Enable the trigger receiver
   trgReceiver.start ();
   printf ("Trigger started\n");


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


   while (!m_exit_of_run)
   {
      uint32_t valid;

      std::cout << "-------------------------------------\n"
                << "launch_rssi_builder exit: " << m_exit_of_run
                << "\n-------------------------------------" << std::endl;

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










