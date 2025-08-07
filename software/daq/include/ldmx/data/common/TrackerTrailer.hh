// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_TRACKERTRAILER_HH__
#define __LDMX_DATA_COMMON_TRACKERTRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/TrackerTrailer.hh
  \brief  Defines the layout of the tracker trailer structure.
  \author Sophie Middleton

  \par
   This file defines the only the data layout, not how to write or
   access this data and is meant to be the base class for derived
   classes that do the accessing and writing.  No methods are defined,
   so that they are POD (plain old data), free of any C++ decorations.

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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.02.20 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
namespace ldmx       {
namespace data       {
namespace common     {
/* ---------------------------------------------------------------------- *//*!

  \brief Defines the bit fields of the 64-bit word that comprises the
         Tracker Trailer
                                                                          */
/* ---------------------------------------------------------------------- */
struct TrackerTrailer
{
public:
   static const uint32_t W32 = 0x880000a3;
public:
   uint32_t  m_w32;  /*!< The 32-bit word containing all the bit fields   */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
