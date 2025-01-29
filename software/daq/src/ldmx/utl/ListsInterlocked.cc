// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!
                                                                              
   \file   ldmx/utl/ListsInterlocked.cc
   \brief  Implements the methods for an interlocked set of lists
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
   


#include <ldmx/utl/ListsInterlocked.hh>
#include <ldmx/utl/List.hh>
#include <ldmx/utl/ResourceWait.hh>
#include <ldmx/utl/Timeout.hh>
#include <time.h>
#include <errno.h>
#include <new>


/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- */


#if 0
/* ---------------------------------------------------------------------- *//*!

  \def   PL_CTL_GET(_que)
  \brief Locates the control block from a given que.
  \param _que   Pointer to the target que
  \return       Pointer to the PL control block

   This routine allows the user to fetch the context value (a void *)
   which was previously set using \a PL_SET_USR()
                                                                          */
/* ---------------------------------------------------------------------- */
#define PL_CTL_GET(_que)                      \
       ((PL_ctl *)((unsigned char *)(_que) -  \
        (_que)->id * sizeof (*(_que))      -  \
        offsetof(PL_ctl, ques)))
/* ---------------------------------------------------------------------- */
#endif



/* ---------------------------------------------------------------------- *//*!

  \brief Interlocked lists destructor
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::~ListsInterlocked ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
r
  \class   ListsInterlocked::List
  \brief   The definition of an interlocked lists node
                                                                          */
/* ---------------------------------------------------------------------- */
class ListsInterlocked::List : public ldmx::utl::List
{
public:
   List () { return; }
   List (ListsInterlocked::Lcb *lcb, 
         int                     id);
  ~List () { return; }

public:
   ListsInterlocked::Lcb *m_lcb; /*!< Backpointer to the control block    */
   void                  *m_usr; /*!< User location                       */
   int                     m_id; /*!< The id of this que                  */
   int                   m_rsvd; /*!< Reserved, assure 64-bit alignment   */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn   List (ListsInterlocked::Lcb *lcb,
               int                    id)
  \brief Constructor for 1 list of the interlocked lists

