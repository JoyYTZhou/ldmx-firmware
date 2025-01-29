// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_DUMPER_HH__
#define __LDMX_UTL_DUMPER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Dumper.hh
  \brief  Basic methods to create dumps (usually hex)
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
 * ---------- --- ---------------------------------------------------------
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.03.28 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <fstream>


/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

 \class Dumper
 \brief Creates dumps (generally hex dumps to the console) for debugging
        purposes
                                                                          */
/* ---------------------------------------------------------------------- */
class Dumper
{
public:
   Dumper ();
   Dumper (std::ostream  &stream);
   Dumper (const char  *filename);


public:
   void dump (const char *label, uint8_t  const  *p8, size_t  n8, bool last = true);
   void dump (const char *label, uint16_t const *p16, size_t n16, bool last = true);
   void dump (const char *label, uint32_t const *p32, size_t n32, bool last = true);
   void dump (const char *label, uint64_t const *p64, size_t n64, bool last = true);

public:
   std::ostream &m_stream;   /*!< The output file, defaults to the stdout */
   int           m_curIdx;   /*!< The current index                       */
};
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

#endif
