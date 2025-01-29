// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_LISTSINTERLOCKED_HH__
#define __LDMX_UTL_LISTSINTERLOCKED_HH__


/* ---------------------------------------------------------------------- *//*!
   
  \file   ldmx/utl/ListsInterlocked.hh
  \brief  Managed a set of interlocked lists
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
                                                                          */
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.01.21 jjr Cloned from EXO versions of PL.h, converted to C++
 *
\* ---------------------------------------------------------------------- */


#include "List.hh"
#include <cinttypes>

struct timespec;

    
/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \class ListsInterlocked
  \brief Manages a set of interlocked lists
                                                                          */
/* ---------------------------------------------------------------------- */
class ListsInterlocked
{
public:
   class                          Lcb;
   class                         List;
   typedef ListNode              Node;
   typedef uint32_t               Set;

public:
   ListsInterlocked () { return; }
   ListsInterlocked (ListsInterlocked::Lcb *lcb, int nlists);
   ListsInterlocked (int nlists);
  ~ListsInterlocked ();


   static const Set ListsInterlockedSetAll = (0xffffffff);

public:
   static void  setUser (ListsInterlocked::List *list, 
                         void                    *ctx);
   static void *getUser (ListsInterlocked::List *list);
   void        *getUser (int                  list_id);


   static Set listMask (int id)          { return               (1 << id); }
   static Set listSet  (int nlists)      { return (listMask (nlists) - 1); }
   static Set listSet  (int    beg, 
                        int nlists)      { return listSet (nlists) << beg; }
   static int     find (Set set)         { return (  __builtin_clz (set)); }
   static Set      add (Set set, int id) { return ( set |  listMask (id)); }
   static Set   remove (Set set, int id) { return ( set & ~listMask (id)); } 


   static int sizeofLcb (int nLists);

public:
   ListsInterlocked::Set       wake (ListsInterlocked::Set wake_set);
   ListsInterlocked::List  *getList (int list_id);

   //  Add a node to the specified list
   Node *insert  (List            *list, 
                  Node            *node);
   Node *jam     (List            *list,
                  Node            *node);
   
   // Remove a node from the highest active priority list
   Node *remove  (volatile const Set *wait_set,
                  int                *list_id);

   Node *removeW (volatile const Set *wait_set,
                  int                 *list_id);

   Node *removeW (volatile const Set *wait_set,
                  int                 *list_id,
                  struct timespec const   *to);

   Node *removeW (volatile const Set *wait_set,
                  int                 *list_id,
                  struct timespec const   &to);


   //  Append 'list' to 'list_id'
   void append   (int           list_id, 
                  ldmx::utl::List *list);


   // Append all or the specified set of active sets to non-interlocked lists
   Set append (ldmx::utl::List          *lists);
   Set append (ldmx::utl::List          *lists,
               Set                         any);

   Set appendW (ldmx::utl::List         *lists);
   Set appendW (ldmx::utl::List          *lists,
                Set                        set); 
   Set appendW (ldmx::utl::List         *lists,
                Set                        any,
                Set                        all); 


   Set appendW (ldmx::utl::List         *lists,
                struct timespec const      *to);
   Set appendW (ldmx::utl::List         *lists,
                Set                        any, 
                struct timespec const      *to);
   Set appendW (ldmx::utl::List         *lists,
                Set                        any, 
                Set                        all, 
                struct timespec const      *to);


    
   // Return the set of active lists
   Set  get         ();
   Set  getW        (volatile const Set *wait_set);
   Set  getW        (volatile const Set *wait_set,
                     struct timespec const    *to);

private:
   ListsInterlocked::Lcb *m_lcb;
};
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */


#endif
