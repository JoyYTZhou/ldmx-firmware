// -*-Mode: C++;-*-

#ifndef _LDMX_UTL_LIST_INTERLOCKED_HH_
#define _LDMX_UTL_LIST_INTERLOCKED_HH_


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/ListInterlocked.hh
  \brief  Manages a interlocked singularly linked list
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
 * 2019.01.17 jjr Cloned from EXO versions, converted to C++
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/List.hh"
#include "ldmx/utl/ResourceWait.hh"


struct timespec;

/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!
  
  \class   ListInterlocked
  \brief   Class for thread safe singularly linked list
                                                                          */
/* ---------------------------------------------------------------------- */
class ListInterlocked : public List
{
public:
   ListInterlocked ();
  ~ListInterlocked ();

public:
   void       append (List *src);
   void       append (ListInterlocked  *src);
   int       disable ();
   int        enable ();
   int     isEnabled () const;
   ListNode  *insert (ListNode        *node);
   ListNode     *jam (ListNode        *node);
   void      prepend (List             *src);    
   void      prepend (ListInterlocked  *src);    
   ListNode  *remove ();
   ListNode *removeW ();
   ListNode *removeW (struct timespec const *to);
   ListNode *removeW (struct timespec const &to);

public:
   ResourceWait  m_rw;  /*!< The Resource Wait control block              */
};
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

#endif






