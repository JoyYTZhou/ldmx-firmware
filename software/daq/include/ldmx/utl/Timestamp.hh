// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_TIMESTAMP_HH__
#define __LDMX_UTL_TIMESTAMP_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Timestamp.hh
  \brief  Manages 64-bit timestamp expressed in nanoseconds
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory  of this distribution and at: 

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
 * 2019.01.32 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <time.h>
#include <cstdint>


/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief Defines a set of simple timestamp manipulation methods
                                                                          */
/* ---------------------------------------------------------------------- */
class Timestamp
{
public:
   Timestamp ();
   Timestamp (struct timespec const &ts);

public:
   static const int32_t NanoSecondsPerSecond = (1000 * 1000 * 1000);
   static const int32_t OneSecond            = (1000 * 1000 * 1000);

public:
   static uint64_t timestamp (struct timespec const *ts);
   static uint64_t timestamp (struct timespec const &ts);

   static uint64_t   current ();
   static void       current (struct timespec         *ts);


   static void       timeout (struct timespec    *timeout, 
                              uint32_t                 to);

   static void       timeout (struct timespec    *timeout, 
                              struct timespec const &base,
                              uint32_t                 to);


   static void       timeout (struct timespec    *timeout, 
                              struct timespec const   &to);


   static void       timeout (struct timespec    *timeout, 
                              struct timespec const &base,
                              struct timespec const   &to);


   int64_t elapsed ()                          const;
   int64_t elapsed (Timestamp timestamp)       const;
   int64_t elapsed (struct timespec const *ts) const;
   int64_t elapsed (struct timespec const &ts) const;

protected:
   uint64_t m_timestamp;
};
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */




