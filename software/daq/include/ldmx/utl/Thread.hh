// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_THREAD_HH__
#define __LDMX_UTL_THREAD_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/List.hh
  \brief  Derived thread class to start a thread in a block state
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
 * 2019.04.02 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/utl/ListInterlocked.hh"

#include <thread>
#include <new>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
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
   uint32_t   m_tid;  /*!< The transaction id   */
   Type      m_type;  /*!< The message type     */
   void      *m_prm;  /*!< The thread parameter */
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

   void    *block ();

public:
   ldmx::utl::ListInterlocked  m_in;
   ldmx::utl::ListInterlocked m_ack;
   uint32_t        m_tid;  
};


class Thread 
{
public:
   Thread ()
   {
      return;
   }

public:
   template<typename Function, typename Arg>
   void  launch (Function&& fn, Arg args);
   bool  enable ();
   void *block  ();


public:
   std::thread   m_thread;
   MessagePasser m_ctl;
};
/* ---------------------------------------------------------------------- */
} /* namespace ldmx                                                       */
} /* namespace utl                                                        */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- */
inline Message::Message (enum Type type, void *prm) :
   m_type (type),
   m_prm  ( prm)
{
   return;
}

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



/* ---------------------------------------------------------------------- *//*!

  \brief Creates the thread, but in a wait state
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename Function, typename Arg>
inline void Thread::launch (Function&& fn, Arg args)
{
   new (&m_thread) std::thread (fn, args);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Enable (unblocks) the thread
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool Thread::enable ()
{
   Message  sndMsg (Message::Type::Enable, this);
   uint32_t    tid = m_ctl.post (&sndMsg);
   Message *retMsg = m_ctl.wait ();

   if (tid != retMsg->m_tid)
   {
      /// fprintf (stderr,
      ///         "Gateway::go Mismatch start transaction ids ="
      ///         " %8.8" PRIx32 " : %8.8" PRIx32 "\n", tid, retMsg->m_tid);
      return false;
   }

   return true;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Waits for an enable message, blocking the execution of the thread
         until ready

  \param[in:out] ctl  The message passing control structure
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *Thread::block ()
{ 
   MessagePasser *ctl = &m_ctl;

   while (1)
   {
      ///puts ("Thread::block Waiting for start message");
      ///fflush (stdout);


      Message *msg = ctl->pend ();
      //printf ("Gateway::assembler Received message type = %d\n",
      //        static_cast<int>(msg->m_type));
      //fflush (stdout);

      Message::Type rcvType = msg->m_type;


      ///printf ("Thread::blocK Sending acknowledgement\n");

      ctl->ack (msg);

      ///printf ("Thread::block Sent    acknowledgement\n");

      if (rcvType == Message::Type::Enable)
      {
         void   *prm = msg->m_prm;
         return prm;
      }

   }
}
/* ---------------------------------------------------------------------- */
} /* namespace ldmx                                                       */
} /* namespace utl                                                        */
/* ====================================================================== */


#endif
