// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/server/TriggerServer.cc
  \brief  Composes and sends trigger batches for the builder server
          emulator
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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.03.19 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "TriggerServer.hh"
#include "ldmx/data/server/TriggerHeader.hh"
#include "ldmx/data/server/TriggerEvent.hh"
#include "ldmx/data/server/TriggerTrailer.hh"

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
TriggerServer::TriggerServer (ldmx::builder::server::Configuration const &cfg) :
   m_path           (  cfg.m_trgPipe),
   m_fd (createPipe (  cfg.m_trgPipe)),
   m_ntrgEvents     (cfg.m_ntrgEvents),
   m_nevents        (               0),
   m_blockNumber    (               0),
   m_eventNumber    (               0),
   m_cur            (        m_buffer)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Establishes a named pipe to push emulated trigger messages thru
  \return The file descriptor of the named pipe

  \param[in] triggerPath The name of the trigger path

  \note
   If the backing file for the trigger path already exists, it will be
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
               "Error:TriggerServer Removing trigger path <%s>, error = %d\n",
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


      printf ("Success::TriggerServer Reopening %s\n", path);
   }
   else if (errno == ENOENT)
   {
      // Create the pipe
      int err = mkfifo (path, 0666);
      if (err)
      {
         fprintf (stderr, 
                  "Error:TriggerServer Creating trigger path <%s>, error = %d\n",
                  path,
                  errno);
         exit (-1);
      }

      printf ("Success:TriggerServer Creating %s\n", path);
   }
   else
   {
      fprintf (stderr,
         "Error:TriggerServer  Creation of trigger pipe <%s> failed err = %d\n",
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

   printf ("Success:TriggerServer Created pipe fd = %d\n", fd);
   return fd;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Attempts to reconnect to a disconnected pipe
                                                                          */
/* ---------------------------------------------------------------------- */
void TriggerServer::reconnect ()
{
   int err = close (m_fd);
   if (err)
   {
      fprintf (stderr,
      "Error:TriggerServer Reconnection failed to close disconnected pipe %d\n",
               err);
      exit (-1);
   }


   int fd = open (m_path, O_WRONLY);

   if (fd < 0)
   {
      fprintf (stderr,
         "Error:TriggerServer  Reconnection of trigger pipe <%s> failed err = %d\n",
         m_path, errno);
      exit (-1);
   }
   else
   {
      fprintf (stderr,
              "Success:TriggerServer Reconnected trigger pipe <%s>\n",
               m_path);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Composes a trigger header
                                                                          */
/* ---------------------------------------------------------------------- */
void TriggerServer::composeHeader ()
{
   static const bool TimestampPresent = true;


   // ----------------------
   // Add the trigger header
   // ----------------------
   new (m_cur) ldmx::data::server::TriggerHeader (0,
                                                  m_blockNumber++,
                                                  TimestampPresent);
   m_cur += sizeof (ldmx::data::server::TriggerHeader);

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Adds one trigger event to the current batch

  \param[in] timestamp  The time of the trigger
                                                                          */
/* ---------------------------------------------------------------------- */
bool TriggerServer::addEvent (uint64_t timestamp)
{
   static const uint8_t Type = 2;


   // -----------------------------------------
   // If starting a new batch of trigger events
   // Then compose the trigger header
   // -----------------------------------------
   if (m_nevents == 0) composeHeader ();


   // -----------------------------------------------
   // The trigger time is defined in units of 4 nsecs
   // -----------------------------------------------
   uint64_t triggerTime = timestamp / 4;


   // --------------------------
   // Add the trigger event data
   // --------------------------
   new (m_cur) ldmx::data::server::TriggerEventLong (Type, 
                                                     m_eventNumber++,
                                                     triggerTime);

   printf ("Added event %d\n", m_nevents);
   dump ("E", m_cur, sizeof (ldmx::data::server::TriggerEventLong));


   m_cur     += sizeof (ldmx::data::server::TriggerEventLong);
   m_nevents += 1;

   printf ("NtrgEvents = %d/%d\n", (int)m_nevents, m_ntrgEvents);



   // -------------
   // Check if full
   // -------------
   bool full = (m_nevents == m_ntrgEvents);
   if (full)
   {
      new (m_cur) ldmx::data::server::TriggerTrailer ();
      m_cur += sizeof (ldmx::data::server::TriggerTrailer);
   }

   return full;
}
/* ---------------------------------------------------------------------- */



   
/* ---------------------------------------------------------------------- *//*!

  \brief  Sends the bundle of trigger messages
  \return Error code

                                                                          */
/* ---------------------------------------------------------------------- */
int TriggerServer::sendBatch ()
{
   ldmx::data::server::TriggerHeader
              *header = reinterpret_cast<decltype(header)>(m_buffer);


   // --------------------------------------------------
   // Patch the number of trigger events into the header
   // --------------------------------------------------
   header->setBlockSize (m_nevents);


   // ---------------------------------------------
   // Write the batch of trigger events to the pipe
   // and reset/clear the context
   // ---------------------------------------------
   size_t  nsend  =  m_cur - m_buffer;


   ldmx::utl::Dumper dumper;
   dumper.dump ("TriggerBatch", 
                reinterpret_cast<uint32_t const *>(m_buffer), 
                nsend / sizeof (uint32_t));


   ssize_t nbytes = write (m_fd, m_buffer, nsend);
   if (nbytes >= 0)
   {
      if (nbytes != (int)nsend)
      {
         fprintf (stderr,
                  "Error:TriggerServer Not enough sent %d:%d\n",
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
                  "Error:TriggerServer  Detected disconnect of pipe <%s>\n",
                  m_path);
         return -1;
      }
      else
      {
         fprintf (stderr, 
                  "Error:TriggerServer  SendBatch failed, error = %d\n", errno);
         exit (-1);
      }
   }

   reset ();

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Convenience function to 
             -# If necessary, compose a trigger header
             -# add a trigger event
             -# If filled, send the trigger frame


  \param[in] timestamp  The trigger time, in nano-seconds
                                                                          */
/* ---------------------------------------------------------------------- */
bool TriggerServer::declare (uint64_t timestamp)
{
   // -----------------------------------------
   // If starting a new batch of trigger events
   // Then compose the trigger header
   // -----------------------------------------
   if (m_nevents == 0) composeHeader ();


   // --------------------------
   // Add the trigger event data
   // --------------------------
   bool filled = addEvent (timestamp);


   // --------------------------------------
   // If this event filled the trigger batch
   // --------------------------------------
   // Then send it off
   if (filled)  sendBatch ();

   
   // ------------------------------------------------------------------
   // Return a flag indicating whether the trigger batch was send or not
   // ------------------------------------------------------------------
   return filled;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets/clears the trigger batch context
                                                                          */
/* ---------------------------------------------------------------------- */
void TriggerServer::reset ()
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
