// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_TIMEOUT_HH__
#define __LDMX_UTL_TIMEOUT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Timeout.hh
  \brief  Timeout definitionsK
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
 * 2019.01.22 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "ldmx/utl/Timespec.hh"


#include <cinttypes>
#include <cstdio>

#define TO_FOREVER (reinterpret_cast<const struct timespec *>(-1))
#define TO_NOWAIT  (reinterpret_cast<const struct timespec *>( 0))

/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \class Timeout
  \brief Defines constants and methods to manipulate timeout values
                                                                          */
/* ---------------------------------------------------------------------- */
class Timeout : public Timespec<CLOCK_MONOTONIC>
{
private:
   typedef Timespec<CLOCK_MONOTONIC> TimespecTo;

public:   
   static bool isForever (const struct timespec *to);
   static bool isNoWait  (const struct timespec *to);
    

public:
   static constexpr uint32_t NanoSecondsInOneSecond = (1000*1000*1000);
   static constexpr uint32_t OneSecond              = (1000*1000*1000);

public:

   // Do nothing 
   Timeout () { return; }

   // Set the timeout = to
   Timeout (Timeout const &to);

   // Set the timeout = current time + nsecs/delta
   Timeout (uint32_t nsecs);
   Timeout (struct timespec const &delta);

   // Set the timeout = base + nsecs/delta
   Timeout (Timeout const &base, uint32_t nsecs);
   Timeout (Timeout const &base, struct timespec const &delta);


public:
   // ----------------------------------------------------------
   // -- These are inherited methods, but provided for reference
   // ----------------------------------------------------------


   // Add nsecs/delta to the timeout value
   // -- Timeout &add (uint32_t               nsecs);
   // -- Timeout &add (struct timespec const &delta);

   // Set to base + nsecs/delta
   // -- Timeout &set (Timeout         const &base, uint32_t               nsecs);
   // -- Timeout &set (Timeout         const &base, struct timespec const &delta);

   // Convenience operators
   // -- Timeout &operator +  (uint32_t               nsecs);
   // -- TTimeout &operator += (uint32_t               nsecs);
   // -- TTimeout &operator +  (struct timespec const &delta);
   // -- TTimeout &operator += (struct timespec const &delta);


   // Compute the elapsed time between to timeout values 
   // -- Tstatic uint64_t elapsed (Timeout const &begin, Timeout const &begin);
   // -- Tuint64_t    elapsedFrom (Timeout const &begin) const;
   // -- Tuint64_t    elapsedTo   (Timeout const &end) const;
};
/* ---------------------------------------------------------------------- */
} /* namespace utl                                                        */
} /* namespace ldmx                                                       */
/* ====================================================================== */




/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief Set the timeout to the \to value

  \param[in] to The timeout value 
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout::Timeout (Timeout const &to) :
       TimespecTo (to)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Sets the timout to the current time + \a nsecs

  \param[in] nsecs The number of nanoseconds to add

  \note
   To set the timeout to the current time, specify \a nsecs = 0,
                                                                          */
/* ---------------------------------------------------------------------- */

inline Timeout::Timeout (uint32_t nsecs) :
       TimespecTo (TimespecTo::current(nsecs))
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Sets the timeout to the current time + \a delta

  \param[in] delta  The time, specified as a timespec, to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout::Timeout (struct timespec const &delta) :
       TimespecTo (TimespecTo::current(delta))
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief     Set timeout to the \a base + \a nsecs

  \param[in]  base  The base timeout value
  \param[in] nsecs  The time, in nanoseconds, to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout::Timeout (Timeout const &base, uint32_t nsecs) :
       TimespecTo (base, nsecs)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Set timeout to the \a base + \a nsecs

  \param[in]  base  The base timeout value
  \param[in] delta  The time, specified as a timespec, to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout::Timeout (Timeout const          &base,
                         struct timespec const &delta) :
       TimespecTo (base, delta)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
   
   \fn     bool isForever (const struct timespec *to)
   \brief  Checks if the specified timeout value \a to, is the forever
           value.
   \retval == true  if the specified timeout value is     the forever value
   \retval == false if the specified timeout value is not the forever value
   
   \param  to The Timeout value to be checked.
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool Timeout::isForever (const struct timespec *to)
{
   bool   is_forever = (to == TO_FOREVER);
   return is_forever;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \fn     bool isNoWait (const struct timespec *to)
  \brief  Checks if the specified timeout value \a to, is the forever
          value.
   \retval == true  if the specified timeout value is     the nowait value
   \retval == false if the specified timeout value is not the nowait value
          
   \param  to The Timeout value to be checked.
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool Timeout::isNoWait (const struct timespec *to)
{
   bool   is_nowait = (to == TO_NOWAIT);
   return is_nowait;
}
/* ---------------------------------------------------------------------- */



/* ====================================================================== *\

   These are inherited methods, so no implementation here is necessary
   They are left here, just in case one wishes to specialized them, e.g.
   by putting in print statements.
\* ---------------------------------------------------------------------- */
#if 0
/* ---------------------------------------------------------------------- */
inline Timeout &Timeout::add (uint32_t nsecs)
{
   TimespecTo::add (nsecs);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Adds \a delta to the timeout

  \param[in] delta  The time, specified as a timespec, to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout &Timeout::add (struct timespec const &delta)
{
   TimespecTo::add (delta);
   return *this;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief     Set timeout to the \a base + \a nsecs

  \param[in]   base  The base timeout value
  \param[in]  nsecs  The time, in nanoseconds, to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout &Timeout::set (Timeout const          &base, 
                              uint32_t               nsecs)
{
   TimespecTo::set (base, nsecs);
   return *this;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Set timeout to the \a base + \a delta

  \param[in]  base  The base timeout value
  \param[in] delta  The time, specified as a timespec, to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline Timeout &Timeout::set (Timeout const          &base, 
                              struct timespec const &delta)
{
   TimespecTo::set (base, delta);
   return *this;
}
/* ---------------------------------------------------------------------- */



inline Timeout &Timeout::operator + (uint32_t nsecs)
{
   TimespecTo::add (nsecs);
   return *this;
}


inline Timeout &Timeout::operator += (uint32_t nsecs)
{
   TimespecTo::add (nsecs);
   return *this;
}


inline Timeout &Timeout::operator + (struct timespec const &delta)
{
   TimespecTo::add (delta);
   return *this;
}


inline Timeout &Timeout::operator += (struct timespec const &delta)
{
   TimespecTo::add (delta);
   return *this;

}
/* ---------------------------------------------------------------------- */
#endif
/* ====================================================================== */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */
#endif

