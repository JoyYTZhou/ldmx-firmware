// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/rc/LdmxTgDataCollector.cc
  \brief  LDMX specific the data collection class
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
 * 2021.04.12 jjr Cloned from eudaq::Ex0TgDataCollector.cc
 *
\* ---------------------------------------------------------------------- */



#include "eudaq/DataCollector.hh"


#include <mutex>
#include <deque>
#include   <map>
#include   <set>

/* ====================================================================== */
/* DEFINITION: LdmxTgDataCollector                                        */
/* ---------------------------------------------------------------------- */
class LdmxTgDataCollector:public eudaq::DataCollector
{
public:
   LdmxTgDataCollector(const std::string &name,
                       const std::string   &rc);
   
   void DoConnect    (eudaq::ConnectionSPC id) override;
   void DoDisconnect (eudaq::ConnectionSPC id) override;
   void DoConfigure  ()                        override;
   void DoReset      ()                        override;
   void DoReceive    (eudaq::ConnectionSPC id,
                      eudaq::EventSP       ev) override;

   static const uint32_t m_id_factory = eudaq::cstr2hash ("LdmxTgDataCollector");
   
private:
   std::mutex                                  m_mtx_map;
   std::map<eudaq::ConnectionSPC,
            std::deque<eudaq::EventSPC>>    m_conn_evque;
   std::set<eudaq::ConnectionSPC>        m_conn_inactive;
   uint32_t                                    m_noprint;
};
/* ---------------------------------------------------------------------- */
/* END DEFINITION: LdmxTgDataCollector                                    */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION: LdmxTgDataCollector                                    */
/* ---------------------------------------------------------------------- *//*!

  \brief Register LdmxTgDataCollector
                                                                          */
/* ---------------------------------------------------------------------- */
namespace
{
  auto dummy0 = eudaq::Factory<eudaq::DataCollector>::
                        Register<LdmxTgDataCollector,
                                 const std::string&,
                                 const std::string&>
                                (LdmxTgDataCollector::m_id_factory);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The LDMX specific Data Collector constructor

  \param[in] name  The name of this specific data collector
  \param[in]   rc  The name of run controller
                                                                          */ 
/* ---------------------------------------------------------------------- */
LdmxTgDataCollector::LdmxTgDataCollector(const std::string &name,
			  	         const std::string   &rc) :
   DataCollector (name, rc)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  LDMX data collector connection method
  
  \brief  idx Index of the data producer (???)
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxTgDataCollector::DoConnect (eudaq::ConnectionSPC idx)
{
   std::cout << "LdmxTgDataCollector idx = " << idx << std::endl;
   std::unique_lock<std::mutex> lk (m_mtx_map);
   m_conn_evque[idx].clear ();
   m_conn_inactive.erase   (idx);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  LDMX method to disconnect a data producer

    \brief idx Index of the data producer (???)
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxTgDataCollector::DoDisconnect (eudaq::ConnectionSPC idx)
{
   std::unique_lock<std::mutex> lk(m_mtx_map);
   m_conn_inactive.insert (idx);
   if(m_conn_inactive.size () == m_conn_evque.size ())
   {
      m_conn_inactive.clear ();
      m_conn_evque.clear    ();
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void LdmxTgDataCollector::DoConfigure ()
{
   m_noprint = 0;
   auto conf = GetConfiguration ();
   if (conf)
   {
      //conf->Print();
      m_noprint = conf->Get("EX0_DISABLE_PRINT", 0);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void LdmxTgDataCollector::DoReset ()
{
   std::unique_lock<std::mutex> lk(m_mtx_map);
   m_noprint = 0;
   m_conn_evque.clear    ();
   m_conn_inactive.clear ();

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief LDMX method to be called when receiving an event

  \param[in]  idx  The index of the data producer
  \param[in] evsp  The event to receive
                                                                          */
/* ---------------------------------------------------------------------- */
void LdmxTgDataCollector::DoReceive (eudaq::ConnectionSPC idx,
                                     eudaq::EventSP      evsp)
{
   std::unique_lock<std::mutex> lk (m_mtx_map);
   
   if (!evsp->IsFlagTrigger ())
   {
      EUDAQ_THROW("!evsp->IsFlagTrigger()");
   }
   m_conn_evque[idx].push_back (evsp);

   uint32_t trigger_n = -1;
   for(auto &conn_evque: m_conn_evque)
   {
      if (conn_evque.second.empty ())
      {
         return;
      }
      else
      {
         uint32_t trigger_n_ev = conn_evque.second.front ()->GetTriggerN ();
         if (trigger_n_ev< trigger_n)
         {
            trigger_n = trigger_n_ev;
         }
      }
   }

   auto ev_sync = eudaq::Event::MakeUnique ("LdmxTg");
   ev_sync->SetFlagPacket ();
   ev_sync->SetTriggerN   (trigger_n);
   for (auto &conn_evque: m_conn_evque)
   {
      auto &ev_front = conn_evque.second.front(); 
      if (ev_front->GetTriggerN() == trigger_n)
      {
         ev_sync->AddSubEvent (ev_front);
         conn_evque.second.pop_front ();
      }
   }
  
   if (!m_conn_inactive.empty ())
   {
      std::set<eudaq::ConnectionSPC> conn_inactive_empty;
      for (auto &conn: m_conn_inactive)
      {
         if(m_conn_evque.find (conn) != m_conn_evque.end() && 
            m_conn_evque[conn].empty())
         {
            m_conn_evque.erase         (conn);
            conn_inactive_empty.insert (conn);	
         }
      }
      
      for (auto &conn: conn_inactive_empty)
      {
         m_conn_inactive.erase (conn);
      }
   }
   
   if (!m_noprint)
   {
      ev_sync->Print(std::cout);
   }
   
   WriteEvent (std::move (ev_sync));

   return;
}
/* ---------------------------------------------------------------------- */
