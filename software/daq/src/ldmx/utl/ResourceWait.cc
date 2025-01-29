// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/ResourceWait.cc
  \brief  Implements methods to wait on a resource to become available
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


#include "ldmx/utl/ResourceWait.hh"
#include <errno.h>

/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \fn     ResourceWait ()
  \brief  ResourceWait constructor

  \return A POSIX status 

   Initializes the locking/blocking mechanism for a resource. This
   mechanism is platform dependent. One must be careful to distinguish
   locking the resource for manipulation, such allocating and freeing,
   and pending when an attempt is made to block the thread waiting to
   acquire a currently exhausted resource.

   One some platforms the locking and pending work as a tandem. On POSIX
   platforms, a condition variable is used in conjunction with a POSIX
   mutex to implement the pend.

                                                                          */
/* ---------------------------------------------------------------------- */
ResourceWait::ResourceWait ()
{
   int status;
 
   // This sets the wait control to enabled with no one waiting
   m_waiting.si = 0;


   /* 
    | Since POSIX condition initialization involves resource allocation
    | beyond memory, just initialize the condition variable independent
    | of whether pending is initially requested or not
    */


   /*
    | Initialize an attribute to default value, 
    | then change to use the MONOTONIC clock
   */
   pthread_condattr_t cattr;
   if ((status = pthread_condattr_init    (&cattr)))                  return;

   if ((status = pthread_condattr_setclock(&cattr, CLOCK_MONOTONIC))) return;

   if ((status = pthread_cond_init        (&m_cond, &cattr)))         return;


   // Always need a mutex


   if ((status = pthread_mutex_init (&m_mutex, NULL))) return;

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn    ~ResourceWait ()
  \brief  The ResourceWait destructor

   Releases the resources associated with the lock. It is the responsibility
   of the user to make sure the list is inactive and stays inactive during
   this procedure. 

   The reasons for failure are
         a) Corrupted mutex or condition variable handle
         b) Mutex is still locked.
         c) Relating to the condition variable, the POSIX manual says
            "The object has been initialized before, and is not destroyed."

   Quite frankly, I have no idea what 'c' means. The only real error that
   the user must deal with is the mutex is still locked. This is a programmer
   error. He has not made sure that the resource RW is controlling is quiet.

                                                                          */
