// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Timespec.cc
  \brief  Implementation of methods to manipulate a standard timespec
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



class Timespec
{
public:
   Timespec () { return; }


public:

   Timespec &current ();
   Timespec &add (uint32_t nsecs);
   Timespec &add (Timespec const &ts1);
   Timespec &add (Timespec const &ts1, Timespec const &ts2);


   Timespec &operator + (Timespec const &ts1);
   Timespec &operator += (Timespec const &ts1);


public:
   struct timespec m_ts;
};


inline Timespec &Timespec::current ()
{
   clock_gettime (CLOCK_REALTIME, &m_ts);
   return *this;
}


inline Timespec &add (uint32_t nsecs)
{
   sec  = m_ts.tv_sec;
   nsec = m_ts.tv_nsec;

   nsec += nsecs
   while (nsec >= NanoSecondsInOneSecond)
   {
      nsec -= NanoSecondsInOneSecond;
      sec  += 1;
   }

   m_ts.tv_sec  =  sec;
   m_ts.tv_nsec = nsec;

   return *this;
}


inline Timespec &Timespec::add (Timespec const &ts1)
{
   sec  = m_ts.tv_sec;
   nsec = m_ts.tv_nsec;

   nsec += ts1.tv_nsec;
   if (nsec >= NanoSecondsInOneSecond)
   {
      nsec -= NanoSecondsInOneSecond;
      sec  += 1;
   }

   m_ts.tv_sec  =  sec;
   m_ts.tv_nsec = nsec;

   return *this;
}



inline Timespec &Timespec::add (Timespec const &ts1, Timespec const &ts2)
{
   sec  = ts1.tv_sec  + ts2.tv_sec;
   nsec = ts1.tv_nsec + t22_tv_nsec 


   if (nsec >= NanoSecondsInOneSecond)
   {
      nsec -= NanoSecondsInOneSecond;
      sec  += 1;
   }

   m_ts.tv_sec  =  sec;
   m_ts.tv_nsec = nsec;

   return *this;
}


inline Timespec &operator Timespec::+ (Timespec const &ts1)
{
   return add (ts1);
}


Timespec &operator Timespec::+= (Timespec const &ts1)
{
   return add (ts1);
}
