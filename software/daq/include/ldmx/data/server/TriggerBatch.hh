// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_TRIGGERBATCH_HH__
#define __LDMX_BUILDER_SERVER_TRIGGERBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TriggerBatch.hh
  \brief  Composes a trigger batch for the builder server emulator
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
 * 2019.03.24 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/server/Allocator.hh"
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace data    {
namespace server  {

   class TriggerHeader;
   class TriggerEvent;
   class TriggerTrailer;
}
}
}
/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \class TriggerBatch
  \brief Composes a trigger batch
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerBatch
{
public:
   TriggerBatch (void *buffer, size_t nbytes);

public:
   TriggerHeader  *getHeader  (std::size_t nbytes);
   TriggerEvent   *getEvent   (std::size_t nbytes);
   TriggerTrailer *getTrailer (std::size_t nbytes); 

public:
   void const *getBuffer () const;
   uint32_t    getNbytes () const;
   uint32_t         left () const;
   uint32_t   getNevents () const;
   void            reset ();

public:
   Allocator  m_alloc;  /*!< The allocator                                */
   uint32_t m_nevents;  /*!< The number of event in the batch so far      */
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/server/Allocator.hh"

/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Construct a Trigger Event Batch

  \param[in] allocator The memory allocator 
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerBatch::TriggerBatch (void *buffer, std::size_t nbytes) :
   m_alloc   (buffer, nbytes),
   m_nevents (             0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get a pointer to the start of the buffer memory
  \return The number of bytes left
                                                                          */
/* ---------------------------------------------------------------------- */
inline void const *TriggerBatch::getBuffer () const
{
   return m_alloc.buffer ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the current byte index, 
          essentially the number of bytes allocated
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t  TriggerBatch::getNbytes () const
{
   return m_alloc.used ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of bytes left
  \return The number of bytes left
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerBatch::left () const
{
   return m_alloc.left ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Clears/resets this batch of events
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TriggerBatch::reset ()
{
   m_nevents   = 0;
   m_alloc.reset ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates the memory for and returns a pointer to where in the
          batch's buffer to place the TriggerHeader
  \return A pointer to where in the batch's buffer to place the
          TriggerHeader
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerHeader *TriggerBatch::getHeader (std::size_t nbytes)
{
   return reinterpret_cast<TriggerHeader *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the TriggerEvent
  \return A pointer to where in the buffer to place the TriggerEvent

  \param[in] nbytes  The number of bytes in the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerEvent *TriggerBatch::getEvent (std::size_t nbytes)
{
   return reinterpret_cast<TriggerEvent *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the TriggerTrailer
  \return A pointer to where in the buffer to place the TriggerTrailer

  \param[in] nbytes  The number of bytes in the trailer
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerTrailer *TriggerBatch::getTrailer (std::size_t nbytes)
{
   return reinterpret_cast<TriggerTrailer *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the number of events in the SVT batch so far
  \return The number of events in the SVT batch so far
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerBatch::getNevents () const
{
   return m_nevents;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
