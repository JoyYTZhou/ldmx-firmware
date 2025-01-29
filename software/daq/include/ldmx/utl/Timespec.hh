// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_TIMESPEC_H__
#define __LDMX_UTL_TIMESPEC_H__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Timespec.hh
  \brief  Creates methods for manipulating (adding, subtracting, etc) a
          standard timespec
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
#include <cstdio>

/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
class Timespec : public timespec
{
public:
   static constexpr uint32_t NanoSecondsInOneSecond = (1000*1000*1000);
   static constexpr uint32_t OneSecond              = (1000*1000*1000);


public:

   // Noop constructor
   Timespec ()  { return; }

   // Time = ts
   Timespec (Timespec const &ts);


   // Time = Current Time + nsecs
   Timespec (uint32_t     nsecs);

   
   // Time = base + nsecs/delta
   Timespec (Timespec const &base, uint32_t               nsecs);
   Timespec (Timespec const &base, struct timespec const &delta);


public:

   // Set to current time + 0/nsecs/delta
   Timespec &current ();
   Timespec &current (uint32_t               nsecs);
   Timespec &current (struct timespec const &delta);


   // Set to base + 0/nsecs/delta
   Timespec &set     (Timespec const &base, uint32_t nsecs);
   Timespec &set     (Timespec const &base, struct timespec const &delta);


   // Add nsecs/delta
   Timespec &add     (uint32_t              nsecs);
   Timespec &add     (struct timespec const &base);


   Timespec &operator +  (uint32_t      nsecs);
   Timespec &operator += (uint32_t      nsecs);
   Timespec &operator +  (struct timespec const &delta);
   Timespec &operator += (struct timespec const &delta);

   static int64_t elapsed (Timespec const &begin, Timespec const &end);
   int64_t    elapsedFrom (Timespec const &begin);
   int64_t    elapsedTo   (Timespec const   &end);
};
/* ---------------------------------------------------------------------- */
} /* namespace utl                                                        */
} /* namespace ldmx                                                       */
/* ====================================================================== */





/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief  Initializes to the specified \a ts

  \param[in]  ts  The initialization timespec
                                                                          */
/* ---------------------------------------------------------------------- */
template <clockid_t CLOCK_TYPE>
inline     Timespec<CLOCK_TYPE>::Timespec (Timespec<CLOCK_TYPE> const &ts) :
           timespec (ts)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Initializes to the current time plus \a nsecs

  \param[in] nsecs  The number of nanoseconds to add to the current time
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline Timespec<CLOCK_TYPE>::
       Timespec (uint32_t nsecs) :
       Timespec (current(), nsecs)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Initializes to the \base time plus \a nsecs

  \param[in]  base  The base time
  \param[in] nsecs  The number of nanoseconds to add to the current time
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline Timespec<CLOCK_TYPE>::
       Timespec (Timespec<CLOCK_TYPE> const &base, uint32_t nsecs) :
       Timespec (base)
{
   add (nsecs);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Initializes to the \base time plus \a delta

  \param[in]  base  The base time
  \param[in] delta  The time, specified as a struct timespec to add to
                    the current time
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline Timespec<CLOCK_TYPE>::
       Timespec (Timespec<CLOCK_TYPE> const  &base, 
                 struct timespec      const &delta) :
   Timespec (base)
{
   add (delta);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the time to the current time
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE>
         &Timespec<CLOCK_TYPE>::current ()
{
   clock_gettime (CLOCK_TYPE, this);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the time to the current time + \a nsecs

  \param[in] nsecs The time, in nanoseconds, to add to the current time
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::current (uint32_t nsecs)
{
   current ().add (nsecs);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the time to the current time + \a delta

  \param[in] delta The time, specified as a struct timespec, to add to 
                   the current time
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::current (struct timespec const &delta)
{
   current ().add (delta);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Adds \a nsecs to the time

  \param[in] nsecs The time, in nanoseconds, to add to
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::add (uint32_t nsecs)
{
   set (*this, nsecs);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Adds \a delta to the time

  \param[in] delta The time, specified as a struct timespec, to add to 
                                                                          */
/* ---------------------------------------------------------------------- */
template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::add (struct timespec const &delta)
{
   set (*this, delta);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the time to the \a base + \a delta

  \param[in]  base The base time
  \param[in] delta The time, specified as a struct timespec, to add to 
                   the current time
                                                                          */
/* ---------------------------------------------------------------------- */

template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::set (Timespec<CLOCK_TYPE> const  &base, 
                                     uint32_t                     nsecs)
{
   uint32_t  sec = base.tv_sec;
   uint32_t nsec = base.tv_nsec + nsecs;


   while (nsec >= NanoSecondsInOneSecond)
   {
      nsec -= NanoSecondsInOneSecond;
      sec  += 1;
   }

   tv_sec  =  sec;
   tv_nsec = nsec;

   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the time to the \a base + \a delta

  \param[in]  base The base time
  \param[in] delta The time, specified as a struct timespec, to add to 
                   the current time
                                                                          */
/* ---------------------------------------------------------------------- */

template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::set (Timespec<CLOCK_TYPE> const  &base, 
                                     struct timespec     const  &delta)
{
   uint32_t  sec = base.tv_sec  + delta.tv_sec;
   uint32_t nsec = base.tv_nsec + delta.tv_nsec;


   if (nsec >= NanoSecondsInOneSecond)
   {
      nsec -= NanoSecondsInOneSecond;
      sec  += 1;
   }

   tv_sec  =  sec;
   tv_nsec = nsec;

   return *this;
}
/* ---------------------------------------------------------------------- */


template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::operator + (uint32_t nsecs)
{
   return add (nsecs);
}



template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE>
         &Timespec<CLOCK_TYPE>::operator += (uint32_t nsecs)
{
   return add (nsecs);
}


template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE> 
         &Timespec<CLOCK_TYPE>::operator + (struct timespec const &delta)
{
   return add (delta);
}


template<clockid_t CLOCK_TYPE>
inline    Timespec<CLOCK_TYPE>
         &Timespec<CLOCK_TYPE>::operator += (struct timespec const &delta)
{
   return add (delta);
}




template<clockid_t CLOCK_TYPE>
inline int64_t Timespec<CLOCK_TYPE>::elapsed (Timespec<CLOCK_TYPE> const &begin, 
                                              Timespec<CLOCK_TYPE> const &end)
{
   uint64_t beginning = NanoSecondsInOneSecond * begin.tv_sec + begin.tv_nsec;
   uint64_t    ending = NanoSecondsInOneSecond *   end.tv_sec +   end.tv_nsec;

   return ending - beginning;
}



template<clockid_t CLOCK_TYPE>
int64_t   Timespec<CLOCK_TYPE>::elapsedFrom (Timespec<CLOCK_TYPE> const &begin)
{
   uint64_t delta = elapsed (begin, *this);
   return   delta;
}


template<clockid_t CLOCK_TYPE>
int64_t   Timespec<CLOCK_TYPE>::elapsedTo (Timespec<CLOCK_TYPE> const &end)
{
   uint64_t delta = elapsed (*this, end);
   return   delta;
}
/* ---------------------------------------------------------------------- */
} /* namespace utl                                                        */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif

