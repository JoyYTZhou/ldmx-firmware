// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

   \file   ldmx/utl/ListInterlocked.cc
   \brief  Interlocked singly linked list, inline versions.
   \author JJRussell - russell@slac.stanford.edu

  \par SYNOPSIS
   This defines the inline interlocked versions of the singly linked
   list routines. These routines are thread-safe

   See also the non-inlined versions of these routines which offer
   possible savings in code space and increased modularity at the expense
   of some performance. The L routines provide the same functionality
   without the overhead of being interlocked.

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
 * 2019.01.17 jjr Cloned from EXO versions, converted to C++
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/utl/ListInterlocked.hh"

/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \fn          ListInterlocked ()
  \brief       Initializes the list head of a singly linked list to 
               an empty list.

  The list is initialized to an empty list. This must be done before
  performing any other operations on the list. The \a type determines how
  the list behaves when one attempts to remove a node from an empty list.
                                                                          */
/* ---------------------------------------------------------------------- */
ListInterlocked::ListInterlocked () :
   List (),
   m_rw ()
{
   return;
}   
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn         ~ListInterlocked ()
  \brief       Interlocked list destructor


   This routine releases the resources acquired when the list was created.
                                                                          */
/* ---------------------------------------------------------------------- */
ListInterlocked::~ListInterlocked ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn           void append (List *src)
  \brief        Appends the \a src list to this list

  \param    src A previously initialized list acting as the source

   Appends the source list to the destination list. After this operation
   the destination list will have consist of its original members followed
   by the members on the source list. The source list will be empty.
                                                                          */
