// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Contribution.cc
  \brief  The base class for all contribution types to an LDMX Event 
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
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
 * 2019.01.08 jjr Moved into ldmx/builder/client namespace
 * 2018.02.11 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Contribution.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Diagnostic routine to print select fields of the contribution 
         header

  \param[in] ctb The contribution
                                                                          */
/* ---------------------------------------------------------------------- */
void Contribution::print () const
{
   printf ("Ctb[%2d:%5.5" PRIx32 "]: %8" PRIx32 " %6lu.%09lu\n",
           m_id, m_sequence, m_nbytes,
           m_rcvTime / ldmx::utl::Timestamp::NanoSecondsPerSecond,
           m_rcvTime % ldmx::utl::Timestamp::NanoSecondsPerSecond);

   return;
}
/* ====================================================================== */

