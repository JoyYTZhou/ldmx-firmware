// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_BFINSERT_HH__
#define __LDMX_UTL_BFINSERT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/BfInsert.hh
  \brief  Defines routines to insert bit fields into integers
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
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.02.20 jjr Created
 *
\* ---------------------------------------------------------------------- */





// W = Output Word type (uint8_t, uint16_t, etc)
// V = Value type (uint8_t, uint16_t, etc)
// 0 = Offset enum class name
// M = Mask   enum class name
//  


namespace ldmx {
namespace utl  {
namespace bf   {


template<typename W, typename V, typename M, typename O>
static W add (V value, M mask, O offset)
{
   W wrd = (value & static_cast<W>(mask)) << static_cast<int>(offset);
   return wrd;
}

}
}
}



#if 0

define BF_INSERT(_type, _wrd, _val, _mask, _offset)                 \
   (((_val) >> static_cast<int>(_offset)) & static_cast<_type>(_mask))


#define BF_INSERT64(_wrd, _val, _mask, _offset)           \
        BF_INSERT(_uint64_t, _val, _mask, _offset)
   
#define BF_INSERT32(_val, _mask, _offset)                              \
        BF_INSERT32(_uint32_t, _val, _mask, _offset)

#endif

#endif
