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
   bool                      m_flag_ts;
   bool                      m_flag_tg;
   uint32_t                 m_plane_id;
   FILE                   *m_file_lock;
   std::string        m_file_lock_path;
   std::chrono::milliseconds m_ms_busy;
   bool                  m_exit_of_run;
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

   // ----------------------------
   // This is the <ldmx>.conf file
   // ----------------------------
   auto conf = GetConfiguration ();
   conf->Print (std::cout);
   
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