  \param[in] lcb   Backpointer to interlocked list control structure
  \param[in[  id   The identifier, \e i.e. list number, for this list
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::List::List (ListsInterlocked::Lcb *lcb,
                              int                     id) :
   m_lcb  (lcb),
   m_usr  (  0),
   m_id   ( id),
   m_rsvd (  0)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn    ListsInterlocked::setUser (ListsInterlocked::List *list,
                                    void                    *ctx)
  \brief Sets the user defined location within the list

  \param[in]  list The target list
  \param[in]   ctx The user context

   This routine allows the user to carry a context value (a void *) along
   with list. The user may fetch this value later using getUser (list).
   This is completely optional but can be useful to tie the list to a 
   more encompassing context.
                                                                          */
/* ---------------------------------------------------------------------- */
void ListsInterlocked::setUser (ListsInterlocked::List *list,
                                void                    *ctx)
{
   list->m_usr = ctx;
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::getUser (ListsInterlocked::List *list)
  \brief  Gets the user defined location within the list_id list
  \return The previously set user context

  \param[in]  list The target list

   This routine allows the user to retrieve the user context value (a void *),
   previously set with setUser, from a list.
                                                                          */
/* ---------------------------------------------------------------------- */
void *ListsInterlocked::getUser (ListsInterlocked::List *list)
{
   return list->m_usr;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
  
  \class ListsInterlocked::Lcb
  \brief The interlocked lists control block

  \warning
   Note that this is a variable length structure, but since its
   implementation is hidden and it is only referenced outside this file
   by a pointer, should cause no usage problems.
                                                                          */
/* ---------------------------------------------------------------------- */
class ListsInterlocked::Lcb
{
public:
   Lcb (int nlists);
  ~Lcb ();

public:
   ListsInterlocked::Set      wake (ListsInterlocked::Set wake_set);
   ListsInterlocked::List *getList (int list_id);


   // Get the number of bytes to hold nlists
   static int sizeofLcb (int nlists);

   // Add a node to the specified list
   static ListsInterlocked::Node *insert  (ListsInterlocked::List *list, 
                                           ListsInterlocked::Node *node);
   static ListsInterlocked::Node *jam     (ListsInterlocked::List *list,
                                           ListsInterlocked::Node *node);
   
   // Remove a node from the highest active priority list
   Node *remove  (volatile const Set *wait_set,
                  int                 *list_id);

   Node *removeW (volatile const Set *wait_set,
                  int                 *list_id);

   Node *removeW (volatile const Set *wait_set,
                  int                 *list_id,
                  struct timespec const    *to);

   Node *remove  (int list_id);

   void append   (int           list_id, 
                  ldmx::utl::List *list);


   Set append (ldmx::utl::List       *lists);
   Set append (ldmx::utl::List        *lists,
               Set                      set);

   Set appendW (ldmx::utl::List      *lists);
   Set appendW (ldmx::utl::List      *lists,
                Set                     set);
   Set appendW (ldmx::utl::List      *lists,
                Set                     any,
                Set                     all);


   Set appendW (ldmx::utl::List      *lists,
                struct timespec const   *to);
   Set appendW (ldmx::utl::List      *lists,
                Set                     set,
                struct timespec const   *to);
   Set appendW (ldmx::utl::List      *lists,
                Set                     any,
                Set                     all,
                struct timespec const   *to);


   // Return the set of active lists
   Set  get         ();
   Set  getW        (volatile const Set *wait_set);

   Set  getW        (volatile const Set *wait_set,
                     struct timespec const    *to);

   Set  getW        (Set                      any,
                     Set                      all);

   Set  getW        (Set                      any,
                     Set                      all,
                     struct timespec const    *to);

public:
   ResourceWait                 m_rw; /*!< Resource wait handle           */
   ListsInterlocked::Set       m_cfg; /*!< Set of configured lists        */
   ListsInterlocked::Set    m_active; /*!< Set of active lists            */
   uint32_t                   m_rsvd; /*!< Reserved                       */
   int                      m_nlists; /*!< Number of configured lists     */
   ListsInterlocked::List m_lists[1]; /*!< The list of configured queues  */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Cheap constructor for ListsInterlocked

  \par
   This version allocates memory from the heap 
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::ListsInterlocked (int nlists)
{
   int nbytes = ListsInterlocked::Lcb::sizeofLcb (nlists);
   m_lcb = reinterpret_cast<ListsInterlocked::Lcb *>(new char[nbytes]);

   new (m_lcb) ListsInterlocked::Lcb (nlists);


   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \struct  RemoveInfo
  \brief   Internal structure to pass the pointer to the wait list and
           receive back the que id from the ListInterlocked::remove 
           routines
                                                                          */
/* ---------------------------------------------------------------------- */
struct RemoveInfo
{
   RemoveInfo (const volatile ListsInterlocked::Set *wait_set,
               int                                   *list_id) :
      m_wait_set (wait_set),
      m_list_id   (list_id)
      {
         return;
      }

   volatile const ListsInterlocked::Set  
                            *m_wait_set; /*!< Address of the wait set     */
   int                       *m_list_id; /*!< The que id (out)            */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn        Lcb (int nlists)
  \brief     Initializes the list head of a doubly linked list to an empty
             list of the specified type.
               
  \param[in] nlists The number of lists to manaage.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Lcb::Lcb (int nlists) :
   m_rw     (),
   m_cfg    (ListsInterlocked::listSet (nlists)),
   m_active (0),
   m_rsvd   (0),
   m_nlists (nlists)
{
   List *list = m_lists;
   int   id   = 0;   


   // Initialize the lists
   while (--nlists >= 0) 
   {
      new (list) List (this, id);
      id         += 1;
      list       += 1;
   }

   return;
}   
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn          void ~Lcb ()
  \brief       Destroys the data structures associate with the interlocked
               lists.

   This routine frees any resources gathered when interlocked list class
   was constructed. 
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Lcb::~Lcb ()
{
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn    ListsInterlocked::sizeofLcb (int nlists)
  \brief Computes the number of bytes needed for set of \a nlists

  \param[in] nlist The number of lists

   This is used to drop enough storage for the control structure + 
   the \e control structure +  nlists. 

   Example
  \verbatim

    / * Drops enough storage for a control structure and 5 lists * /
    unsigned char my_pl[ListsInterlocked::sizeofLcb (5));

    ListsInterlocked::Lcb *lcb = new (my_pl) ListsInterlocked::Lcb (5);

    ListsInterlocked lists (lcb);
   \endverbatim 
                                                                          */
/* ---------------------------------------------------------------------- */
int ListsInterlocked::Lcb::sizeofLcb (int nlists)
{ 
   int nbytes = (sizeof (ListsInterlocked::Lcb)
              + ((nlists) - 1) * sizeof(ListsInterlocked::List));
   return nbytes;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::getUser (int list_id)
  \brief  Gets the user defined location within the list
  \return The previously set user context

  \param[in]  list_d The target list identifier

   This routine allows the user to retrieve the user context value (a void *),
   previously set with setUser, from a list by its identifier
                                                                          */
/* ---------------------------------------------------------------------- */
void *ListsInterlocked::getUser (int list_id)
{
   return m_lcb->m_lists[list_id].m_usr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
   
  \fn         ListsInterlocked::Node *insert (ListsInterlocked::List  *list,
                                              ListsInterlocked::Node *node)
  \brief      Adds a node to the tail of a managed list.
  \return     Pointer to the old backward link. This can be used to test 
              whether this was the first item on the queue. \n
              If return_value == list, then the list was initially empty.

  \param[in]  node  The node to add to the \a list

   Adds the specified node to the tail of the que. If all nodes
   are added with the \a insert method, the que behaves as a FIFO.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::insert (ListsInterlocked::List *list,
                          ListsInterlocked::Node *node)
{
   ListsInterlocked::Node *prvNode = m_lcb->insert (list, node);
   return                  prvNode;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn             ListsInterlocked::Node *jam (ListsInterlocked::List *list,
                                               ListsInterlocked::Node *node)
  \brief          Adds a node to the head of a managed set of lists
  \return         Pointer to the old forward link. This can be used to test 
                  whether this was the first node on the list. \n
                  If return_value == list, then the list was initially empty

  \param[in] node The node to add to \a list

   Adds the specified node to the head of the list. If all nodes
   are added with the jam method, the list behaves as a LIFO.

                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::jam (ListsInterlocked::List *list,
                       ListsInterlocked::Node *node)
{
   ListsInterlocked::Node *prvNode = m_lcb->jam (list, node);
   return                  prvNode;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListInterlocked::List *getList (int id)
  \brief  Locates a pointer to the list specified by \a list_id
  \return A pointer to the list specified by \a id or NULL if \a id
          specifies an invalid or non-configured list.

  \param[in] id  The id of the list to locate
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::List *ListsInterlocked::getList (int id)
{
   ListsInterlocked::List *list = m_lcb->getList (id);
   return                  list;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \fn     ListsInterlocked::Node 
          *remove (volatile const ListsInterlocked::Set *set,
                   int                                   *id)
  \brief  Removes a node from the head of the highest priority non-empty
          list. If all eligible lists are empty, then \e remove
          returns NULL immediately without blocking.

  \param[in]  set A pointer to the set of list to wait on. The wait
                  is treated as an OR.
  \param[ou[t id  Returned with the id of the list the node was removed
                  from.
  
  \return A pointer to the removed node, or NULL if all the lists
          are empty.

   Removes the node at the head of the highest priority non-empty list. 
   If the all eligible lists are empty, NULL is immediately returned.
   This routine does not block.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::remove (volatile const ListsInterlocked::Set *set,
                          int                                   *id)
{
   ListsInterlocked::Node *node = m_lcb->remove (set, id);
   return                  node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Node
         *removeW (volatile const ListsInterlocked::Set *set,
                   int                                   *id)
  \brief  Removes a node from the head of the highest priority non-empty
          list. If all eligible queues are empty, then \e removeW waits
          indefinitely.

  \param[in]  set A pointer to the set of the lists to wait on. The wait
                  is treated as an OR.
  \param[out] id  Returned with the id of the list the node was removed
                  from.
  
  \return A pointer to the removed node, or NULL if on error

   Removes the node at the head of highest priority available and 
   eligible list. If all lists are empty, the routine blocks indefinitely
   or until something shows up or there is an error. 

   If there is an error, the routine returns NULL as its node. There are
   only two reasons NULL will be returned:
  \verbatim
     1. Internal error
     2. The list of eligible ques does not include any configured ques.
  \endverbatim 
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::removeW (volatile const ListsInterlocked::Set *set,
                           int                                   *id)
{
   ListsInterlocked::Node *node = m_lcb->remove (set, id);
   return                  node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn    ListsInterlocked::Node 
        *removeW (volatile const ListsInterlocked::Set *set,
                  int                                   *id,
                  struct timespec const                 *to)
  \brief Removes a node from the head of the highest priority non-empty
         que. If all eligible queues are empty, then \e removeW_
         waits the specified until the timeout expires.

  \param[in]    set  A pointer to the set of the lists to wait on. The 
                     wait is treated as an OR.
  \param[out]        Returned with the id of the list the node was
                     removed from
  \param        toc  The timeout control. this maybe specified as
                     \a TO_FOREVER or TO_NOWAIT.
  \return            A pointer to the removed node, or NULL on error

   Removes the node at the head of the highest priority non-empty list. 
   If the all eligible queues are empty, then \e removeW waits
   until something shows up or the timeout period, specified by \a to
   expires.

   If there is an error, the routine returns NULL as its node. There are
   only three reasons NULL will be returned:

     - # Internal error
     - # The list of eligible ques does not include any configured ques.
     - # Timeout expired.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::removeW (volatile const ListsInterlocked::Set *set,
                           int                                   *id,
                           const struct timespec                 *to)
{
   ListsInterlocked::Node *node = m_lcb->removeW (set, id, to);
   return                  node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn    ListsInterlocked::Node 
        *removeW (volatile const ListsInterlocked::Set *set,
                  int                                   *id,
                  struct timespec const                 &to)
  \brief Removes a node from the head of the highest priority non-empty
         que. If all eligible queues are empty, then \e removeW_
         waits the specified until the timeout expires.

  \param[in]    set  A pointer to the set of the lists to wait on. The 
                     wait is treated as an OR.
  \param[out]        Returned with the id of the list the node was
                     removed from
  \param        toc  The timeout control. this maybe specified as
                     \a TO_FOREVER or TO_NOWAIT.
  \return            A pointer to the removed node, or NULL on error

   Removes the node at the head of the highest priority non-empty list. 
   If the all eligible queues are empty, then \e removeW waits
   until something shows up or the timeout period, specified by \a to
   expires.

   If there is an error, the routine returns NULL as its node. There are
   only three reasons NULL will be returned:

     - # Internal error
     - # The list of eligible ques does not include any configured ques.
     - # Timeout expired.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::removeW (volatile const ListsInterlocked::Set *set,
                           int                                   *id,
                           const struct timespec                 &to)
{
   ListsInterlocked::Node *node = m_lcb->removeW (set, id, &to);
   return                  node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Append specified list to the interlocked list, \a list_id


  \param[in]     list_id The target list identifier
  \param[in:out]   lists The array of list to transfer to
                                                                          */
/* ---------------------------------------------------------------------- */
void ListsInterlocked::append (int list_id, ldmx::utl::List *list)
{
   m_lcb->append (list_id, list);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Immediately append the nodes from the set of all currently
           active lists to \a lists)
  \return The set of lists that are active.  If no lists are active,
          the 0 is returned.

  \param[in:out] lists The array of lists to append to
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::append (ldmx::utl::List *lists)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists);
   return                retSet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Immediately append the nodes from the subset of currently
           active lists to \a lists)
  \return The set of lists that were active


  \param[in:out] lists The array of lists to append to
  \param[in]       set The subset of lists to target
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::append (ldmx::utl::List        *lists,
                          ListsInterlocked::Set     set)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists, set);
   return                retSet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set 
ListsInterlocked::appendW (ldmx::utl::List      *lists)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists);
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::appendW (ldmx::utl::List      *lists,
                           ListsInterlocked::Set   set)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists, set);
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       any Returns if any of the specified lists are active
  \param[in]       all Returns if all of the specified lists are active
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::appendW (ldmx::utl::List      *lists,
                           ListsInterlocked::Set   any,
                           ListsInterlocked::Set   all)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists, any, all);
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is 
           active or the timeout expires
  \return  The set of lists that were active.  A zero value will be 
           returned iff the method times out.

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::appendW (ldmx::utl::List      *lists,
                           struct timespec const   *to)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists, to);
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is 
           active or the timeout expires
  \return  The set of lists that were active.  A zero value will be 
           returned iff the method times out.

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::appendW (ldmx::utl::List      *lists,
                           ListsInterlocked::Set   set,
                           struct timespec const   *to)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists, set, to);
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       any Returns if any of the specified lists are active
  \param[in]       all Returns if all of the specified lists are active
  \param[in]        to The timeout time
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::appendW (ldmx::utl::List      *lists,
                           ListsInterlocked::Set   any,
                           ListsInterlocked::Set   all,
                           struct timespec const   *to)
{
   ListsInterlocked::Set retSet = m_lcb->appendW (lists, any, all, to);
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set get ()
  \brief  Returns a list of the currently active lists.

  \return A list of the currently active lists.

  \warning
   The caller is cautioned that the list returned only reflects the state
   of the active lists at the time of the call. Writers to the lists may
   add active nodes to previously inactive lists or, in the case of multiple
   readers, other readers may remove nodes causing previously active lists
   to become inactive.

   Note that in single reader usage, the returned list will always be at
   least a subset of the active ques. Since no other reader is removing
   nodes, the active list can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set ListsInterlocked::get ()
{
   ListsInterlocked::Set retSet = m_lcb->get ();
   return                retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set getW (volatile const ListsInterlocked::Set set)
  \brief  Returns a list of the currently active lists which, at the time
          of the call, is guaranteed to include at least one member of
          the \a set. The routine blocks indefinitely until this
          condition is met.
  \return The set of active lists. It is guaranteed to include at least
          one member in \a set. Note that the list is not trimmed
          to the wait list. This would destroy information and the
          trimming is easily done by user.

  \param[in]  set  A pointer to the set of the list to wait on. The
                   wait is treated as an OR.


  \warning
   The caller is cautioned that the setreturned only reflects the state
   of the active lists at the time of the call. Writers may add active nodes
   to previously inactive lists or, in the case of multiple readers, other
   readers may remove nodes causing previously active liststo become empty.

   Note that in single reader usage, the returned list will always be at
   least a subset of the active set. Since no other reader is removing
   nodes, the active set can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set 
ListsInterlocked::getW (volatile const ListsInterlocked::Set *set)
{
   ListsInterlocked::Set subSet = m_lcb->getW (set);
   return                subSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn    ListsInterlocked::Set 
         getW (volatile const ListsInterlocked::Set *set,
               const struct timespec                *to)
  \brief Returns the set of the currently active lists which, at the time
         of the call, is guaranteed to include at least one member of
         the \a set. The routine blocks until this condition is met
         or the timeout expires.
 \return The set of active lists. It is guaranteed to include at least
         one member in \a set. Note that the list is not trimmed
         to \a set. This would destroy information and the trimming is 
         easily done by user


  \param[in] set  A pointer to the set of the lists to wait on. The
                  wait is treated as an OR.
  \param[in]  to  The timeout


  \warning
   The caller is cautioned that the set returned only reflects the state
   of the active lists at the time of the call. Writers may add active 
   nodes to previously inactive lists or, in the case of multiple
   readers, other readers may remove nodes causing previously active lists
   to become inactive.

   Note that in single reader usage, the returned set will always be at
   least a subset of the set of the active lists. Since no other reader 
   is removing nodes, the active set can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set 
ListsInterlocked::getW (volatile const ListsInterlocked::Set *set,
                        const struct timespec                 *to)
{
   ListsInterlocked::Set subSet = m_lcb->getW (set, to);
   return                subSet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set ListsInterlocked::wake (ListInterlocked::Set set)
  \brief  Issues a wake up call if any of lists in \a set are currently active. 
  \return The set of active lists at the time the wake up call was
          issued.


  \param[in]    The set of ques to issue the wakeup call on

   This routine allows the user to effectively  enable and disable the 
   servicing of lists of ques. Remember that the wait list parameter
   passed to \e PL__remove(), \e PL__removeW() and \e PL__removeW_toc
   is passed as a volatile pointer. This means that some other piece of
   code is allowed to dynamically alter the contents of the wait list.
   However, if the remove routines are already pending on a list, the 
   remove routines will never notice the change. Hence the reason for this
   routine.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set ListsInterlocked::wake (ListsInterlocked::Set set)
{
   ListsInterlocked::Set wakeSet = m_lcb->wake (set);
   return                wakeSet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn    ListsInterlocked::Node *remove_s (ListsInterlocked::Lcb        *lcb,
                                           ListsInterlocked::RemoveInfo *rmv)
  \brief Funky internal routine used as the \a ResourceWait wait callback
  
  \param lcb[in] The private ListsInterlocked control block
  \param rmv     The removal information structure. This holds pointers to
                 the wait_set (in) and the list_id to be returned.
  \returns       The removed node or NULL if timeout.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *remove_s (ListsInterlocked::Lcb  *lcb,
                                  RemoveInfo             *rmv)
{
   return lcb->remove (rmv->m_wait_set, rmv->m_list_id);
}
/* ---------------------------------------------------------------------- */



#include <stdio.h>

/* ---------------------------------------------------------------------- *//*!

  \fn      ListsInterlocked::Node *insert (ListsInterlocked::List *list,
                                           ListsInterlocked::Node *node)
  \brief   Adds a node to the tail of a managed set of interlocked lists.

  \return  Pointer to the old backward link. This can be used to test
           whether this was the first item on the list. \n
              If return_value == list, then the list was initially empty.

  \param[in] list   The target list
  \param[in] node   The node to add to \a list

   Adds the specified node to the tail of the list. If all nodes
   are added with the \e insert method, the list behaves as a FIFO.
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Node *
       ListsInterlocked::Lcb::insert(ListsInterlocked::List *list, 
                                     ListsInterlocked::Node *node)
{
   // Get and lock the control structure
   Lcb *lcb = list->m_lcb;
   lcb->m_rw.lock ();


   // Insert the new node on the list
   ListNode *blnk = list->insert (node);


   // Did this cause the que to go non-empty
   if (blnk == reinterpret_cast<ldmx::utl::ListNode *>(list))
   {
      lcb->m_active = add (lcb->m_active, list->m_id);
      lcb->m_rw.wake ();
   }

   lcb->m_rw.unlock ();

   return blnk;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Node *jam (ListsInterlocked::List *list,
                                       ListsInterlocked::Node *node)
  \brief  Adds a node to the head of a managed set of interlocked lists.
  \return Pointer to the old forward link. This can be used to test
          whether this was the first item on the list. \n
          If return_value == list, then the list was initially empty


  \param  list  the target list
  \param  node  The node to add to \a list

   Adds the specified node to the head of the list. If all nodes
   are added with the \e jam method, the que behaves as a LIFO.
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Node *
ListsInterlocked::Lcb::jam (ListsInterlocked::List *list, 
                            ListsInterlocked::Node *node)
{
   // Get and lock the control structure
   Lcb *lcb  = list->m_lcb;
   lcb->m_rw.lock ();


   // Jam the new node on the list
   ListNode *flnk = list->jam (node);


   // Did this cause the que to go non-empty
   if (flnk == reinterpret_cast<ldmx::utl::ListNode *>(list))
   {
      lcb->m_active = add (lcb->m_active, list->m_id);
      lcb->m_rw.wake ();
   }

   lcb->m_rw.unlock ();


   return flnk;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::List *getList (int id)
  \brief  Locates a pointer to the list specified by \a id
  \return A pointer to the list specified by \a id or NULL if \a qid
          specifies an invalid or non-configured list.

  \param[in] id  The id of the list to locate

                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::List *ListsInterlocked::Lcb::getList (int id)
{
   return  (id < m_nlists)
          ? &m_lists[id]
          : NULL;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Node *
          rmv (volatile const ListsInterlocked::Set *wait_set,
               int                                   *list_id)

  \brief  Bare bones, non-interlocked removal routine. This is
          for internal use only.

  \return A pointer to the deque'd node of the highest
                    priority active que in the \a wait_set. If
                    the \a wait_set does not specify any configured
                    ques, NULL is returned.


  \param[in] wait_set  Pointer to the set of lists to wait on.
  \param           id  Returned with the id of the list the node that was 
                       removed.

                                                                          */
/* ---------------------------------------------------------------------- */
static inline ListsInterlocked::Node *
         rmv (ListsInterlocked::Lcb                     *lcb,
              volatile const ListsInterlocked::Set *wait_set,
              int                                   *list_id)
{
   // Get a list of the active and ques to wait on
   ListsInterlocked::Set active = lcb->m_active & *wait_set;

   // Check if anything to remove
   if (active == 0)
   {
       return NULL;
   }
   else
   {
      // Locate the highest priority active list
      int     id = ListsInterlocked::find (active);
      auto *list = &lcb->m_lists[id];

     *list_id        = id;
      ListNode *node = list->remove ();

      // Check if the que is now empty...
      if (list->isEmpty ())
      {
         /* ... if empty, remove it from the list of active ques */
         lcb->m_active = ListsInterlocked::remove (active, id);
       }

      return node;
   }
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Node *
          ListsInterlocked::Lcb::
          remove (volatile const ListsInterlocked::Set *wait_set,
                  int                                   *list_id)

  \brief  Removes a node from the head of the highest priority non-empty
          list. If all eligible lists are empty, then \e removes
          returns NULL immediately without blocking.

  \return A pointer to the removed node, or NULL if all the lists are
          empty.

  \param   wait_set A pointer to the set of the lists to wait on. The wait
                    is treated as an OR.
  \param   list_id  Returned with the id of the list the node was removed
                    from.


   Removes the node at the head of the highest priority non-empty que.
   If the all eligible queues are empty, NULL is immediately returned.
   This routine does not block.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::Lcb::remove (volatile const ListsInterlocked::Set *wait_set,
                               int                                   *list_id)
{
   // Lock the control structure
   m_rw.lock ();

   // Use bare bones no lock/unlock to remove the node
   auto *node = rmv (this, wait_set, list_id);

   // Unlock the queue
   m_rw.unlock ();

   return node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Node *
          ListsInterlocked::Lcb::
          removeW (volatile const ListsInterlocked::Set *wait_set,
                   int                                   *list_id)

  \brief  Removes a node from the head of the highest priority non-empty
          list. If all eligible list are empty, then \e removeW waits
          indefinitely.

  \param   wait_set A pointer to the set of the list to wait on. The wait
                    is treated as an OR.
  \param    list_id Returned with the id of the list the node was removed
                    from.

  \return A pointer to the removed node, or NULL if on error

   Removes the node at the highest priority available and eligible list.
   If all lists are empty, the routine blocks indefinitely or until
   something shows up or there is an error.

   If there is an error, the routine returns NULL as its node. There are
   only two reasons NULL will be returned:
  \verbatim
     1. Internal error
     2. The set of eligible lists does not include any configured lists.
  \endverbatim
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::Lcb::
               removeW (volatile const ListsInterlocked::Set *wait_set,
                        int                                   *list_id)
{
   ListsInterlocked::Node  *node;

   /*
    | Make one attempt to remove the node before going into the
    | wait routine.
   */
   if ((node = remove (wait_set, list_id))) return node;

   RemoveInfo rmv (wait_set, list_id);
   node = reinterpret_cast<ListsInterlocked::Node *>
          (m_rw.get ((ResourceWait::GetCb)remove_s, this, &rmv));

   return node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Node *
          ListsInterlocked::Lcb::
                removeW (volatile const ListsInterlocked::Set *wait_set,
                         int                                   *list_id,
                         const struct timespec                      *to)

  \brief  Removes a node from the head of the highest priority non-empty
          list. If all eligible lists are empty, then \e removeW
          waits the specified until the timeout expires.

  \return A pointer to the removed node, or NULL on error

  \param[in]  wait_set  A pointer to the list of the ques to wait on. The wait
                        is treated as an OR.
  \param[out]  list_id  Returned with the id of the que the node was removed
                        from.

  \param[in]        to  The TimeOut control structure. This may be specified
                        as TO_NOWAIT or TO_FOREVER.


   Removes the node at the head of the highest priority non-empty list.
   If the all eligible lists are empty, then \e removeW waits
   until something shows up or the timeout period, specified by \a to
   expires.


   If there is an error, the routine returns NULL as its node. There are
   only three reasons NULL will be returned:

     - # Internal error
     - # The list of eligible ques does not include any configured ques.
     - # Timeout expired.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Node *
ListsInterlocked::Lcb::
               removeW (volatile const ListsInterlocked::Set *wait_set,
                        int                                   *list_id,
                        const struct timespec                      *to)
{
   ListsInterlocked::Node   *node;

   /*
    | Make one attempt to remove a node before launching into the
    | full fledged timeout removal routine.
   */
   if ( (node = remove (wait_set, list_id)) || Timeout::isNoWait (to))
   {
      return node;
   }


   RemoveInfo rmv (wait_set, list_id);
   node = reinterpret_cast<ListsInterlocked::Node *>
         (m_rw.getW ((ResourceWait::GetCb)remove_s, this, &rmv, to));

   return node;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static ListsInterlocked::Set append (ListsInterlocked::Lcb *lcb, 
                                     ldmx::utl::List     *lists,
                                     ListsInterlocked::Set  set)
{
   auto retSet = set &= lcb->m_active;

   while (set)
   {
      int id = __builtin_ctz (set);
      List *list = lcb->m_lists + id;
      lists[id].append (list);
      set &= ~(1 << id);
   }

   lcb->m_active &= ~retSet;

   return retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Append specified list to the interlocked list, \a list_id

  \param[in]     list_id The target list identifier
  \param[in:out]   lists The array of list to transfer to
                                                                          */
/* ---------------------------------------------------------------------- */
void ListsInterlocked::Lcb::append (int list_id, ldmx::utl::List *list)
{
   m_rw.lock ();


   List *dstList = m_lists + list_id;
   bool wasEmpty = dstList->isEmpty ();
   dstList->append (list);


   // ---------------------------------------------------
   // If was empty and no longer is, wake up the consumer
   // ---------------------------------------------------
   if (wasEmpty)
   {
      m_active = add (m_active, list_id);
      m_rw.wake ();
   }

   m_rw.unlock ();

   return;
}


/* ---------------------------------------------------------------------- *//*!

  \brief  Immediately append the nodes from the set of all currently
           active lists to \a lists)
  \return The set of lists that are active.  If no lists are active,
          the 0 is returned.

  \param[in:out] lists The array of lists to append to
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::append (ldmx::utl::List *lists)
{
   m_rw.lock ();

   auto set = ldmx::utl::append (this, lists, m_cfg);

   m_rw.unlock ();

   return set;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Immediately append the nodes from the subset of currently
           active lists to \a lists)
  \return The set of lists that were active


  \param[in:out] lists The array of lists to append to
  \param[in]       set The subset of lists to target
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::append (ldmx::utl::List        *lists,
                               ListsInterlocked::Set     set)
{
   m_rw.lock ();

   auto retSet = ldmx::utl::append (this, lists, set);

   m_rw.unlock ();

   return retSet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set 
ListsInterlocked::Lcb::appendW (ldmx::utl::List *lists)
{
   ListsInterlocked::Set retSet;
   m_rw.lock ();

   while (1)
   {
      if (m_rw.m_waiting.bf.dsb == 0)
      {
         retSet = ldmx::utl::append (this, lists, m_cfg);
         if (retSet) break;
      }

      m_rw.wait ();
   }
         
   m_rw.unlock ();
   
   return retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::appendW (ldmx::utl::List      *lists,
                                ListsInterlocked::Set   set)
{
   ListsInterlocked::Set retSet;
   m_rw.lock ();

   while (1)
   {
      if (m_rw.m_waiting.bf.dsb == 0)
      {
         retSet = ldmx::utl::append (this, lists, set);
         if (retSet) break;
      }

      m_rw.wait ();
   }
         
   m_rw.unlock ();
   
   return retSet;
}
/* ---------------------------------------------------------------------- */







/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is active
  \return  The set of lists that were active.  Since this methoud waits
           indefinitely, this will always be non-zero

  \param[in:out] lists The array of lists to append to
  \param[in]       any Completes if any of these lists are active
  \param[in]       all Completes if all of these lists are active
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::appendW (ldmx::utl::List      *lists,
                                ListsInterlocked::Set   any,
                                ListsInterlocked::Set   all)
{
   ListsInterlocked::Set active;

   m_rw.lock ();

   while (1)
   {
      if (m_rw.m_waiting.bf.dsb == 0)
      {
         active  = m_active;
         bool satisfied = ( (active & any) || ((active & all) == all) );

         // Do not append the lists until the condition is satisfied
         if (satisfied)
         {
            ldmx::utl::append (this, lists, active);
            break;
         }
         else
         {
            // not statisfied, go back to waiting
         }
      }

      m_rw.wait ();
   }
         
   m_rw.unlock ();
   
   return active;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is 
           active or the timeout expires
  \return  The set of lists that were active.  A zero value will be 
           returned iff the method times out.

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::appendW (ldmx::utl::List      *lists,
                                struct timespec const   *to)
{
   ListsInterlocked::Set retSet = 0;
   m_rw.lock ();

   while (1)
   {
      if (m_rw.m_waiting.bf.dsb == 0)
      {
         retSet = ldmx::utl::append (this, lists, m_cfg);
         if (retSet) break;
      }

      int status = m_rw.wait (to);


      if (status == ETIMEDOUT) break;
   }
         
   m_rw.unlock ();

   return retSet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is 
           active or the timeout expires
  \return  The set of lists that were active.  A zero value will be 
           returned iff the method times out.

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::appendW (ldmx::utl::List      *lists,
                                ListsInterlocked::Set   set,
                                struct timespec const   *to)
{
   ListsInterlocked::Set retSet = 0;
   m_rw.lock ();

   while (1)
   {
      if (m_rw.m_waiting.bf.dsb == 0)
      {
         retSet = ldmx::utl::append (this, lists, set);
         if (retSet) break;
      }

      int status = m_rw.wait (to);


      if (status == ETIMEDOUT) break;
   }
         
   m_rw.unlock ();

   return retSet;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief   Append the nodes from the set of all currently active lists 
           to \a lists). This method waits until at least one list is 
           active or the timeout expires
  \return  The set of lists that were active.  A zero value will be 
           returned iff the method times out.

  \param[in:out] lists The array of lists to append to
  \param[in]       set The set to wait on.  The wait is treated as an OR
                                                                          */
/* ---------------------------------------------------------------------- */
inline ListsInterlocked::Set
ListsInterlocked::Lcb::appendW (ldmx::utl::List      *lists,
                                ListsInterlocked::Set   any,
                                ListsInterlocked::Set   all,
                                struct timespec const   *to)
{
   ListsInterlocked::Set active;
   m_rw.lock ();

   while (1)
   {
      if (m_rw.m_waiting.bf.dsb == 0)
      {
         active = m_active;
         bool satisfied = ( (active & any) || ((active & all) == all) );

         // Do not append the lists until the condition is satisfied
         if (satisfied)
         {
            ldmx::utl::append (this, lists, active);
            break;
         }
         else
         {
            // Continue waiting
         }
      }

      int status = m_rw.wait (to);


      if (status == ETIMEDOUT)
      {
         // Return what is available
         ldmx::utl::append (this, lists, active);
         break;
      }
   }
         
   m_rw.unlock ();

   return active;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set wake (ListsInterlocked::Set wakeup_set)
  \brief  Issues a wake up call if any of the lists in the \a wakeup_set
          are currently active.
  \return The set of active lists at the time the wake up call was issued.


  \param wakeup_set  The set of lists to issue the wakeup call on

   This routine allows the user to effectively enable and disable the
   servicing of lists of ques. Remember that the wait list parameter
   passed to \e remove(), \e removeW() (without or with a timeout)
   is passed as a volatile pointer. This means that some other piece of
   code is allowed to dynamically alter the contents of the wait list.
   However, if the remove routines are already pending on a list, the
   remove routines will never notice the change. Hence the reason for this
   routine.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set 
ListsInterlocked::Lcb::wake(ListsInterlocked::Set wakeup_set)
{
    m_rw.lock ();
    ListsInterlocked::Set active = m_active;

    if (wakeup_set & active) m_rw.wake ();

    m_rw.unlock ();

    return active;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     test_or (ListsInterlocked::Lcb                *lcb, 
                   volatile const ListsInterlocked::Set *set)
  \brief  Tests whether the current active set of lists contains any
          member of the specified \a set.

  \param  lcb   The interlocked list control handle
  \param  set   A pointer to the set of the lists to test.

  \return The set of active lists. Note that this set is not trimmed
          by the \a set. The caller is free to do this if he wishes.
          If the set does not contain any members of \a set, 0 is
          returned.

  \warning This is an internal routine primarily used as a ResourceWait
           callback.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set test_or (ListsInterlocked::Lcb                *lcb, 
                               volatile const ListsInterlocked::Set *set)
{
   ListsInterlocked::Set active = lcb->m_active;
   return (active & *set) ? active : 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     test_anyAll (ListsInterlocked::Lcb            *lcb, 
                       const ListsInterlocked::Set anyAll[2])

  \brief  Tests whether the current active set of lists contains any
          member of anyAll[0] or all of anyAll[1] 

  \param  lcb    The interlocked list control handle
  \param  anyAll An array, the first member is the set of \e any 
                 lists and the second is the all set of lists.
                 Logically, this will be mutually exclusive.

  \return The set of active lists. Note that this set is not trimmed
          by the \a set. The caller is free to do this if he wishes.
          If the set does not contain any members of \a set, 0 is
          returned.

  \warning This is an internal routine primarily used as a ResourceWait
           callback.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set test_anyAll (ListsInterlocked::Lcb            *lcb, 
                                   const ListsInterlocked::Set anyAll[2])
{
   ListsInterlocked::Set any = anyAll[0];
   ListsInterlocked::Set all = anyAll[1];
   ListsInterlocked::Set active = lcb->m_active;
   return ( (active & any) || ((active & all) == all)) ? active : 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn      ListsInterlocked::Set ListsInterlocked::Lcb::get ()
  \brief   Returns the set of the currently active lists
  \return  A list of the currently active ques.

  \warning
   The caller is cautioned that the set returned only reflects the state
   of the active lists at the time of the call. Writers to the lists may
   add active nodes to previously inactive lists or, in the case of 
   multiple readers, other readers may remove nodes causing previously 
   active lists to become inactive.

   Note that in single reader usage, the returned set will always be at
   least a subset of the active lists. Since no other reader is removing
   nodes, the active list can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set ListsInterlocked::Lcb::get ()
{
   return m_active;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set 
          getW (volatile const ListsInterlocked::Set *wait_set)

  \brief  Returns the set of currently active lists which, at the time
          of the call, is guaranteed to include at least one member of
          the \a wait_set. The routine blocks indefinitely until this
          condition is met.

  \return The set of active ques. It is guaranteed to include at least
          one member in \a wait_set. Note that the set is not trimmed
          to \a wait_set. This would destroy information and the
          trimming is easily done by user

  \param  wait_set  A pointer to the list of the ques to wait on. The wait
                    is treated as an OR.

  \warning
   The caller is cautioned that the set returned only reflects the state
   of the active lists at the time of the call. Writers to the lists may
   add active nodes to previously inactive lists or, in the case of
   multiple readers, other readers may remove nodes causing previously 
   active lists to become inactive.

   Note that in single reader usage, the returned set will always be at
   least a subset of the active lists. Since no other reader is removing
   nodes, the active set can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::Lcb::getW (volatile const ListsInterlocked::Set *wait_set)
{
   ListsInterlocked::Set active;

   /*
    | Interesting issue:
    |   Should this be done under the protection of a lock?
    |
    | Answer:
    |   No real reason to do this. Why? Because the set being returned
    |   can only be treated as a possible set of active lists. It is
    |   entirely possible that the set of active lists will be modified
    |   before the caller consumes this set. This can happen with both
    |   single and multiple readers of the lists. In both cases, the
    |   writer may add nodes to previously inactive lists, rendering the
    |   result returned by this routine invalid. In the multiple reader
    |   case, another reader may remove a node from a que causing it to
    |   go inactive.
    |
    | Conclusion
    |   The result returned from this routine must be treated as only
    |   a suggestion. Only by trying to remove a node from one of the
    |   suggested ques can one tell whether the list is empty or not.
   */
   active = test_or (this, wait_set);
   if (active) return active;

   active = static_cast<ListsInterlocked::Set>
          ((uintptr_t)m_rw.getW (reinterpret_cast<ResourceWait::GetCb>(test_or),
                                 this,
                                 (void *)wait_set));
   return active;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set 
          getW (volatile const ListsInterlocked::Set *wait_set)

  \brief  Returns the set of currently active lists which, at the time
          of the call, is guaranteed to include at least one member of
          the \a wait_set. The routine blocks indefinitely until this
          condition is met.

  \return The set of active ques. It is guaranteed to include at least
          one member in \a wait_set. Note that the set is not trimmed
          to \a wait_set. This would destroy information and the
          trimming is easily done by user

  \param  wait_set  A pointer to the list of the ques to wait on. The wait
                    is treated as an OR.

  \warning
   The caller is cautioned that the set returned only reflects the state
   of the active lists at the time of the call. Writers to the lists may
   add active nodes to previously inactive lists or, in the case of
   multiple readers, other readers may remove nodes causing previously 
   active lists to become inactive.

   Note that in single reader usage, the returned set will always be at
   least a subset of the active lists. Since no other reader is removing
   nodes, the active set can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set
ListsInterlocked::Lcb::getW (ListsInterlocked::Set any,
                             ListsInterlocked::Set all)
{
   /*
    | Interesting issue:
    |   Should this be done under the protection of a lock?
    |
    | Answer:
    |   No real reason to do this. Why? Because the set being returned
    |   can only be treated as a possible set of active lists. It is
    |   entirely possible that the set of active lists will be modified
    |   before the caller consumes this set. This can happen with both
    |   single and multiple readers of the lists. In both cases, the
    |   writer may add nodes to previously inactive lists, rendering the
    |   result returned by this routine invalid. In the multiple reader
    |   case, another reader may remove a node from a que causing it to
    |   go inactive.
    |
    | Conclusion
    |   The result returned from this routine must be treated as only
    |   a suggestion. Only by trying to remove a node from one of the
    |   suggested ques can one tell whether the list is empty or not.
   */
   ListsInterlocked::Set anyAll[2] = { any, all };
   ListsInterlocked::Set    active = test_anyAll (this, anyAll);
   if (active) return active;

   active = static_cast<ListsInterlocked::Set>
          ((uintptr_t)m_rw.getW (reinterpret_cast<ResourceWait::GetCb>(test_anyAll),
                                 this,
                                 (void *)anyAll));
   return active;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set 
          getW (volatile const ListsInterlocked::Set *wait_set,    
                const struct timespec                      *to)

  \brief  Returns a set of the currently active lists which, at the time
          of the call, is guaranteed to include at least one member of
          \a wait_set. The routine blocks until this condition is met
          or the timeout expires.

  \return The list of active ques. It is guaranteed to include at least
          one member in the wait list. Note that the list is not trimmed
          to the wait list. This would destroy information and the
          trimming is easily done by user

  \param[in]  wait_set  A pointer to the set of the lists to wait on. The wait
                        is treated as an OR.
  \param[in]  to        The timeout 


  \warning
   The caller is cautioned that the set returned only reflects the state
   of the active lists at the time of the call. Writers to the lists may
   add active nodes to previously inactive lists or, in the case of
   multiple readers, other readers may remove nodes causing previously 
   active lists to become inactive.

   Note that in single reader usage, the returned set will always be at
   least a subset of the active lists. Since no other reader is removing
   nodes, the active set can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set  
ListsInterlocked::Lcb::getW (volatile const ListsInterlocked::Set *wait_set,
                             const struct timespec                      *to)
{
   ListsInterlocked::Set active;

   /*
    | See the comments in PL__getW on why this is not evaluated under
    | the protection of a lock
   */
   active = test_or (this, wait_set);
   if (active || Timeout::isNoWait (to)) return active;


   active = static_cast<ListsInterlocked::Set>
          ((uintptr_t)m_rw.getW (reinterpret_cast<ResourceWait::GetCb>(test_or),
                                 this,
                                 (void *)wait_set,
                                 to));
   return active;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     ListsInterlocked::Set 
          getW (volatile const ListsInterlocked::Set *wait_set,    
                const struct timespec                      *to)

  \brief  Returns a set of the currently active lists which, at the time
          of the call, is guaranteed to include at least one member of
          \a wait_set. The routine blocks until this condition is met
          or the timeout expires.

  \return The list of active ques. It is guaranteed to include at least
          one member in the wait list. Note that the list is not trimmed
          to the wait list. This would destroy information and the
          trimming is easily done by user

  \param[in]  any       The set of any active lists
  \param[in]  all       The set of all active lists
                        is treated as an OR.
  \param[in]  to        The timeout 


  \warning
   The caller is cautioned that the set returned only reflects the state
   of the active lists at the time of the call. Writers to the lists may
   add active nodes to previously inactive lists or, in the case of
   multiple readers, other readers may remove nodes causing previously 
   active lists to become inactive.

   Note that in single reader usage, the returned set will always be at
   least a subset of the active lists. Since no other reader is removing
   nodes, the active set can only stay the same or get larger.
                                                                          */
/* ---------------------------------------------------------------------- */
ListsInterlocked::Set  
ListsInterlocked::Lcb::getW (ListsInterlocked::Set any,
                             ListsInterlocked::Set all,
                             const struct timespec *to)
{
   /*
    | See the comments in PL__getW on why this is not evaluated under
    | the protection of a lock
   */
   ListsInterlocked::Set anyAll[2] = { any, all };
   ListsInterlocked::Set    active = test_anyAll (this, anyAll);


   if (active || Timeout::isNoWait (to)) return active;


   active = static_cast<ListsInterlocked::Set>
          ((uintptr_t)m_rw.getW (reinterpret_cast<ResourceWait::GetCb>(test_anyAll),
                                 this,
                                 (void *)anyAll,
                                 to));
   return active;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

