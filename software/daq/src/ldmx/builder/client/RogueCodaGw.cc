// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RogueCodeGw.cc
  \brief  Gateway to the LDMX event/fragment builder
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
 * 2019.03.29 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/RogueCodaGw.hh"
#include "Gateway.hh"
#include <cinttypes>



/* ====================================================================== */
/* IMPLEMENTATION:RogueCodaGw                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the LDMX Rogue/Code gateway
                                                                          */
/* ---------------------------------------------------------------------- */
RogueCodaGw::RogueCodaGw () :
   m_gw (new Gateway)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the LDMX Rogue/Code gateway
                                                                          */
/* ---------------------------------------------------------------------- */
RogueCodaGw::~RogueCodaGw ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::download (char const *cfgFile, char const *usrString)
{
   int32_t status = m_gw->download (cfgFile, usrString);
   return  status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  This is the configuration callback
  \retval = true,  successful configuration
  \retval = false, failed configuration

  \param[in] confFile  Name of the file containing the configuation
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::download (ldmx::builder::client::Configuration const &cfg)
{
   bool    status = m_gw->download (cfg);
   return  status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Prestart control function
  \retval = true,  successful prestart
  \retval = false, failed prestart
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::prestart()
{
   bool   status = m_gw->prestart ();
   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Go control function
  \retval = true,  successful go 
  \retval = false, failed go
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::go()
{
   int32_t status = m_gw->go ();
   return  status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Callback when trigger is received
  \return  The event count

  \param[in]  trigData   The trigger data
  \paramiin]  trigLength The length of the trigger data, in units of 32-bits
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::trigger(uint32_t *trigData, uint32_t trigLength)
{
   int32_t eventCount = m_gw->trigger (trigData, trigLength);
   return  eventCount;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fill in the event
  \return The length, in 32-bit words, of the event

  \param[out] eventBuffer  Pointer to the event buffer to fill in
  \param[ in]       index  ???
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::event(uint32_t *eventBuffer, uint32_t index)
{
   int    n32 = m_gw->event (eventBuffer, index);
   return n32;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Fill in the sync event
  \retval |= 0x01, if config should be added
  \retval |= 0x10, if calibrationrun is ended

  \param[in] eventBuffer  Pointer to the event buffer to fill in
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::update (uint32_t syncFlag)
{
   int    n32 = m_gw->update (syncFlag);
   return n32;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Copy the configuration into the event buffer
  \return The number of 32 bit words copied into the buffer

  \param[in] eventBuffer The destintination buffer
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::config (uint32_t *eventBuffer)
{
   int    n32 = m_gw->config (eventBuffer);
   return n32;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  End control function
  \retval = true,  successful configuration
  \retval = false, failed configuration
                                                                          */
/* ---------------------------------------------------------------------- */
int32_t RogueCodaGw::end ()
{
   int32_t status = m_gw->end ();
   return  status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Close control function
  \retval = true,  successful configuration
  \retval = false, failed configuration
                                                                          */
/* ---------------------------------------------------------------------- */
void RogueCodaGw::close ()
{
   m_gw->close ();
   return;
}
/* ====================================================================== */
