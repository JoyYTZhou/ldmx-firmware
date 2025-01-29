// -*-Mode: C++;-*-

#ifndef _LDMX_UTL_RESOURCE_WAIT_HH_
#define _LDMX_UTL_RESOURCE_WAIT_HH_

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/ResourceWait.hh
  \brief  Controls locking and waiting for a resource
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
 * 2019.01.16 jjr Cloned from EXO versions, converted to C++
 *
\* ---------------------------------------------------------------------- */



#include <pthread.h>
#include <stdio.h>




/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \union  ResourceWaitCtl_bf
  \brief  Structure giving the wait control word mapped up as a bit field
          structure
                                                                          *//*!
   The low 31 bits of the wait control word are used as a count of the
   number of pending tasks. The sign bit is used as an enable/disable
   flag. If the flag is disabled (set), RW_wake will not generate a signal
   to wake up any pending tasks.
                                                                          */
/* ---------------------------------------------------------------------- */
struct ResourceWaitCtl_bf
{
   unsigned int cnt:31;  /*!< The low 31 bits are a count of the
                              number of outstandings waits                */
   unsigned int dsb:1;   /*!< The sign bit is a enable(0)/disable(1)      */
};
/* ---------------------------------------------------------------------- */
       


/* ---------------------------------------------------------------------- *//*!

  \union  ResourceWaitCtl_u
  \brief  Union giving the wait control word as a simple unsigned int
          and mapped up as a bit field structure
                                                                          *//*!
                                                                          */
/* ---------------------------------------------------------------------- */
union ResourceWaitCtl_u
{
    ResourceWaitCtl_bf bf;    /*!< The wait control as bit fields         */
    signed int         si;    /*!< The wait control as a signed int       */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class  ResourceWait
  \brief   POSIX platform dependent version of a Resource Wait 
           control block
                                                                          *//*!
  \typedef RW_ctl
  \brief   Typedef for the struct _RW_ctl

   This data structure provides supports both locking and blocking of
   a resource. On the POSIX, a mutex is used to lock the resource. This
   The blocking is implemented with a standard semaphore and condition
   variable.
                                                                          */
/* ---------------------------------------------------------------------- */
class ResourceWait
{
public:
   ResourceWait ();
  ~ResourceWait ();

public:
   /* ------------------------------------------------------------------ *//*!

     \typedef  GetCb
      \brief   Function signature for the get callback routine. Used by
               ResourceWait::getW() with and without timeout.
                                                                          */
   /* ------------------------------------------------------------------- */
   typedef void *(*GetCb) (void *hndl, void *prm);
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \typedef  FreeCb
     \brief    Function signature for the free callback routine. Used by
               ResourceWait::free().
                                                                          */
   /* ------------------------------------------------------------------- */
   typedef int (*FreeCb)(void *ctx, void *resource, void *prm);
   /* ------------------------------------------------------------------- */

public:
   int      lock ();
   int    unlock ();
   int      wait ();
   int      wait (struct timespec const *abs_timeout);
   int      wait (struct timespec const &abs_timeout);
   int      wake ();
   int   disable ();
   int    enable ();
   int isEnabled () const;

   int     free (ResourceWait::FreeCb       free,
                 void                       *ctx,
                 void                  *resource,
                 void                    *amount);

   void     *get (ResourceWait::GetCb       get,
                  void                      *ctx,
                  void                   *amount);

   void    *getW (ResourceWait::GetCb        get,
                  void                      *ctx,
                  void                   *amount);

   void    *getW (ResourceWait::GetCb        get,
                  void                      *ctx,
                  void                   *amount,
                  struct timespec const *timeout);


