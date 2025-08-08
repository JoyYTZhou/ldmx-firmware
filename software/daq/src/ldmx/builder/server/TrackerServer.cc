// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TrackerServer.cc
  \brief  Composes and sends tracker batches for the builder server
          emulator
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


#include "TrackerServer.hh"
#include "ldmx/data/server/TrackerHeader.hh"
#include "ldmx/data/server/TrackerEvent.hh"
#include "ldmx/data/server/TrackerTrailer.hh"

#include "ldmx/builder/server/Configuration.hh"

#include "ldmx/utl/Dumper.hh"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static int  createPipe (const char *path);
static void dump       (char const *label, void const *ptr, int nbytes);
/* ====================================================================== */




/* ---------------------------------------------------------------------- */
TrackerServer::TrackerServer (ldmx::builder::server::Configuration const &cfg) :
   m_path           (  cfg.m_trkPipe),
   m_fd (createPipe (  cfg.m_trkPipe)),
   m_ntrkEvents     (cfg.m_ntrkEvents),
   m_nevents        (               0),
   m_blockNumber    (               0),
   m_eventNumber    (               0),
   m_cur            (        m_buffer)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Establishes a named pipe to push emulated tracker messages thru
  \return The file descriptor of the named pipe

  \param[in] trackerPath The name of the tracker path

  \note
   If the backing file for the tracker path already exists, it will be
   removed.
                                                                          */