/* ---------------------------------------------------------------------- */
ResourceWait::~ResourceWait ()
{
   int status;

   
   // If anyone waiting, don't destroy
   if (m_waiting.bf.cnt) return;

   
   // A condition variable is always allocated with the RW, destroy it
   if ((status = pthread_cond_destroy (&m_cond))) return;
   

   // A mutex is always allocated with the RW, destroy it
   pthread_mutex_destroy (&m_mutex);


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn              int   free (ResourceWait:FreeCb free,
                               void                *ctx,
                               void           *resource,
                               void             *amount)

  \brief           Frees a resource, potentially waking up a task pending
                   on an RW_get().

  \return         The return value of the free callback routine.

 \param[in] free       User provide callback routine to implement the
                       actual freeing of the resource. The function
                       signature is

                       \code
                         (*free)(void *ctx, void *resource, void *amount);
                       \endcode

                       where typically \a ctx provides context to the free
                       routine, the \a resource is a pointer to the resource
                       being freed, and \a amount is the amount of the
                       resource being freed.

  \param[in] ctx      Context parameter to the free routine
  \param[in] resource The resource being freed.
  \param[in[ amount   Typically the amount being freed.


   Note that while the three parameters to the free routine are given
   suggestive names, they are, in fact, just void *'s, so the user is
   free to pass any 64-bit value that he wishes. 
                                                                          */
/* ---------------------------------------------------------------------- */
int ResourceWait::free (ResourceWait::FreeCb free, 
                        void                 *ctx,
                        void            *resource,
                        void              *amount) 
{
   int status;

   lock ();
   {
      status = (*free) (ctx, resource, amount);
      wake ();
   }
   unlock ();

   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn           void *get (ResourceWait::GetCb get,
                           void               *ctx,
                           void            *amount)

  \brief        Attempts to allocate a resource using the provided
                get routine. Whether the resource is available or
                not, this routine returns immediately with the
                return value of the get routine. See RW_getW() and
                RW_getW_toc() for versions that implement waiting.
                If the resource is disabled, the routine returns NULL.

  \return       A pointer to the garnered resource or NULL if the
                resource is unavailable or the resource is disabled.


  \param[in] get    A user provide 'get' routine. The function signature is

                    \code
                     void *resource = (*get)(void *ctx, void *amount);
                    \endcode   

  \param[in] ctx    A context parameter passed to the \a get routine.
  \param[in] amount The amount of the resource being requested.


   This routine locks access to the resource while the get is being
   performed.

   While the names of the two parameters to the \a get routine are
   suggestive, they are, in fact, just two void *'s, so the get routine
   may make any use of them it pleases.
                                                                          */
/* ---------------------------------------------------------------------- */  
void *ResourceWait::get (ResourceWait::GetCb get,
                         void               *ctx,
                         void            *amount)
{
   void *resource;
 
   /* Only attempt to deque is enabled */
   if (m_waiting.bf.dsb == 0)
   {
      lock   ();
      resource = (*get)(ctx, amount);
      unlock ();
   }
   else
   {
      resource = NULL;
   }
   

   return resource;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \fn         void *getW (ResourceWait::GetCb get,
                           void               *ctx,
                           void            *amount)

  \brief        Attempts to allocate a resource using the provided
                get routine. This routine blocks until the get
                routine returns a non-NULL value. If nowaiting is
                desired, see RW_get(). If a waiting with a timeout
                is desired, see RW_getW_toc().

  \return       A pointer to the garnered resource

  \param[in] get     A user provide 'get' routine. The function signature is

                     \code
                      void *resource = (*get)(void *ctx, void *amount);
                      \endcode   

  \param[in] ctx    A context parameter passed to the \a get routine.
  \param[in] amount The amount of the resource being requested.


   This routine locks access to the resource pool while the get is being
   performed and waits until the resource is available. An equivalent
   call to RW_free() will unblock this routine, and another attempt will
   be made to allocate the resource. If successful, as indicated by the
   get routine returning a non-NULL value, the resource will be returned
   to the caller. If unsuccessful, the routine will block again.

   While the names of the two parameters to the \a get routine are
   suggestive, they are, in fact, just two void *'s, so the get routine
   may make any use of them it pleases.
                                                                          */
/* ---------------------------------------------------------------------- */  
void *ResourceWait::getW (ResourceWait::GetCb get, 
                          void               *ctx, 
                          void            *amount)
{
   void *resource;

   lock ();

   while (1)
   {
       /* Attempt to allocate, iff the resource is enabled */
      if (m_waiting.bf.dsb == 0)
      {
         resource = (*get)(ctx, amount);
         if (resource) break;
      }

      wait ();
   }

   unlock ();

   return resource;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \fn         void *getW (ResourceWait:GetCb    get,
                           void                 *ctx,
                           void              *amount,
                           struct timespec const *to)

  \brief        Attempts to allocate a resource using the provided
                get routine. This routine blocks until the get
                routine returns a non-NULL value or until the timeout
                period expires. If nowaiting is desired, see RW_get().
                If a waiting with no timeout is desired, see RW_getW_toc().

  \return       A pointer to the garnered resource, in reality whatever
                non-NULL value the user provided get routine returns.


  \param[in] get    A user provide 'get' routine. The function signature is

                     \code
                       void *resource = (*get)(void *ctx, void *amount);
                    \endcode   

  \param[in] ctx    A context parameter passed to the \a get routine.
  \param[in] amount The amount of the resource being requested.
  \param[in] toc    The timeout control structure. 
                    TOC_NOWAIT and TOC_FOREVER may be specified.

   This routine locks access to the resource pool while the get is being
   performed and waits until the resource is available. An equivalent
   call to RW_free() will unblock this routine, and another attempt will
   be made to allocate the resource. If successful, as indicated by the
   get routine returning a non-NULL value, the resource will be returned
   to the caller. If unsuccessful, the routine will block again until
   the timeout period expires or the resource becomes available.

   While the names of the two parameters to the \a get routine are
   suggestive, they are, in fact, just two void *'s, so the get routine
   may make any use of them it pleases.

  \warning 
   Also note that timeouts are not disabled even if the RW has been
   disabled. In other words, if the timeout period expires during the
   time the RW is disabled, NULL is returned. This leaves the caller
   in somewhat of a quandry, did the routine timeout because the
   resource was exhausted or because it was disabled. This routine
   cannot determine this fact any better than the caller. If necessary,
   the caller has two tools at his disposal for resolving this issue,
   he can poll his own resource to see if it is exhausted and he can
   query the RW as to its enabled/disabled state. Both these methods have
   the same problem, \e i.e. the state can at any time. The issue of
   why a routine times out is largely philosophical for the same reason.
   It may have been that a wait was initially caused by a lack of
   resources, but before the resource was refreashed, the controlling
   RW was diabled.
                                                                          */
/* ---------------------------------------------------------------------- */  
void *ResourceWait::getW (ResourceWait::GetCb    get,
                          void                  *ctx,
                          void               *amount,
                          struct timespec const  *to)
{
   void *resource = NULL;

   lock ();
   {
      while (1)
      {
         /* Attempt to allocate, iff the resource is enabled */
         if (m_waiting.bf.dsb == 0)
         {
            resource = (*get)(ctx, amount);
            if (resource) break;
         }
       
         int status = wait (to);

         /*
          | !!! DESIGN DECISION !!!
          | -----------------------
          | Note that a disabled que will still return after a timeout.
          | This is a design decision. It seems the user would want to
          | be notified independent of why the resource did not appear.
          | He can always check the state of the resource disable when
          | the routine returns and modify his behaviour accordingly.
         */
         if (status == ETIMEDOUT) break;
      }
   }

   unlock  ();

   return resource;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

