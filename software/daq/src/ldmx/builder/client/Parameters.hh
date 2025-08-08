// -*-Mode: C++;-*-

#ifndef _LDMX_BUILDER_CLIENT_PARAMETERS_HH_
#define _LDMX_BUILDER_CLIENT_PARAMETERS_HH_

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Parameters.hh
  \brief  Class to contain the command line parameters
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to
   the license terms in the LICENSE.txt file found in the top-level directory
   of this distribution and at:

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html.
   \endverbatim

   No part of the rogue software platform, including this file, may be
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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.03.29 jjr Added number of trigger events/batch
 * 2019.03.18 jjr Added number of RSSI frame and SVT events/batch
 * 2019.02.12 jjr Moved to namespace ldmx::builder::client
 * 2018.12.21 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/Logging.hh"
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace builder {
namespace client  {
/* ---------------------------------------------------------------------- */

class Configuration;

}  /* NAMESPACE: client                                                   */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */



/* ---------------------------------------------------------------------- *//*!

  \class Parameters
  \brief Captures the command line parameters in a digestable form

\* ---------------------------------------------------------------------- */
class Parameters
{
public:
   Parameters (int argc, char *const argv[]);

public:
   enum class ConnectionType
   {
      None  = 0,
      TcpIp = 1,
      Rssi  = 2
   };

public:
   void report ();
   int configure      (ldmx::builder::client::Configuration *cfg) const;
   int translate_srcs (ldmx::builder::client::Configuration *cfg) const;

public:
   ConnectionType            m_type; /*!< Connection type                 */
   char const        *m_triggerPipe; /*! Name of pipe for the trigger     */
   char const        *m_trackerPipe; /*! Name of pipe for the trigger     */
   char const               *m_srcs; /*!< IP addresses, comma separated
                                                                   list   */
   uint16_t                  m_port; /*!< IP port                         */
   int32_t           m_toConnection; /*!< Connection timeout, in seconds  */
   ldmx::builder::Logging::Level
                            m_level; /*!< The logging level               */

   /* ------------------------------------------------------------------- */
   /* -- Output buffering                                                 */
   /* ------------------------------------------------------------------- */
   unsigned short int  m_nfragments; /*!< Number of queued fragments      */
   unsigned short int       m_orsvd; /*!< Reserved for future use         */
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- */
   /* -- Input buffering                                                  */
   /* ------------------------------------------------------------------- */
   unsigned short int m_nrssiFrames; /*!< Number RSSI frames to maintain  */
   unsigned short int       m_irsvd; /*!< Reserved for future use         */
   unsigned short int m_ntrgBatches; /*!< Number trigger batches          */
   unsigned short int
               m_ntrgEventsPerBatch; /*!< Number of trigger events/batch  */
   unsigned short int m_ntrkBatches; /*!< Number tracker batches          */
   unsigned short int
               m_ntrkEventsPerBatch; /*!< Number of tracker events/batch  */
   unsigned short int  m_nsvtEvents; /*!< Number SVT     events to buffer */
   /* ------------------------------------------------------------------- */

};
/* ---------------------------------------------------------------------- */

#endif
