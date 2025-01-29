// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_LIST_HH__
#define __LDMX_UTL_LIST_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/List.hh
  \brief  Singularly link list - non-interlocked versions
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
 * 2019.01.16 jjr Cloned from EXO versions, converted to C++
 *
\* ---------------------------------------------------------------------- */



/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \struct   ListNode
  \brief    Defines the control structure needed to maintain a node on a
            singly linked list.
                                                                          */
/* ---------------------------------------------------------------------- */
struct ListNode
{
   ListNode *m_flnk;  /*!< The forward link                              */

   ListNode *next ();
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Defines a simple singularly linked list
                                                                          */
/* ---------------------------------------------------------------------- */
class List
{
public:
   List ();
  ~List ();

public:
   // Test if the list is empty
   bool     isEmpty ();

   // Insert node at tail and head of the list
   ListNode *insert (ListNode *node);
   ListNode    *jam (ListNode *node);

   // Remove from the head of the list
   ListNode *remove ();

   // List merges
   void      append (List      *src);
   void     prepend (List      *src);

   ListNode *unlink (ListNode const *node, 
                     ListNode  *predessor);

   ListNode *first   ();
   ListNode  *last   ();

protected:
    ListNode *m_flnk;  /*!< The forward  link                             */
    ListNode *m_blnk;  /*!< The backward link                             */
};
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief   Get the next node 
  \return  A pointer to the next node

