// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_BFEXTRACT_HH__
#define __LDMX_UTL_BFEXTRACT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/BfExtract.hh
  \brief  Defines routines to extract bit fields from 32 and 64-bit words
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
 * 2019.02.20 jjr Created
 *
\* ---------------------------------------------------------------------- */



namespace ldmx {
namespace utl  {
namespace bf   {


template<typename W, typename V, typename M, typename O>
static V extract (W word, M mask, O offset)
{
   V      field = (word >> static_cast<int>(offset)) & static_cast<W>(mask);
   return field;
}

}
}
}

#endif
