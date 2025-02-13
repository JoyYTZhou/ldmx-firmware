// -*-Mode: C++;-*-

#ifndef _LDMX_BUILDER_CLIENT_CONFIGURATION_HH_
#define _LDMX_BUILDER_CLIENT_CONFIGURATION_HH_

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Configuration.hh
  \brief  LDMX event builder configuration
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
 * 2019.03.29 jjr Added number of trigger events per batch
 * 2019.03.18 jjr Added number of RSSI frame and SVT events/batch
 * 2019.01.08 jjr Moved into hps::builder::client namespace
 * 2019.01.08 jjr Moved into hps::eb::client namespace
 * 2018.12.20 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/CfgContributor.hh"
#include "ldmx/builder/Logging.hh"
#include <cinttypes>
#include <vector>
#include <string>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

// <none>

/* ====================================================================== */




/* ====================================================================== */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- */
class Configuration
{
public:
   Configuration ();
   Configuration (int type, int nfragments);

public:
   void addContributor (char const                        *name, 
                        int                                 len,  
                        uint16_t                           port,
                        int                             timeout,
                        int                         nrssiFrames,
                        int                             nevents);
   
public:
   void print () const;
   void print (std::string const &name) const;

public:
   std::string                                                m_name;
   int                                                        m_type;
   int                                                  m_nfragments;
   std::vector<ldmx::builder::client::CfgContributor> m_contributors;
   int                                           m_connectionTimeout;
   std::string                                         m_triggerPipe;

   unsigned short int                           m_ntrgEventsPerBatch;
   unsigned short int                                  m_ntrgBatches;
   unsigned short int                                   m_ntrgEvents;

   unsigned short int                           m_nsvtEventsPerBatch;
   unsigned short int                                  m_nsvtBatches;
   unsigned short int                                   m_nsvtEvents;
   
   unsigned short int                                  m_nrssiFrames;
   
   ldmx::builder::Logging::Level                             m_level;

};
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */


#endif 