/* ---------------------------------------------------------------------- */
static int createPipe (const char *path)
{
#if 0
   int removeError = remove (path);
   if (removeError)
   {
      fprintf (stderr,
               "Error:TrackerServer Removing tracker path <%s>, error = %d\n",
               path,
               removeError);
      exit (-1);
   }
#endif

   // --------------------
   // Check if pipe exists
   // --------------------
   int status = access (path, W_OK);
   if (status == 0)
   {
      // Pipe exists


      printf ("Success::TrackerServer Reopening %s\n", path);
   }
   else if (errno == ENOENT)
   {
      // Create the pipe
      int err = mkfifo (path, 0666);
      if (err)
      {
         fprintf (stderr,
                  "Error:TrackerServer Creating tracker path <%s>, error = %d\n",
                  path,
                  errno);
         exit (-1);
      }

      printf ("Success:TrackerServer Creating %s\n", path);
   }
   else
   {
      fprintf (stderr,
         "Error:TrackerServer  Creation of tracker pipe <%s> failed err = %d\n",
         path, errno);
      exit (-1);
   }


   int fd = open (path, O_WRONLY);

   if (fd >= 0)
   {
      // ------------------------------------------------
      // Ignore the signal indicating the pipe has closed
      // It is almost useless for 2 reasons
      //    1) It is not delivered when the error,
      //       typically a disconnect has occurred,
      //    2) It has no context, i.e. which pipe
      //       has the errors.
      // So don't see why one just doesn't field the
      // write error.
      // ------------------------------------------------
      signal(SIGPIPE,SIG_IGN);
   }

   printf ("Success:TrackerServer Created pipe fd = %d\n", fd);
   return fd;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Attempts to reconnect to a disconnected pipe
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerServer::reconnect ()
{
   int err = close (m_fd);
   if (err)
   {
      fprintf (stderr,
      "Error:TrackerServer Reconnection failed to close disconnected pipe %d\n",
               err);
      exit (-1);
   }


   int fd = open (m_path, O_WRONLY);

   if (fd < 0)
   {
      fprintf (stderr,
         "Error:TrackerServer  Reconnection of tracker pipe <%s> failed err = %d\n",
         m_path, errno);
      exit (-1);
   }
   else
   {
      fprintf (stderr,
              "Success:TrackerServer Reconnected tracker pipe <%s>\n",
               m_path);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Composes a tracker header
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerServer::composeHeader ()
{
   static const bool TimestampPresent = true;


   // ----------------------
   // Add the tracker header
   // ----------------------
   new (m_cur) ldmx::data::server::TrackerHeader (0,
                                                  m_blockNumber++,
                                                  TimestampPresent);
   m_cur += sizeof (ldmx::data::server::TrackerHeader);

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Adds one tracker event to the current batch

  \param[in] timestamp  The time of the tracker
                                                                          */
/* ---------------------------------------------------------------------- */
bool TrackerServer::addEvent (uint64_t timestamp)
{
   static const uint8_t Type = 2;


   // -----------------------------------------
   // If starting a new batch of tracker events
   // Then compose the tracker header
   // -----------------------------------------
   if (m_nevents == 0) composeHeader ();


   // -----------------------------------------------
   // The tracker time is defined in units of 4 nsecs
   // -----------------------------------------------
   uint64_t trackerTime = timestamp / 4;


   // --------------------------
   // Add the tracker event data
   // --------------------------
   new (m_cur) ldmx::data::server::TrackerEventLong (Type,
                                                     m_eventNumber++,
                                                     trackerTime);

   printf ("Added event %d\n", m_nevents);
   dump ("E", m_cur, sizeof (ldmx::data::server::TrackerEventLong));


   m_cur     += sizeof (ldmx::data::server::TrackerEventLong);
   m_nevents += 1;

   printf ("NtrkEvents = %d/%d\n", (int)m_nevents, m_ntrkEvents);



   // -------------
   // Check if full
   // -------------
   bool full = (m_nevents == m_ntrkEvents);
   if (full)
   {
      new (m_cur) ldmx::data::server::TrackerTrailer ();
      m_cur += sizeof (ldmx::data::server::TrackerTrailer);
   }

   return full;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Sends the bundle of tracker messages
  \return Error code

                                                                          */
/* ---------------------------------------------------------------------- */
int TrackerServer::sendBatch ()
{
   ldmx::data::server::TrackerHeader
              *header = reinterpret_cast<decltype(header)>(m_buffer);


   // --------------------------------------------------
   // Patch the number of tracker events into the header
   // --------------------------------------------------
   header->setBlockSize (m_nevents);


   // ---------------------------------------------
   // Write the batch of tracker events to the pipe
   // and reset/clear the context
   // ---------------------------------------------
   size_t  nsend  =  m_cur - m_buffer;


   ldmx::utl::Dumper dumper;
   dumper.dump ("TrackerBatch",
                reinterpret_cast<uint32_t const *>(m_buffer),
                nsend / sizeof (uint32_t));


   ssize_t nbytes = write (m_fd, m_buffer, nsend);
   if (nbytes >= 0)
   {
      if (nbytes != (int)nsend)
      {
         fprintf (stderr,
                  "Error:TrackerServer Not enough sent %d:%d\n",
                 (int)nsend, (int)nbytes);
      }
   }
   else
   {
      // Error
      if (errno == EPIPE)
      {
         reset ();
         fprintf (stderr,
                  "Error:TrackerServer  Detected disconnect of pipe <%s>\n",
                  m_path);
         return -1;
      }
      else
      {
         fprintf (stderr,
                  "Error:TrackerServer  SendBatch failed, error = %d\n", errno);
         exit (-1);
      }
   }

   reset ();

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Convenience function to
             -# If necessary, compose a tracker header
             -# add a tracker event
             -# If filled, send the tracker frame


  \param[in] timestamp  The tracker time, in nano-seconds
                                                                          */
/* ---------------------------------------------------------------------- */
bool TrackerServer::declare (uint64_t timestamp)
{
   // -----------------------------------------
   // If starting a new batch of tracker events
   // Then compose the tracker header
   // -----------------------------------------
   if (m_nevents == 0) composeHeader ();


   // --------------------------
   // Add the tracker event data
   // --------------------------
   bool filled = addEvent (timestamp);


   // --------------------------------------
   // If this event filled the tracker batch
   // --------------------------------------
   // Then send it off
   if (filled)  sendBatch ();


   // ------------------------------------------------------------------
   // Return a flag indicating whether the tracker batch was send or not
   // ------------------------------------------------------------------
   return filled;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets/clears the tracker batch context
                                                                          */
/* ---------------------------------------------------------------------- */
void TrackerServer::reset ()
{
   m_cur     = m_buffer;
   m_nevents =        0;

   return;
}
/* ---------------------------------------------------------------------- */








/* ---------------------------------------------------------------------- */
static void dump (char const *label, void const *ptr, int nbytes)
{
   uint64_t const *p64 = reinterpret_cast<uint64_t const *>(ptr);
   int             n64 = nbytes / sizeof (uint64_t);

   for (int idy = 0; idy < n64; idy++)
   {
      if ( (idy % 4) == 0 ) printf ("%s[%1d] =", label, idy);
      printf (" %16.16" PRIx64 "", p64[idy]);
      if ( (idy % 4) == 3)  putchar ('\n');
   }

   if (n64 % 4) putchar ('\n');
   return;
}
/* ---------------------------------------------------------------------- */
