// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_FRAGMENT_BUILDER_HH__
#define __LDMX_BUILDER_CLIENT_FRAGMENT_BUILDER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Builder.hh
  \brief  The aggregation of data contributions (\e e.g. Trigger and SVT)
          into a coherent class
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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Fragment.hh"
#include "ldmx/utl/List.hh"
#include "ldmx/utl/ListInterlocked.hh"
#include "ldmx/utl/Timestamp.hh"

#include <thread>

#include <ldmx/utl/Thread.hh>

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
/* <none>                                                                 */
/* ====================================================================== */


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
class Message : public ldmx::utl::ListNode
{
public:
   enum class Type
   {
      Noop   = 0,
      Enable = 1,
   };

public:
   Message (enum Type type, void *prm);


public:
   uint32_t m_tid;  /*!< The transaction id   */
   Type    m_type;  /*!< The message type     */
   void    *m_prm;  /*!< The thread parameter */
};
/* ---------------------------------------------------------------------- */ 



/* ---------------------------------------------------------------------- */ 
class MessagePasser
{
public:
   uint32_t post (Message *msg);
   Message *pend ();

   void     ack  (Message *msg);
   Message *wait ();

public:
   ldmx::utl::ListInterlocked  m_in;
   ldmx::utl::ListInterlocked m_ack;
   uint32_t        m_tid;  
};
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Typedef for the list of fragments
                                                                          */
/* ---------------------------------------------------------------------- */
typedef ldmx::utl::List  FragmentList;
/* ---------------------------------------------------------------------- */

class Contributions;
class Fragment;


/* ---------------------------------------------------------------------- *//*!

  \brief The Fragment Builder
                                                                          */
/* ---------------------------------------------------------------------- */
class Builder : public FragmentList
{
public:
   Builder ();
   Builder (uint32_t   expected, 
            int      nfragments,
            Contributions *ctbs);

   enum Status
   {
      Keep      = -1, /*!< Contribution did not match any active fragment */
      TooOld    = -2, /*!< Fragment is too old, fragment already promoted */
      Duplicate = -3  /*!< Contribution already is a member of a fragment */
   };


   virtual int post (Fragment *fragment);
   int poster (Fragment *fragment);

public:
   static void  assembler (Builder *builder);

   int             launch ();
   bool            enable ();
   int             build  (ldmx::utl::List *lists,
                           uint32_t      required, 
                           uint64_t  receive_time);
   void            create (int seqNum);

   uint32_t       promote (uint64_t     timestamp,
                           uint32_t      sequence,
                           uint32_t      expected,
                           uint32_t          seed,
                           ldmx::utl::List *lists);

private:
   static uint32_t remove (List *lists, uint32_t set);
   static void  removeAll (List *lists, uint32_t set);

public:
   bool            m_new;  /*!< Build is brand new, i.e. no history       */
   Pool           m_pool;  /*!< The Fragment memory pool                  */
   uint32_t     m_oldest;  /*!< The sequence number of the oldest fragment*/
   uint32_t   m_expected;  /*!< The list of expected contribuors          */
   uint32_t    m_pending;  /*!< Bit mask of pending contributors.  These
                                are contributions with sequence numbers
                                larger than 1 past the newest             */
   uint64_t  m_timestamp;  /*!< Timestamp of any pending fragment         */
   uint64_t    m_timeout;  /*!< Timeout value for use on the next fragment*/

   Contributions *m_ctbs;

   Fragment  *m_fragments[32]; /*!< Fragment to start the scan by 
                                    contributor                           */
   ldmx::utl::ListInterlocked
               m_postList; /*!< The posting queue                         */

   ldmx::utl::Thread
   //std::thread 
             m_assembler;  /*!< The fragment assembler thread             */
   MessagePasser    m_ctl; /*!< The controlling message passer            */
};
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENATION                                                          */
/* ---------------------------------------------------------------------- *//*!

   \brief Dummy constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline Builder::Builder ()
{
   return;
}
/* ====================================================================== */


inline Message::Message (enum Type type, void *prm) :
   m_type (type),
   m_prm  ( prm)
{
   return;
}


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief  Posts a message
  \return The transaction id
  
  \param[in] msg  The message to post
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t MessagePasser::post (Message *msg)
{
   uint32_t tid = m_tid++;
   msg->m_tid   = tid;
   m_in.insert (msg);
   return tid;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Pends, waiting for an incoming message
  \return The received message
                                                                          */
/* ---------------------------------------------------------------------- */
inline Message *MessagePasser::pend ()
{
   Message *msg = reinterpret_cast<decltype(msg)>(m_in.removeW ());
   return   msg;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Acknowledges the message
  
  \param[in] msg  The message to acknowledge
                                                                          */
/* ---------------------------------------------------------------------- */
inline void MessagePasser::ack (Message *msg)
{
   m_ack.insert (msg);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Waits for the return of an acknowledged message
  \return  The acknowledged message
                                                                          */
/* ---------------------------------------------------------------------- */
inline Message *MessagePasser::wait ()
{
   Message *msg = reinterpret_cast<decltype(msg)>(m_ack.removeW ());
   return   msg;
}
/*====================================================================== */
#endif                