  \warning
   This is used to scan a list.  It is the responsiblity of the caller
   to determine when the last node on the list has been reached.
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *ListNode::next ()
{
   return m_flnk;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn          void ~List()
  \brief       Destroys the data structures associate with the list.
  \param list  Pointer to the list to destroy.

   Currently this routine is effectively a NO-OP, but is provided for
   upward compatibility, just in case at some time in the future this
   operations does something meaningful.
                                                                          */
/* ---------------------------------------------------------------------- */
inline List::~List ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn     List ()
  \brief  Constructs an empty list (singly linked list).
                                                                          */
/* ---------------------------------------------------------------------- */
inline List::List () :
   m_flnk (reinterpret_cast<ListNode *>(this)),
   m_blnk (reinterpret_cast<ListNode *>(this))
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn          isEmpty ()
  \brief       Returns a flag indicating whether the list is empty.

  \retval      true  If the list is empty
  \retval      false If the list is not empty
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool List::isEmpty ()
{
   bool   empty = (m_flnk == (reinterpret_cast<ListNode *>(&m_flnk)));
   return empty;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the first node in the list
  \return A pointer to the first node in the list
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *List::first   ()
{
   return m_flnk;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the last node in the list
  \return A pointer to the first node in the list
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *List::last ()
{
   return m_blnk;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn        ListNode *insert (ListNode *node)
  \brief     Adds a node to the tail of the list.

  \return    Pointer to the old tail. This can be used to test
             whether this was the first item on the list.

                   If return_value == list then empty

  \param[in] node The node to add at the tail of the mode


   Adds the specified node to the tail of the list. If all nodes are added
   with the insert method, the list behaves as a FIFO.
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *List::insert (ListNode *node)
{
   ListNode *blnk;


   // Current tail
   blnk = m_blnk;


   // Set the termination for the new node
   node->m_flnk = reinterpret_cast<ListNode *>(this);


   // This is now the last item on the list
   m_blnk = node;


   // The old last item's flnk points to the new node
   blnk->m_flnk = node;


   // Return old last link
   return blnk;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn           ListNode *jam (ListNode *node)
  \brief        Adds a node to the head of the list

  \return       Pointer to the old forward link. This can be used to test
                whether this was the first item on the list. \n
                If return_value == list, then empty


  \param  node  The node to add.

   Adds the specified node to the head of the list. If all nodes are added
   with the List::jam method, the list behaves as a LIFO.

                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *List::jam (ListNode *node)
{
   ListNode *flnk;


   // Current first item on the list
   flnk = m_flnk;


   // First link the new node to the old first node
   node->m_flnk = flnk;


   // This is now the first item on the list
   m_flnk = node;


   return flnk;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn        void List::append (List *src)
  \brief     Appends the \ src list members to the this list.

  \param[in] src A previously initialized list acting as the source

   Appends the source list to this list. After this operation, the 
   destination list will consist of its original members followed
   by the members on the source list. The source list will be empty.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void List::append  (List *src)
{
   ListNode *sflnk;
   ListNode *sblnk;
   ListNode *dblnk;

   /*
               DESTINATION LIST

                     BEFORE
                                            NAMES
       +-------> +--------------+ -------+
       |         | flnk dst blnk|        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |  dst.flnk
       |         | flnk  d0     |        |
       |   +---- +--------------+ <--+   |
       |   |                         |   |
       |   +---> +--------------+ ---+   |
       |         | flnk  d0     |        |
       +-------- +--------------+ <------+  dst.blnk


                     AFTER                  NAMES     NEW VALUES

       +-------> +--------------+ -------+            dst.blnk = src.blnk
       |         | flnk dst blnk|        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |  dst.flnk
       |         | flnk d0      |        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |
       |         | flnk d1      |        |
       |   +---- +--------------+        |  dst.blnk  dst.blnk.flnk = src.flnk
       |   |                             |
       |   +---> +--------------+        |  src.flnk
       |         | flnk s0      |        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |
       |         | flnk s0      |        |
       +-------- +--------------+ <------+  src.blnk src.blnk.flnk = dst;
   */


   // Check if source list is empty, if so nothing to do
   sflnk = src->m_flnk;
   if ((char *)sflnk == (char *)src) return;


   // Non-empty, get the current tail of the source & destination lists
   dblnk =      m_blnk;
   sblnk = src->m_blnk;


   // Link this to the first member of the source list
   dblnk->m_flnk = sflnk;


   // Make the tail of the destination the tail of the source list
   m_blnk   = sblnk;


   // Link the current tail of the source list to the destination head
   sblnk->m_flnk = (ListNode *)this;


   // Declare the source list empty
   src->m_flnk = src->m_blnk = (ListNode *)src;


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn           void prepend (List_*src)
  \brief        Prepends the \a src list members to this list.

  \param    src A previously initialized list acting as the source

   Prepends the source list to this list. After this operation, the 
   destination list will have consist of its original members preceded
   by the members on the source list. The source list will be empty.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void List::prepend (List *src)
{
   ListNode *sflnk;
   ListNode *sblnk;
   ListNode *dflnk;


   /*
               DESTINATION LIST

                     BEFORE
                                            NAMES
       +-------> +--------------+ -------+
       |         | flnk dst blnk|        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |  dst.flnk
       |         | flnk  d0     |        |
       |   +---- +--------------+ <--+   |
       |   |                         |   |
       |   +---> +--------------+ ---+   |
       |         | flnk  d0     |        |
       +-------- +--------------+ <------+  dst.blnk


                     AFTER                  NAMES     NEW VALUES

       +-------> +--------------+ -------+
       |         | flnk dst blnk|        |
       |   +---- +--------------+        |            dst.flnk = src.flnk
       |   |                             |
       |   +---> +--------------+        |  src.flnk
       |         | flnk s0      |        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |  src.blnk
       |         | flnk s1      |        |
       |   +---- +--------------+        |            src.blnk.flnk = dst.flnk
       |   |                             |
       |   +---> +--------------+        |  dst.flnk
       |         | flnk d0      |        |
       |   +---- +--------------+        |
       |   |                             |
       |   +---> +--------------+        |  src.blnk
       |         | flnk d0      |        |
       +-------- +--------------+ <------+
   */


   // Check if source list is empty, if so nothing to do
   sflnk = src->m_flnk;
   if (sflnk == reinterpret_cast<ListNode *>(src)) return;


   // Non-empty, get tcurrent head of the source, tail of the destination
   dflnk =      m_flnk;
   sblnk = src->m_blnk;
   
   
   // Make the head of the destination list the first node of the source
   m_flnk = sflnk;


   // Make the tail of the source forward link to the destination
   sblnk->m_flnk = dflnk;


   // Declare the source list empty
   src->m_flnk = src->m_blnk = reinterpret_cast<ListNode *>(src);


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn           ListNode *remove ()
  \brief        Removes the node from the head of a previously initialized
                list.. An empty list returns NULL as its node.

  \return       A pointer to the removed node or NULL if the list is empty.

   Removes the node at the head of the list. If the list is empty, NULL
   is returned.
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *List::remove ()
{
   ListNode *node;

   node = m_flnk;

   // Check if there is an actual node on the list
   if (node != reinterpret_cast<ListNode *>(this))
   {
       ListNode *flnk = node->m_flnk;
       m_flnk         = flnk;         // New forward link is node after 


       // If this caused the list to go empty, then modify blnk
       if (flnk == (reinterpret_cast<ListNode *>(this))) 
       {
          m_blnk = reinterpret_cast<ListNode *>(this);
       }
   }
   else
   {
      node = reinterpret_cast<ListNode *>(0);
   }

   return node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn                 ListNode *unlink (const ListNode  *node,
                                        Listnode *predecessor)
  \brief              Removes the specified node from the list.

  \return             Pointer to the forward link of the removed node.

  \param  node        The node to unlink from the list
  \param  predecessor The predecessor node.


   Unlinks the specified node from the list it is on
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListNode *List::unlink (ListNode const  *node, 
                               ListNode *predecessor)
{
   /*
    | Just need to replace the predecessor's link with the removed node's link
    | A word of caution. Although this looks like one can produce an
    | interlocked version by using a memory reservation technique, in fact
    | you can't because the predecessor node could be removed out from
    | underneath you during the relink, i.e. the predecessor ceases to
    | be the predecessor. One cannot even jacket this routine with a
    | semaphore or an interlock, because the same problem remains, the
    | predecessor was located outside of the lock. Ergo, no interlocked
    | version of this routine can be produced.
   */
   return predecessor->m_flnk = node->m_flnk;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */
#endif
