// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/rc/LdmxMonitor.cc
  \brief  LDMX specific the event monitoring class
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
 * 2021.04.14 jjr Cloned from eudaq::Ex0Monitor.cc
 *
\* ---------------------------------------------------------------------- */


#include "eudaq/Monitor.hh"
#include "eudaq/StandardEvent.hh"
#include "eudaq/StdEventConverter.hh"

#include <iostream>
#include <fstream>
#include <ratio>
#include <chrono>
#include <thread>
#include <random>



/* ====================================================================== */
/* DEFINITION: LdmxMonitor                                                */
/* ---------------------------------------------------------------------- *//*!

  \brief LDMX specific eudaq monitoring class
                                                                          */
/* ---------------------------------------------------------------------- */
class LdmxMonitor : public eudaq::Monitor
{
public:
  LdmxMonitor(const std::string & name, const std::string & runcontrol);
   
  void DoInitialise () override;
  void DoConfigure  () override;
  void DoStartRun   () override;
  void DoStopRun    () override;
  void DoTerminate  () override;
  void DoReset      () override;
  void DoReceive    (eudaq::EventSP ev) override;
  
  static const uint32_t m_id_factory = eudaq::cstr2hash("LdmxMonitor");
  
private:
  bool         m_en_print;
  bool m_en_std_converter;
  bool     m_en_std_print;
};
/* ---------------------------------------------------------------------- */
/* END DEFINITION: LdmxMonitor                                            */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: LdmxMonitor                                            */
/* ---------------------------------------------------------------------- *//*!

  \bief Register LdmxMonitor with eudaq's factory
                                                                          */
/* ---------------------------------------------------------------------- */
namespace
{
   auto dummy0 = eudaq::Factory<eudaq::Monitor>::
                 Register<Ldmx0Monitor,
                          const std::string &,
                          const std::string &> (LdmxMonitor::m_id_factory);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific Monitor constructor

  \param[in] name  The name of this specific data collector
  \param[in]   rc  The name of run controller
                                                                          */ 
/* ---------------------------------------------------------------------- */ 
LdmxMonitor::LdmxMonitor (const std::string &name,
                          const std::string &runcontrol) :
   eudaq::Monitor (name, runcontrol)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific method called upon initialization
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoInitialise ()
{
  auto ini = GetInitConfiguration ();
  ini->Print(std::cout);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific method called upon configuration
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoConfigure ()
{
  auto conf = GetConfiguration();
  conf->Print(std::cout);
  m_en_print         = conf->Get ("LDMX_ENABLE_PRINT",         1);
  m_en_std_converter = conf->Get ("LDMX_ENABLE_STD_CONVERTER", 0);
  m_en_std_print     = conf->Get ("LDMX_ENABLE_STD_PRINT",     0);

  return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  The LDMX specific method called upon start run
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoStartRun ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific method called upon stop run
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoStopRun ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  The LDMX specific method called upon reset
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoReset ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific method called upon termination
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoTerminate ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  The LDMX specific method to receive and do the actual monitoring
          of the received event.

  \param[in] ev  The event
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxMonitor::DoReceive(eudaq::EventSP ev)
{
  if (m_en_print)
  {
    ev->Print(std::cout);
  }

  
  if (m_en_std_converter)
  {
     auto stdev = std::dynamic_pointer_cast<eudaq::StandardEvent>(ev);
     if(!stdev)
     {
        stdev = eudaq::StandardEvent::MakeShared();
        eudaq::StdEventConverter::Convert(ev, stdev, nullptr); //no conf
        
        if(m_en_std_print)
        {
           stdev->Print(std::cout);
        }
     }
  }

  return;
}
/* ---------------------------------------------------------------------- */
/* END IMPLEMENTATION: LdmxMonitor                                        */
/* ====================================================================== */
