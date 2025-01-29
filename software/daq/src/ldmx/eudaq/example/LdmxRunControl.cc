// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/eudaq/LdmxRunControl.cc
  \brief  LDMX specific run control class
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
 * 2021.04.09 jjr Cloned from eudaq::Ex0RunControl.cc
 *
\* ---------------------------------------------------------------------- */


#include "eudaq/RunControl.hh"
#include <iostream>
#include <iomanip>


/* ====================================================================== */
/* Definition: LdmxRunControl                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Implements the LDMX Run Control within the EUDAQ framework

  \par
   This class inherits from the eudaq RunControl and implements the 
   special functionality required by LDMX
                                                                          */
/* ---------------------------------------------------------------------- */
class LdmxRunControl: public eudaq::RunControl
{
public:
   LdmxRunControl (const std::string &listenaddress);
   void Configure () override;
   void  StartRun () override;
   void   StopRun () override;
   void      Exec () override;


   // ------------------------------------------------------------------
   //  Identifies this class when registering to the main eudaq:Factory.
   //
   //  An alternate design choice could have been by simple the name.
   //  The hash lookup is likely more economical and perhaps faster
   //  Since, I think, this is only done once, why this is necessary
   //  should be looked into.
   //
   // ------------------------------------------------------------------
   static const uint32_t m_id_factory = eudaq::cstr2hash("LdmxRunControl");

private:
   uint32_t                               m_stop_second;
   bool                                  m_flag_running;
   std::chrono::steady_clock::time_point m_tp_start_run;
};
/* ---------------------------------------------------------------------- */
/* END DEFINITION: LdmxRunControl                                         */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION: LdmxRunControl                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Registers LdmxRunControl

  \par
   This is EUDAQ's method of including experiment specific functionality.
   This gets invoked when the shareable containing this code is loaded
   via dlopen. This code is invoked as a static initializer meaning it
   runs before the first line of the \i main routine. 

   To debug this, one must set the breakpoint immediately on entering 
   the gdb.  Because at that point the shareable has not been mapped,
   the gdb will not know anything about this code, so will ask if you
   want to set this as a future breakpoint.  Since the breakpoint cannot
   be confirmed, you have to be very careful that you have correctly 
   specified the breakpoint.
                                                                          */
/* ---------------------------------------------------------------------- */
namespace
{
   auto dummy0 = eudaq::Factory<eudaq::RunControl>::
                 Register<LdmxRunControl,
                          const std::string&>(LdmxRunControl::m_id_factory);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific RunControl constructor

  \param[in] listenaddress  The TPC/IP address used by other processes
                            to communicate with the LDMX run control
                            process.
                                                                          */
/* ---------------------------------------------------------------------- */
LdmxRunControl::LdmxRunControl(const std::string & listenaddress) :
    RunControl (listenaddress)
{
   // -----------------------------------------------------------------
   // Introductory message -- should be removed, was just for debugging
   // -----------------------------------------------------------------
   std::cout << "Constructing the run control" << std::endl;
   m_flag_running = false;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific StartRun method
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxRunControl::StartRun ()
{
   // Execute the generic start run method
   RunControl::StartRun ();

   
   // -----------------------------------------------------------------
   // Introductory message -- should be removed, was just for debugging
   // -----------------------------------------------------------------
   std::cout << "Starting the run" << std::endl;

   
   // Note the start time and set the run running flag
   m_tp_start_run = std::chrono::steady_clock::now();
   m_flag_running = true;

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific StopRun method
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxRunControl::StopRun ()
{
   // Execute the generic start run method   
   RunControl::StopRun ();

   // -----------------------------------------------------------------
   // Introductory message -- should be removed, was just for debugging
   // -----------------------------------------------------------------   
   std::cout << "Stopping the run" << std::endl;
   
   // Clear the run running flag
   m_flag_running = false;

   return;
}
/* ---------------------------------------------------------------------- */

void LdmxRunControl::Configure ()
{
   // Get the configuration information
  auto conf = GetConfiguration();

  
  // -----------------------------------------------------------------
  // Introductory message -- should be removed, was just for debugging
  // -----------------------------------------------------------------   
  std::cout << "Configuring run" << std::endl;

  
  // Extract the run time in seconds
  m_stop_second = conf->Get("EX0_STOP_RUN_AFTER_N_SECONDS", 0);

  
  // Execute the generic configuration
  RunControl::Configure();

  return;
}
/* ---------------------------------------------------------------------- */

void LdmxRunControl::Exec ()
{
  StartRunControl ();

  // -------------------------------------------------------
  // Poll to see if the run time has expired
  // A fancier implementation would be set a time interrupt,
  // but, so far, this is just a demonstration piece of code.
  // --------------------------------------------------------
  while (IsActiveRunControl())
  {
    if(m_flag_running && m_stop_second)
    {
      auto tp_now = std::chrono::steady_clock::now();
      std::chrono::nanoseconds du_ts(tp_now - m_tp_start_run);
      if(du_ts.count()/1000000000>m_stop_second)
      {
         StopRun();
      }
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return;
}
/* ---------------------------------------------------------------------- */
/* END IMPLEMENTATION: LdmxRunControl                                     */
/* ====================================================================== */
