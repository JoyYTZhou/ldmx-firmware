// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/SvtTrailer.cc
  \brief  Accesses the SVT event trailer structure.
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
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/client/SvtTrailer.hh"
#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the SVT event trailer to the console

  \param[in] tlr  The trailer to print
                                                                          */
/* ---------------------------------------------------------------------- */
void SvtTrailer::print (SvtTrailer const *tlr)
{
   uint16_t multiSampleCount = tlr->getMultiSampleCount   ();
   uint16_t        skipCount = tlr->getSkipCount          ();
   uint8_t        undefined0 = tlr->getUndefined0         ();
   bool         apvSyncError = tlr->getApvSyncError       ();
   bool   apvFifoBackupError = tlr->getApvFifoBackupError ();
   uint8_t        undefined1 = tlr->getUndefined1         ();
   
   printf ("Svt Trailer: "
           "nms: %4" PRIx16 " " 
           "nskip: %4" PRIx16 " "
           "und0: %1" PRIx8 " "
           "synErr: %1d "
           "fifoErr: %1d"
           "und0: %1" PRIx8 "\n",
           multiSampleCount, skipCount,          undefined0,
           apvSyncError,     apvFifoBackupError, undefined1);

   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */
