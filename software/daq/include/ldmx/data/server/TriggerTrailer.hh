// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_TRIGGERTRAILER_HH__
#define __LDMX_DATA_SERVER_TRIGGERTRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TriggerTrailer.hh
  \brief  Defines filling the trigger trailer structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

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
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.03.29 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/TriggerTrailer.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Fill the trigger trailer
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerTrailer : public ldmx::data::common::TriggerTrailer
{
public:
   TriggerTrailer ();
   TriggerTrailer (uint32_t w32);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Trigger trailer

  \note
   There is one trigger trailer for each batch of trigger events.
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerTrailer::TriggerTrailer ()
{
   m_w32 = ldmx::data::common::TriggerTrailer::W32;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Trigger trailer

  \note
   There is one trigger trailer for each batch of trigger events.

  \param[in] w32  The trigger trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerTrailer::TriggerTrailer (uint32_t w32)
{
   m_w32 = w32;
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
