// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_SERVER_TRACKERBATCH_HH__
#define __LDMX_BUILDER_SERVER_TRACKERBATCH_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TrackerBatch.hh
  \brief  Composes a tracker batch for the builder server emulator
  \author Sophie Middleton

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



#include "ldmx/data/server/Allocator.hh"
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace data    {
namespace server  {

   class TrackerHeader;
   class TrackerEvent;
   class TrackerTrailer;
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

  \class TrackerBatch
  \brief Composes a tracker batch
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerBatch
{
public:
   TrackerBatch (void *buffer, size_t nbytes);

public:
   TrackerHeader  *getHeader  (std::size_t nbytes);
   TrackerEvent   *getEvent   (std::size_t nbytes);
   TrackerTrailer *getTrailer (std::size_t nbytes);

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

  \brief Construct a Tracker Event Batch

  \param[in] allocator The memory allocator
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerBatch::TrackerBatch (void *buffer, std::size_t nbytes) :
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
inline void const *TrackerBatch::getBuffer () const
{
   return m_alloc.buffer ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the current byte index,
          essentially the number of bytes allocated
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t  TrackerBatch::getNbytes () const
{
   return m_alloc.used ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of bytes left
  \return The number of bytes left
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TrackerBatch::left () const
{
   return m_alloc.left ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Clears/resets this batch of events
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TrackerBatch::reset ()
{
   m_nevents   = 0;
   m_alloc.reset ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates the memory for and returns a pointer to where in the
          batch's buffer to place the TrackerHeader
  \return A pointer to where in the batch's buffer to place the
          TrackerHeader
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerHeader *TrackerBatch::getHeader (std::size_t nbytes)
{
   return reinterpret_cast<TrackerHeader *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the TrackerEvent
  \return A pointer to where in the buffer to place the TrackerEvent

  \param[in] nbytes  The number of bytes in the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerEvent *TrackerBatch::getEvent (std::size_t nbytes)
{
   return reinterpret_cast<TrackerEvent *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to where in the buffer to place the TrackerTrailer
  \return A pointer to where in the buffer to place the TrackerTrailer

  \param[in] nbytes  The number of bytes in the trailer
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerTrailer *TrackerBatch::getTrailer (std::size_t nbytes)
{
   return reinterpret_cast<TrackerTrailer *>(m_alloc.allocate (nbytes));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the number of events in the SVT batch so far
  \return The number of events in the SVT batch so far
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TrackerBatch::getNevents () const
{
   return m_nevents;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
