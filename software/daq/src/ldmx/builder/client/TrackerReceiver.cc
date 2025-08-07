// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TrackerReceiver.cc
  \brief  Reads tracker messages from the builder server emulator
  \author Sophie Middleton

  \par
   This file is part of the LDMX software platform. It is subject to
   the license terms in the LICENSE.txt file found in the top-level directory
   of this distribution and at:

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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.03.21 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "TrackerReceiver.hh"
#include "Tracker.hh"
#include "Contributions.hh"
#include "ldmx/builder/client/Configuration.hh"


#include "ldmx/data/client/TrackerBatch.hh"
#include "ldmx/data/client/TrackerHeader.hh"
#include "ldmx/data/client/TrackerEvent.hh"

#include "ldmx/utl/Dumper.hh"

#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <cstdint>
#include <cstdlib>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
// <none>
/* ====================================================================== */




/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static Tracker *allocate (ldmx::utl::FixedPacket                     *fpa);

static int     readBatch (int                                          fd,
                          ldmx::data::client::TrackerBatch         *batch,
                          size_t                                  maxSize);

inline static uint64_t
        getTrackerNumber (ldmx::data::client::TrackerEventLong const *evt,
                          bool                                     isLong);

/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief  Opens the named pipe for reading
  \return The file descripotor of the pipe

  \param[in] path The path of the tracker pipe
                                                                          */