   pthread_mutex_t m_mutex; /*!< The locking mutex                        */
   pthread_cond_t   m_cond; /*!< The condition variable signaller         */
   volatile ResourceWaitCtl_u
                 m_waiting; /*!< Number of customers waiting /enable flag */

};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn         ResourceWait::Key lock ()
  \brief      Locks a resource so it can be manipulated in an interlocked
              fashion.
  \return     Status
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::lock ()  
{
   int    status = pthread_mutex_lock (&m_mutex);
   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn         int unlock ()
  \brief      This routine unlocks a previously locked resource
  \return     Status 
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::unlock ()
{
   int    status = pthread_mutex_unlock (&m_mutex);
   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn          int wait ()
  \brief       Blocks a task/thread until there is a reasonable chance
               that a resource is available.
  \return      Status

  \par
   Blocks a task until there is a chance that resource becomes available.
   This routine must be called with the resource locked. The routine 
   manipulates some internal structures, unlocks the resource,  blocks 
   waiting for a wake-up signal (RW__wake) and then relocks the  resource
   in anticipation of the caller attempting to reallocate the  resource.

                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::wait ()
{
    m_waiting.bf.cnt += 1;
    int    status = pthread_cond_wait (&m_cond, &m_mutex);
    return status;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn          int wait (struct timespec const *abs_to)
  \brief       Blocks a task/thread until there is a reasonable chance
               that a resource is available.
  \return      Status, the most interesting in ETIMEDOUT

  \param[in]   to  The timeout value specified as a struct timespec. 
                   The timespec is an absolute time. 

  \par
   Blocks a task until there is a chance that resource becomes available.
   This routine must be called with the resource locked. The routine 
   manipulates some internal structures, unlocks the resource,  blocks 
   waiting for a wake-up signal (wake) and then relocks the  resource
   in anticipation of the caller attempting to reallocate the  resource.

  \note
   The timeout is specified as an absolute time.  The default is to use
   clock_gettime specifying CLOCK_REALTIME, but since this is used almost
   exclusively as a timeout, the default has been changed in the 
   initialization of the condition variable to be CLOCK_MONOTONIC.

   Although nothing prevents directly calling clock_gettime with 
   CLOCK_MONOTONIC, using the ldmx::utl::Timestamp methods will ensure
   this.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::wait (struct timespec const *abs_to)
{
   m_waiting.bf.cnt += 1;
   printf ("ResourceWait: Abs timeout = %6ld.%09ld\n", 
           abs_to->tv_sec, abs_to->tv_nsec);
   int status = pthread_cond_timedwait (&m_cond, &m_mutex, abs_to);
   return status;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \fn          int wait (struct timespec const &abs_to)
  \brief       Blocks a task/thread until there is a reasonable chance
               that a resource is available.
  \return      Status, the most interesting in ETIMEDOUT

  \param[in]   to  The timeout value specified as a struct timespec. 
                   The timespec is an absolute time. 

  \par
   Blocks a task until there is a chance that resource becomes available.
   This routine must be called with the resource locked. The routine 
   manipulates some internal structures, unlocks the resource,  blocks 
   waiting for a wake-up signal (wake) and then relocks the  resource
   in anticipation of the caller attempting to reallocate the  resource.

  \note
   The timeout is specified as an absolute time.  The default is to use
   clock_gettime specifying CLOCK_REALTIME, but since this is used almost
   exclusively as a timeout, the default has been changed in the 
   initialization of the condition variable to be CLOCK_MONOTONIC.

   Although nothing prevents directly calling clock_gettime with 
   CLOCK_MONOTONIC, using the ldmx::utl::Timestamp methods will ensure
   this.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::wait (struct timespec const &abs_to)
{
   int    status = wait (&abs_to);
   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn          int wake ()
  \brief       This provides the signaling mechanism to wake up a
               thread of code which is waiting for a resource.
  \retval Status


   This routine checks if any thread is waiting for a resource, and if so
   wakes that thread. The resource should be locked on entry to this
   routine. This routine always exits with the resource still locked.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::wake ()
{
   /* Is there anyone waiting for something to deque */
   ResourceWaitCtl_u waiting;
   waiting.si = m_waiting.si;

   /* 
    | For waiting.si to be > 0, both
    |     1. Disable flag (in the sign bit) must be 0
    |     2. The count field must be non-zero
   */
   if (m_waiting.si > 0)
   {
       /* Yes, decrease the number waiting by 1 */
       m_waiting.bf.cnt = waiting.bf.cnt - 1;

       /* And wake them up */
       return pthread_cond_signal (&m_cond);
   }

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn     int disable ()
  \brief  Disables wakeup calls.

  \retval      0 RW was previously disabled
  \retval    !=0 RW was previously  enabled

   Wakeup calls are disabled by this call.  Effectively wakeup calls issued
   during the time the RW control structure is disabled are pended until
   reenabled by calling \e RW__enable().
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::disable ()
{
   int disabled;

   lock ();
   {
      ResourceWaitCtl_u waiting;

      waiting.si     = m_waiting.si;
      disabled       = waiting.bf.dsb;
      waiting.bf.dsb = 1;
      m_waiting.si   = m_waiting.si;
   }
   unlock ();

   return !disabled;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     int enable ()
  \brief  Enables wakeup calls.
  \retval      0 RW was previously disabled
  \retval    !=0 RW was previously  enabled


   Wakeup calls are enabled by this call.  The routine also issues a wakeup
   if there are any pending wakeup calls.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::enable ()
{
   int disabled;

   lock ();
   {
      ResourceWaitCtl_u waiting;

      waiting.si     = m_waiting.si;
      disabled       = waiting.bf.dsb;
      waiting.bf.dsb = 0;
      m_waiting.si   = waiting.si;

      // Attempt to wake up anyone waiting during the disable time
      wake   ();
   }
   unlock ();

   return !disabled;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     int isEnabled () const
  \brief  Returns the state of the enabled flag
  \retval      0 RW was previously disabled
  \retval    !=0 RW was previously  enabled

  \warning
   As with all queries of this type, the answer is only good until it
   changes. The user is cautioned on its usage. It is primarily provided
   as a debugging tool.
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ResourceWait::isEnabled () const
{
   return !m_waiting.bf.dsb;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

#endif
