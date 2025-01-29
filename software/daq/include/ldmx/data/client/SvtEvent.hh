// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_SVTEVENT_HH__
#define __LDMX_DATA_CLIENT_SVTEVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/SvtEvent.hh
  \brief  Defines cataloging  SVT events
  \author JJRussell - russell@slac.stanford.edu

  \par
   This various member fields of the SvtEvent.


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
 * 2019.02.28 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCE                                                      */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {

   class SvtHeader;
   class SvtMultiSample;
   class SvtTrailer;

} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */






/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to catalog the SVT events
                                                                          */
/* ---------------------------------------------------------------------- */
class SvtEvent
{
public:
   SvtEvent () { return; }
   SvtEvent (uint8_t const *ptr, uint32_t nbytes);


public:
   SvtHeader       const *getHeader        () const;
   SvtMultiSample  const *getMultiSample   () const;
   SvtTrailer      const *getTrailer       () const;
   uint32_t               getNBytes           () const;
   uint32_t               getMultiSampleCount () const;

public:
   static void print (SvtEvent const *evt);
   void        print () const;


public:
   SvtHeader  const  *m_header; /*!< Pointer to the SVT event header      */
   SvtTrailer const *m_trailer; /*!< Pointer to the SVT event trailer     */
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */



#include "ldmx/data/client/SvtHeader.hh"
#include "ldmx/data/client/SvtMultiSample.hh"
#include "ldmx/data/client/SvtTrailer.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructors the information to describe one SVT event

  \param[in]    ptr  Pointer to the memory just after end of the SVT event
  \param[in] nbytes  The length, in bytes, of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtEvent::SvtEvent (uint8_t const *ptr, uint32_t nbytes) :
   m_header  (reinterpret_cast<SvtHeader  const *>(ptr - nbytes)),
   m_trailer (reinterpret_cast<SvtTrailer const *>(ptr 
                       - sizeof(ldmx::data::common::SvtTrailer)))
{
   //// printf ("SvtEvent %4" PRId32 " @ %p\n", nbytes, (void *)ptr);
   //// printf ("   Header: @ %p\n", (void *)m_header);
   //// printf ("  Trailer: @ %p\n", (void *)m_trailer);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a readonly pointer to the SVT event header
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtHeader const *SvtEvent::getHeader () const
{
   return reinterpret_cast<SvtHeader const *>(m_header);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a readonly pointer to the beginning of SVT event
          multisample data
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtMultiSample const *SvtEvent::getMultiSample () const
{
   return reinterpret_cast<SvtMultiSample const *>(m_header + 1);
}
/* ---------------------------------------------------------------------- */   



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a readonly pointer to the SVT event trailer
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtTrailer const *SvtEvent::getTrailer () const
{
   return m_trailer;
}
/* ---------------------------------------------------------------------- */
 


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the length of the SVT event in bytes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtEvent::getNBytes () const
{
   return reinterpret_cast<uint8_t const *>(m_trailer + 1)
        - reinterpret_cast<uint8_t const *>(m_header);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the length of the SVT event in bytes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtEvent::getMultiSampleCount () const
{
   return m_trailer->getMultiSampleCount ();
}
/* ---------------------------------------------------------------------- */


 
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the SVT event to the console
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtEvent::print () const
{
   print (this);
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