/* ---------------------------------------------------------------------- */
void ListInterlocked::append (List *src)
{
   // Lock the list
   m_rw.lock       ();


   // Appedn the source list to this list
   List::append (src);


   // Wake-up any task/thread waiting for a node on this list
   m_rw.wake      ();


   // Unlock the list
   m_rw.unlock     ();

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           void append (ListInterlocked *src)
  \brief        Appends the \a src list to this list

  \param    src A previously initialized list acting as the source

   Appends the source list to the destination list. After this operation
   the destination list will have consist of its original members followed
   by the members on the source list. The source list will be empty.
                                                                          */
/* ---------------------------------------------------------------------- */
void ListInterlocked::append (ListInterlocked *src)
{
   // Lock the lists
   m_rw.lock       ();
   src->m_rw.lock  ();


   // Append the source list to this list
   List::append (src);

   
   // Wake-up any task/thread waiting for a node on this list
   m_rw.wake      ();


   // Unlock the lists
   src->m_rw.unlock ();
   m_rw.unlock      ();

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           int disable ()
  \brief        Disables the dequeing of nodes from this list

  \retval         0 = List was previously disabled
  \retval       !=0 = List was previously enabled

   Any items on the que and any items placed on the que after the disable
   call will not be dequed by any of the removeXXX routines. Delivery
   of these items may resume after a call to \e enable().

  \warning
   Be cautious with this routine. It is very easy to deadlock the system
   by having the same task disable the list and place items on the list.
   Sooner or later in this situation the source of items to be placed on
   the list will become exhausted. If the user is getting these items from
   a resource that blocks when it the pool of items becomes exhausted,
   no call to issue a enable can be made, \e i.e. deadlock.

   To avoid this situation, either separate the task controlling the list
   via disable's and enables' from the task allocating new items
   for the list or, if in the same task, cease to allocate new items when
   the list has been disabled.
                                                                          */
/* ---------------------------------------------------------------------- */
int ListInterlocked::disable ()
{
   return m_rw.disable ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           int enable ()
  \brief        Enable the dequeing of nodes from this list

  \retval         0 = List was previously disabled
  \retval       !=0 = List was previously  enabled

   After issuing this call the remove routines will deliver items
   once again. If any remove call is currently waiting on a non-empty
   list, a wakeup call is issued, permitting nodes to be dequed.
                                                                          */
/* ---------------------------------------------------------------------- */
int ListInterlocked::enable ()
{
   return m_rw.enable ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           int iseEnabled () const
  \brief        Returns the state of the enabled flag for this list

  \retval      0 List is currently disabled
  \retval    !=0 List is currently  enabled

  \warning
   As with all queries of this type, the answer is only good until it
   changes. The user is cautioned on its usage. It is primarily provided
   as a debugging tool.
                                                                          */
/* ---------------------------------------------------------------------- */
int ListInterlocked::isEnabled () const
{
   return m_rw.isEnabled ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           ListNode *ListInsert (ListNode *node)
  \brief        Adds a node to the tail of a previously initialized list.

  \return       Pointer to the old backward link. This can be used to test
                whether this was the first item on the list. \n
                If return_value == list, the empty)


  \param  node  The node to add at the tail of the list.

   Adds the specified node to the tail of the list. If all nodes are added
   with the insert routine, the list behaves as a FIFO.
                                                                          */
/* ---------------------------------------------------------------------- */
ListNode *ListInterlocked::insert (ListNode *node)
{
   // Lock the list
   m_rw.lock ();


   // Insert the new element on the list
   ListNode *blnk = reinterpret_cast<ListNode *>(List::insert (node));


   // Wake any potential waiting task
   m_rw.wake ();


   // Unlock the list
   m_rw.unlock ();


   // Return the previous oldest element
   return blnk;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \fn           ListNode *jam (ListNode *node)
   \brief        Adds a node to the head of a previously initialized list.

   \return       Pointer to the old forward link. This can be used to test
                 whether this was the first item on the list. \n
                 If return_value == list, the empty)

   \param  node  The node to add.

    Adds the specified node to the head of the list. If all nodes are added
    with the jam routine, the list behaves as a LIFO.
                                                                          */
/* ---------------------------------------------------------------------- */
ListNode *ListInterlocked::jam (ListNode *node)
{
   // Lock the list
   m_rw.lock ();


   // Jam the new node on the list
   ListNode *flnk = reinterpret_cast<ListNode *>(List::jam (node));


   // Wake-up any task/thread waiting for a node on this list
   m_rw.wake ();


   // Unlock the list
   m_rw.unlock ();


   return flnk;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           void prepend (List *src)
  \brief        Prepends the \a src list members to this ist

  \param    src A previously initialized list acting as the source

   Prepends the source list to the destination list. After this operation
   the destination list will have consist of its original members preceded
   by the members on the source list. The source list will be empty.
                                                                          */
/* ---------------------------------------------------------------------- */
void ListInterlocked::prepend (List *src)
{
   // Lock the list
   m_rw.lock     ();


   // Prepend the source list
   List::prepend (src);


   // Wake-up any task/thread waiting for a node on this list
   m_rw.wake     ();


   // Unlock the list
   m_rw.unlock   ();

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn           void prepend (ListInterlocked *src)
  \brief        Prepends the \a src list members to this ist

  \param    src A previously initialized list acting as the source

   Prepends the source list to the destination list. After this operation
   the destination list will have consist of its original members preceded
   by the members on the source list. The source list will be empty.
                                                                          */
/* ---------------------------------------------------------------------- */
void ListInterlocked::prepend (ListInterlocked *src)
{
   // Lock the lists
   m_rw.lock     ();
   src->m_rw.lock();


   // Prepend the source list
   List::prepend (src);


   // Wake-up any task/thread waiting for a node on this list
   m_rw.wake     ();


   // Unlock the lists
   src->m_rw.unlock ();
   m_rw.unlock      ();

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn          ListNode *remove ()
  \brief       Removes the node from the tail of a previously initialized
               list. An empty list returns NULL as its node.

  \return      A pointer to the removed node of NULL if the list is empty.

   Removes the node at the head of the list. If the list is empty, NULL is
   returned.
                                                                          */
/* ---------------------------------------------------------------------- */
ListNode *ListInterlocked::remove ()
{
   // Lock the list
   m_rw.lock ();


   // Remove the node iff the list is enabled 
   ListNode *node = m_rw.isEnabled ()
                  ? reinterpret_cast<ListNode *>(List::remove ())
                  : NULL;

   // Unlock the list
   m_rw.unlock ();


   // Return the remove node
   return node;
}
/* ---------------------------------------------------------------------- */



static void *listRemove (void *list, void *unused)
{
   return reinterpret_cast<List *>(list)->remove ();
}


/* ---------------------------------------------------------------------- *//*!

  \fn          ListNode *removeW  ()

  \brief       Removes the node from the tail from this list.
               If the list is empty, the thread of the calling code is 
               blocked until a node becomes available.

  \return      A pointer to the removed node or NULL on error.

                                                                          */
/* ---------------------------------------------------------------------- */
ListNode *ListInterlocked::removeW ()
{
   ListNode *node;

   // Make one attempt to remove the node before going into the wait routine
   if ( (node = List::remove ()) ) return node;


   // Wait for a node to become available
   node = reinterpret_cast<ListNode *>(m_rw.getW (listRemove, this, 0));


   // Return it
   return node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn          ListNode *removeW  (struct timespec const *to)

  \brief       Removes the node from the tail of a previously
               initialized linked list. If the list is empty, the
               thread of code is  blocked until a node becomes
               available or the timeout expires.

  \return      A pointer to the removed node or NULL on error or timeout.


  \param[in]   to The timeout Maybe specified as either TO_NOWAIT or 
               TO_FOREVER.
                                                                          */
/* ---------------------------------------------------------------------- */
ListNode *ListInterlocked::removeW (struct timespec const *to)
{
   ListNode *node;

   // Make one attempt to remove a node before going into the wait routine
   if ( (node = List::remove ()) ) return node;


   // Wait for a node to become available
   node = reinterpret_cast<ListNode *>(m_rw.getW (listRemove, this, 0, to));

   // Return it
   return node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn          ListNode *removeW  (struct timespec const &to)

  \brief       Removes the node from the tail of a previously
               initialized linked list. If the list is empty, the
               thread of code is  blocked until a node becomes
               available or the timeout expires.

  \return      A pointer to the removed node or NULL on error or timeout.


  \param[in]   to The timeout Maybe specified as either TO_NOWAIT or 
               TO_FOREVER.
                                                                          */
/* ---------------------------------------------------------------------- */
ListNode *ListInterlocked::removeW (struct timespec const &to)
{
   ListNode *node = removeW (&to);

   // Return it
   return node;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