/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief  Convert the specified timespec to a 64-bit time in nanoseconds
  \return The timespec in nanoseconds
  
  \param[in] ts  Reference to a struct timespec

                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t Timestamp::timestamp (struct timespec const &ts)
{
   uint64_t time = ts.tv_sec;
   time *= NanoSecondsPerSecond;
   time += ts.tv_nsec;
   return  time;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Convert the specified timespec to a 64-bit time in nanoseconds
  \return The timespec in nanoseconds
  
  \param[in] ts  Pointer to a struct timespec

                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t Timestamp::timestamp (struct timespec const *ts)
{
   uint64_t time = timestamp (*ts);
   return  time;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the current time as a 64-bit timestamp
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t Timestamp::current ()
{
   struct timespec ts;
   clock_gettime (CLOCK_MONOTONIC, &ts);

   uint64_t time = timestamp (ts);
   return   time;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Get the current time as a struct timespec

   \param[out] ts  The struct timespec to be returned with the current
                   time
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Timestamp::current (struct timespec *ts)
{
   clock_gettime (CLOCK_MONOTONIC, ts);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \brief  From the absolute timeout from the current time + the specified
          timeout value

  \param[out] abs_timeout The absolute timeout
  \param[in]  to_nsecs    The relative timeout value, in nanoseconds.
                          This will limited ragned will satisfy most
                          needs, with the timeout capped at ~4 seconds.
                                                                          */ 
/* ---------------------------------------------------------------------- */
inline void Timestamp::timeout (struct timespec *abs_timeout, 
                                uint32_t            to_nsecs)
{
   // Get the current time
   current (abs_timeout);

   // Add on the timeout value
   uint32_t nsec = abs_timeout->tv_nsec + to_nsecs;
   

   // Normalize the nanoseconds
   while (nsec >= NanoSecondsPerSecond) 
   {
      nsec -= NanoSecondsPerSecond;
      abs_timeout->tv_sec += 1;
   }


   // Return the timeout as an absolute time
   abs_timeout->tv_nsec = nsec;

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Calculate the absolute timeout relative to a base time

  \param[out] abs_timeout The calculate absolute timeout
  \param[in]         base The base time for computing the timeout
  \param[ in]    to_nsecs The relative timeout, in nanoseconds.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Timestamp::timeout (struct timespec *abs_timeout, 
                                struct timespec const  &base,
                                uint32_t            to_nsecs)
{
   // Add on the timeout value
   uint32_t nsec = base.tv_nsec + to_nsecs;
   uint32_t  sec = base.tv_sec;
   

   // Normalize the nanoseconds
   while (nsec >= NanoSecondsPerSecond) 
   {
      nsec -= NanoSecondsPerSecond;
      sec  += 1;
   }


   // Return the timeout as an absolute time
   abs_timeout->tv_sec  =  sec;
   abs_timeout->tv_nsec = nsec;
   

   return;
}
/* ---------------------------------------------------------------------- */
   


/* ---------------------------------------------------------------------- *//*!
   
  \brief  From the absolute timeout from the current time + the specified
          timeout value

  \param[out] abs_timeout The absolute timeout
  \param[in]  to          The relative timeout value, as a struct timespec

                                                                          */ 
/* ---------------------------------------------------------------------- */
inline void Timestamp::timeout (struct timespec  *abs_timeout, 
                                struct timespec const     &to)
{
   // Get the current time
   current (abs_timeout);


   // Add on the timeout value
   uint32_t nsec = abs_timeout->tv_nsec + to.tv_nsec;
   uint32_t  sec = abs_timeout->tv_sec  + to.tv_sec;


   // Normalize the nanoseconds   
   while (nsec >= NanoSecondsPerSecond) 
   {
      nsec -= NanoSecondsPerSecond;
      sec  += 1;
   }

   // Return the timeout as an absolute time
   abs_timeout->tv_sec  =  sec;
   abs_timeout->tv_nsec = nsec;

   return;
}
/* ---------------------------------------------------------------------- */



   


/* ---------------------------------------------------------------------- *//*!
   
  \brief  From the absolute timeout from the current time + the specified
          timeout value

  \param[out] abs_timeout The absolute timeout
  \param[in]         base The base time for computing the timeout
  \param[in]           to The relative timeout value, as a timespec

                                                                          */ 
/* ---------------------------------------------------------------------- */
inline void Timestamp::timeout (struct timespec  *abs_timeout, 
                                struct timespec const   &base,
                                struct timespec const     &to)
{
   // Add on the timeout value
   uint32_t nsec = base.tv_nsec + to.tv_nsec;
   uint32_t  sec = base.tv_sec  + to.tv_sec;
   

   // Normalize the nanoseconds
   while (nsec >= NanoSecondsPerSecond) 
   {
      nsec -= NanoSecondsPerSecond;
      sec  += 1;
   }

   abs_timeout->tv_sec  =  sec;
   abs_timeout->tv_nsec = nsec;

   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Defines a set of simple timestamp manipulation methods
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timestamp::Timestamp () :
   m_timestamp (current ())
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Get the elapsed time, using this class as the start time
  \return  The elapsed time

  \param[in]  end_timestamp The ending time, expressed as a Timestamp
                                                                          */
/* ---------------------------------------------------------------------- */ 
inline int64_t Timestamp::elapsed (Timestamp end_timestamp) const
{
   return end_timestamp.m_timestamp - m_timestamp;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Get the elapsed time, using this class as the start time
  \return  The elapsed time

  \param[in]  end_timespec The ending time, expressed as a struct timespec
                                                                          */
/* ---------------------------------------------------------------------- */ 
inline int64_t Timestamp::elapsed (struct timespec const *end_timespec) const
{
   return elapsed (*end_timespec);
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Get the elapsed time, using this class as the start time
  \return  The elapsed time

  \param[in]  end_timespec The ending time, expressed as a struct timespec
                                                                          */
/* ---------------------------------------------------------------------- */ 
inline int64_t Timestamp::elapsed (struct timespec const &end_timespec) const
{
   int64_t time = timestamp (end_timespec);
   return  m_timestamp - time;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Get the elapsed time, using this class as the start time and
           the current time as the stop time
  \return  The elapsed time
                                                                          */
/* ---------------------------------------------------------------------- */
inline int64_t Timestamp::elapsed () const
{
   int64_t time = current ();
   return  m_timestamp - time;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */

#endif
