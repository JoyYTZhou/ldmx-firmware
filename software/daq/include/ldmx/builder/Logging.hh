// -*-Mode: C++;-*-

#ifndef _LDMX_BUILDER_LOGGING_HH_
#define _LDMX_BUILDER_LOGGING_HH_

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/Logging.hh
  \brief  The LDMX Event Builder message logging control
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
 * 2019.01.08 jjr Moved into client namespace
 * 2018.12.21 jjr Changed from RSSI specific to a generic level
 * 2018.12.20 jjr Created
 *
\* ---------------------------------------------------------------------- */



/* ====================================================================== */
namespace ldmx    {
namespace builder {
/* ---------------------------------------------------------------------- */
class Logging
{
public:
   enum class Level : unsigned char
   {
      None     = 0,
      Debug    = 1,
      Info     = 2,   
      Warning  = 3,
      Error    = 4,
      Critical = 5,
      Max      = 5
   };

public:
   static int setLevel (Logging::Level level);

};
/* ---------------------------------------------------------------------- */
}  /* NAMESPACE: builder                                                  */
}  /* NAMESPACE: ldmx                                                     */
/* ====================================================================== */

#endif
