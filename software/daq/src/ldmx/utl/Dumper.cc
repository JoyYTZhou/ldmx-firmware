// -*-Mode: C++;-*-

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


#include "ldmx/utl/Dumper.hh"
#include <iostream>
#include <iomanip>


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {

/* ---------------------------------------------------------------------- *//*!

  \brief Constructs a dumper directed at the console
                                                                          */
/* ---------------------------------------------------------------------- */
Dumper::Dumper () :
   m_stream (std::cout),
   m_curIdx         (0)
{
   m_stream << std::setbase (16);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructs a dumper directed at the specified file stream
                                                                          */
/* ---------------------------------------------------------------------- */
Dumper::Dumper (std::ostream &stream) :
   m_stream (stream),
   m_curIdx      (0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructs a dumper directed at the file
                                                                          */
/* ---------------------------------------------------------------------- */
Dumper::Dumper (const char *filename) :
   m_stream (*new std::ofstream (filename)),
   m_curIdx (0)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Method for a simple hex dump of 32-bit values

   \param[in] label  A string used to label the output
   \param[in]   p32  Pointer to the array of 32-bit values
   \param[in]   n32  The number of 32-bit values to dump
   \param[in]  last  Flag indicating that this it the last of this dump
                                                                          */
/* ---------------------------------------------------------------------- */
void Dumper::dump (const char   *label, 
                   uint32_t const *p32,
                   size_t          n32, 
                   bool           last)
{
   int beg = m_curIdx;
   int end = m_curIdx + n32;
   for (int idy = beg; idy < end; idy++)
   {
      if ( (idy % 8) == 0)  m_stream << label << "[" << std::setfill (' ') 
                                     << std::setw(3) << idy << "] ";
      m_stream << ' ' << std::setw(8) << std::setfill ('0') << p32[idy];
      if ( (idy % 8) == 7)  m_stream << std::endl;
   }


   if (last)
   {
      if (n32 % 8) m_stream << std::endl;
      m_curIdx = 0;
   }
   else
   {
      m_curIdx = n32;
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Method for a simple hex dump of 64-bit values

   \param[in] label  A string used to label the output
   \param[in]   p64  Pointer to the array of 64-bit values
   \param[in]   n64  The number of 32-bit values to dump
   \param[in]  last  Flag indicating that this it the last of this dump
                                                                          */
/* ---------------------------------------------------------------------- */
void Dumper::dump (const char   *label, 
                   uint64_t const *p64,
                   size_t          n64, 
                   bool           last)
{
   int beg = m_curIdx;
   int end = m_curIdx + n64;
   for (int idy = beg; idy < end; idy++)
   {
      if ( (idy % 4) == 0) m_stream << label << "[" << std::setfill (' ') 
                                             << std::setw(3) << idy << "] ";
      m_stream << ' ' << std::setw(16) << std::setfill ('0') << p64[idy];
      if ( (idy % 4) == 3)  m_stream << std::endl;
   }


   if (last)
   {
      if (n64 % 4) m_stream << std::endl;
      m_curIdx = 0;
   }
   else
   {
      m_curIdx = n64;
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