/* ---------------------------------------------------------------------- */
static inline int openPipe (char const *path)
{
   int fd = open(path, O_RDONLY);
   if (fd < 0)
   {
      fprintf (stderr,
               "Error:TrackerReceiver unable to open the tracker pipe <%s>\n",
               path);
      exit (-1);
   }

   return fd;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Packet initializer

  \param[in]    fpa        The initialize parameter, in this case a pointer
                           to the controlling fixed packet class
  \param[in] packet        The packet to initialize
  \param[in] packet_size   The size, in bytes, of the packet
  \param[in] packet_offset The offset, in bytes, of the packet's control
                           structure, this is typically a link pointer
                                                                          */
/* ---------------------------------------------------------------------- */
static void init (void *fpa,
                  void *packet,
                  int   packet_size,
                  int   packet_offset)
{
   Contribution *ctb = reinterpret_cast<decltype(ctb)>(packet);

   ctb->m_fpa  = reinterpret_cast<decltype(ctb->m_fpa)>(fpa);
   ctb->m_type = Contribution::Type::Tracker;
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Computes the size of a buffer larage enough to hold the specified
          number of tracker events.
  \return The size of the buffer, in bytes


 \param[in] nevents  The maximum number of events in a tracker batch

                                                                          */
/* ---------------------------------------------------------------------- */
static inline int getBatchSize (int nevents)
{
   int    nbytes = ldmx::data::client::TrackerBatch::maxSize (nevents);
   return nbytes;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the tracker receiver

  \param[in] cfg  Configuration parameters
                                                                          */
/* ---------------------------------------------------------------------- */
TrackerReceiver::TrackerReceiver (ldmx::builder::client::Configuration const &cfg,
                                  Contributions                             *ctbs,
                                  int                                      ctbIdx) :
   m_contributions (ctbs, ctbIdx),
   m_fpa           (cfg.m_ntrgEvents,
                    sizeof (Tracker),
                    0,
                    init,
                    &m_fpa),
   m_thread        (),
   m_tiNBuf        (getBatchSize (cfg.m_ntrgEventsPerBatch)),
   m_tiBuf         (new uint8_t[m_tiNBuf]),
   m_fd            (openPipe (cfg.m_trackerPipe.c_str())),
   m_nbatches      (0),
   m_nevents       (0)
{
   return;
}
/* ---------------------------------------------------------------------- */



void test ()
{
   printf ("Hello\n");
   for (int idx = 0; idx < 10; idx++)
   {
      usleep (500*1000);
      printf ("Testing %d\n", idx);
   }
}


/* ---------------------------------------------------------------------- *//*!

  \brief Starts the tracker receiver thread
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerReceiver::start ()
{
   printf ("Starting tracker receiver\n");
   new (&m_thread) std::thread (receiver, this);
   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Receives tracker events

  \param[in] receiver The tracker receiver class
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerReceiver::receiver (TrackerReceiver *receiver)
{
   int     fd =  receiver->m_fd;
   auto   fpa = &receiver->m_fpa;
   auto lists = &receiver->m_contributions.m_ctbs->m_lists;
   int listId =  receiver->m_contributions.m_beg;


   ldmx::data::client::TrackerHeader hdr;
   ldmx::utl::List               lclList;
   ldmx::data::client::TrackerBatch *batch = receiver->trackerBatch ();
   int                           batchSize = receiver->m_tiNBuf;


   ldmx::utl::Dumper dumper;

   while (1)
   {

      // Read the tracker batch
      int nread __attribute__ ((unused));


      nread = readBatch (fd, batch, batchSize);
      ldmx::data::client::TrackerHeader *hdr = batch->getHeader ();

      dumper.dump ("T",
                   reinterpret_cast<uint32_t const *>(batch),
                   nread / sizeof (uint32_t));


      // Retrieve the information needed to read the tracker events
      int      nevents = hdr->getBlockSize0       ();
      bool      isLong = hdr->getTimestampPresent ();
      size_t eventSize = isLong
                       ? sizeof (ldmx::data::client::TrackerEventLong)
                       : sizeof (ldmx::data::client::TrackerEvent);

      printf ("Reading %d events\n", nevents);



      // Read in the number of events in this tracker batch
      for (int idx = 0; idx < nevents; idx++)
      {
         // Allocate enough storage for the long form of the tracker event
         auto *trg = allocate (fpa);


         // Locate the event
         ldmx::data::client::TrackerEventLong const
                            *evt = batch->getEventLong (idx);


         // Get the receive time and the event sequence number
         auto      rcvTime = ldmx::utl::Timestamp::current ();
         uint64_t sequence = getTrackerNumber (evt, isLong);


         // Is this the last event
         bool last = idx == nevents - 1;


         // Fill in the contribution header
         // This is common to all contributions and is used to build the fragment
         printf ("TrackerEvent: nevents:%4d/%4d long:%d seq:%12.12lx nbyte:%zd\n",
                 idx, nevents, isLong, sequence, eventSize);


         ::new (trg) Tracker (eventSize,
                              sequence,
                              rcvTime,
                              evt,
                              last);


         dumper.dump ("TrgEvent",
                      reinterpret_cast<uint32_t const *>(&trg->m_event),
                      sizeof (trg->m_event) / sizeof (uint32_t));




         // Place this tracker event on the local list
         lclList.insert (trg);
      }


      // Transfer the local list of tracker events to the communications list
      lists->append (listId, &lclList);
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Allocates one tracker event from the specified fixed packet pool
  \return  The allocated tracker event

  \param[in] fpa The fixed packeet pool to allocate from

  \note
   The pool is constructed to return enough memory to hold both the short
   and long form of the tracker event
                                                                          */
/* ---------------------------------------------------------------------- */
inline static Tracker *allocate (ldmx::utl::FixedPacket *fpa)
{
   Tracker *trg = reinterpret_cast<decltype(trg)>(fpa->getW ());

   return trg;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Retrieve either the 32 or 48-bit tracker sequence number
  \return Either the 32 or 48-bit tracker sequence number

  \param[in]    evt The tracker event
  \param[in] isLong Flag indicating that this is the long form of the
                    tracker event
                                                                          */
/* ---------------------------------------------------------------------- */
inline static uint64_t
       getTrackerNumber (ldmx::data::client::TrackerEventLong const *evt,
                         bool                                     isLong)
{
   uint64_t sequence = isLong
                      ? evt->getTrackerNumber48 ()
                      : reinterpret_cast<ldmx::data::client::TrackerEvent const *>
                       (evt)->getTrackerNumber ();
   return sequence;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Reads in the Tracker header

  \param[ in] fd  The file descriptor of the named pipe
  \param[out] hdr The memory to hold the tracker header
                                                                          */
/* ---------------------------------------------------------------------- */
inline static int readHeader (int fd, ldmx::data::client::TrackerHeader *hdr)
{
   ssize_t nread = read (fd, hdr, sizeof (*hdr));

   if (nread != sizeof (*hdr))
   {
      fprintf (stderr,
               "TrackerReceiver::receiver Error reading in header\n"
               "                          Needed:Got %zd:%zd\n",
               sizeof (*hdr), nread);
      exit (-1);
   }

   return nread;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Reads in everything but tracker header for a tracker batch

  \param[ in]    fd  The file descriptor of the named pipe
  \param[out]   buf  The memory to hold the remainder of the tracker batch
  \param[ in] nbytes The number of bytes to read
                                                                          */
/* ---------------------------------------------------------------------- */
inline static int readRest (int fd, void *buf, size_t nbytes)
{
   ssize_t nread = read (fd, buf, nbytes);

   if (nread != static_cast<decltype(nread)>(nbytes))
   {
      fprintf (stderr,
               "TrackerReceiver::receiver Error reading in remainder\n"
               "                          Needed:Got %zd:%zd\n",
               nbytes, nread);
      exit (-1);
   }

   return nread;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Read a batch of tracker events
  \return The number of events in the batch

  \param[ in]      fd The file descriptor to read from
  \param[out]   batch Pointer to the receiving tracker batch
  \parsm[ in] maxSize The maximum number of bytes available in \a batch
                                                                           */
/* ---------------------------------------------------------------------- */
static int readBatch (int                                  fd,
                      ldmx::data::client::TrackerBatch *batch,
                      size_t                          maxSize)
{
   // Locate where the header goes
   ldmx::data::client::TrackerHeader *hdr = batch->getHeader ();


   // Read the header
   int nhdr = readHeader (fd, hdr);


   // Retrieve the information needed to read the rest of the tracker batch
   int      nevents = hdr->getBlockSize0       ();
   bool      isLong = hdr->getTimestampPresent ();
   size_t eventSize = isLong
                    ? sizeof (ldmx::data::client::TrackerEventLong)
                    : sizeof (ldmx::data::client::TrackerEvent);


   // Calculate the size of the remaining data
   size_t nbytes = nevents * eventSize + sizeof (ldmx::data::client::TrackerTrailer);


   // Read the remainder of the tracker batch
   printf ("Reading %d events\n", nevents);
   int nrest = readRest (fd, hdr + 1, nbytes);


   // Return the total number of bytes in this batch
   return nhdr + nrest;
}
/* ---------------------------------------------------------------------- */
