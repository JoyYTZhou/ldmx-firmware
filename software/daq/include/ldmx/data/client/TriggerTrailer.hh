// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_TRIGGERTRAILER_HH__
#define __LDMX_DATA_CLIENT_TRIGGERTRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/TriggerTrailer.hh
  \brief  Defines accessing the Trigger trailer structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This various member fields of the Trigger trailer structure.

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
 * 2019.02.27 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/TriggerTrailer.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the Trigger trailer
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerTrailer : public ldmx::data::common::TriggerTrailer
{
public:
   TriggerTrailer () { return; }

public:
   uint32_t getW32 () const;
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the trigger trailer word
  \return  The 32-bit value of the trigger trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerTrailer::getW32 () const
{
   return m_w32;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
