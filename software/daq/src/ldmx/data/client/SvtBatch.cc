// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/SvtBatch.cc
  \brief  Accesses 1 batch of SVT events.
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
 * 2019.03.02 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/client/SvtBatch.hh"
#include "ldmx/data/client/SvtBatchHeader.hh"
#include "ldmx/data/client/SvtEvent.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!
  \brief Prints the SVT batch of events to the console

  \param[in] batch The SVT batch of events to print
                                                                          */
/* ---------------------------------------------------------------------- */
void SvtBatch::print (SvtBatch const *batch)
{
   SvtBatchHeader::print (batch->getHeader ());

   auto rbeg = batch->rbegin ();
   auto rend = batch->rend   ();

   // ----------------
   // Print the events
   // ----------------
   for (auto event = rbeg;  event != rend; event++)
   {
      putchar ('\n');
      event->print ();

      SvtTrailer   const *trailer = event->getTrailer ();
      SvtBatchTail const    *tail = reinterpret_cast<decltype(tail)>
                                                         (trailer+1);

      tail->print  ();
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

