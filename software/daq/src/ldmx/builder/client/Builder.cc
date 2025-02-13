// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Builder.cc
  \brief  Builds and keeps track of the fragments.
  \author JJRussell - russell@slac.stanford.edu

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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.01.31 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Builder.hh"
#include "Contributions.hh"
#include "Contribution.hh"

#include "ldmx/utl/Timeout.hh"

#include <cstdlib>
#include <cinttypes>
#include <cstdio>


#define REPORT_BUILD   0
#define REPORT_PROMOTE 0


#define REPORT_BUILDER 0
#define REPORT_POST    0
#define ANNOUNCE_POST  1


/* ====================================================================== */
/* DIAGNOSTICS                                                            */
/* ---------------------------------------------------------------------- */
namespace builder
{
   static void report          (uint32_t   valid, 
                                uint32_t  needed);
   static void reportTimeout   (uint64_t timeout);
   static void reportTimedout  (uint32_t  inHand);
   static void reportAppending (uint32_t  newSet);
   static void reportSeeding   (uint32_t  needed,
                                uint32_t  inHand);
   static void reportSeeded    (uint32_t  needed);
}


namespace post
{
   inline static void announce     (uint32_t  sequence);
   inline static void reportTiming (int             id, 
                                    int          ctbId,
                                    uint32_t  sequence,
                                    uint64_t timestamp,
                                    uint64_t  earliest,
                                    uint64_t   lastest);
   inline static void reportStatus (uint32_t   status);
}
/* ====================================================================== */




/* ====================================================================== */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
inline static uint32_t seedNew      (ldmx::utl::List *lists, 
                                     uint32_t          seed,
                                     uint32_t       *seaRet);
   
inline static uint32_t seedExisting (ldmx::utl::List *lists, 
                                     uint32_t         valid, 
                                     uint32_t      sequence,
                                     uint32_t       *seqRet);
/* ====================================================================== */



namespace build
{
   #if REPORT_BUILD

   inline static void reportNeeded (uint32_t   needed,
                                    uint32_t     seed,
                                    uint32_t sequence,
                                    uint32_t   oldest)
   {
      printf (
      "More needed %8.8" PRIx32 " : %8.8" PRIx32 
             " seq %8.8" PRIx32 " : %8.8" PRIx32 "\n",
            needed, seed, sequence, oldest);
      return;
   }

   
   inline static void reportReseed (uint32_t   valid, 
                                    uint32_t emptied,
                                    uint32_t  needed)
   {
      printf ("Builder::seed %8.8" PRIx32 " & ~%8.8" PRIx32 " = %8.8" PRIx32 "\n",
              valid, emptied, needed);
      return;
   }

   #else

   inline static void reportNeeded (uint32_t   needed,
                                    uint32_t     seed,
                                    uint32_t sequence,
                                    uint32_t   oldest) { return; }

   inline static void reportReseed (uint32_t   valid, 
                                    uint32_t emptied,
                                    uint32_t  needed)  { return; }
   #endif
}



namespace promote
{
   #if REPORT_PROMOTE

   inline static void reportEmpty (uint32_t emptySet)
   {
      printf ("Build::seed empty set = %8.8" PRIx32 "\n", emptySet);
      return;
   }

   #else

   inline static void reportEmpty (uint32_t emptySet) { return; }

   #endif
}



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the fragment builder

  \param[in]   expected The set of the expected contributors.  This 
                        includes the trigger and SVT contributors
  \param[in] nfragments The number of outstanding built, but not yet
                        promoted fragments that will be buffered awaiting
                        promotion to the next level
  \param[in]       ctbs The set of contributions 
                                                                          */
/* ---------------------------------------------------------------------- */
Builder::Builder (uint32_t expected, int nfragments, Contributions *ctbs) :
   FragmentList (),
   m_new       (true),
   m_pool      (nfragments),
   m_oldest    (~static_cast<decltype(m_oldest)>(0)),
   m_expected  (expected),
   m_pending   (0),
   m_timestamp (0),
   m_timeout   (0),
   m_ctbs      (ctbs)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Attempts to build a fragment

  \param[in]    lists  The lists of contributors contributions
  \param[in] required  Bit mask of contributions that are required to
                       form a fragment.  By design, this set is also
                       guarantees each valid contributor has at least
                       one contribution on its list. 
  \param[in] received  Timestamp of when this batch of  lists was
                       received

  \par
   The valid/required list may not be all the contributions that are 
   connected, with the \e missing contributors being those that have
   timed out.

   In short, it is up to the calling method to 
     -# define what the required set of contributions is
     -# guarantee that there is a list one contribution on each list

   This latter condition could be relaxed, but that would put the burden
   on this method to check. If, in fact, it did check and finding any
   contribtution lists empty, it would simply return after doing no useful
   work.
                                                                          */
/* ---------------------------------------------------------------------- */
int Builder::build (ldmx::utl::List           *lists, 
                    uint32_t                required, 
                    uint64_t            receive_time)
{
   // -------------------------------------------------------------------
   // The list of in progress fragments is empty. There
   // are two possibilities 
   //   1) This is the very first set of contributions to arrive,
   //      i.e. the initial start-up.  
   //   2) This is not first set.
   //
   // CASE 1:  This is first set of contributions to arrive, i.e start-up
   // There is no established sequence number. The smallest sequennce 
   // number from the first contribution of each non-empty list is used
   // as the target sequence number.
   //
   // CASE 2: This is not the first set of contributions
   // In this case there are two potential seed sequence number
   //   1) One more than the last sequence number seen
   //   2) The oldest in the list of required contributions.
   //
   // -------------------------------------------------------------
   auto      expected = m_expected;
   auto      sequence = m_oldest;

   //printf ("Bulld:build starting\n");

   // -----------------------------------------------------
   // Ignore any contributions from unexpected contributors
   // This should be almost impossible, but must handle it.
   // -----------------------------------------------------
   uint32_t unexpected = required & ~expected;
   removeAll (lists, unexpected);


   // --------------------------------------------------
   // Trim down to only the set of expected contributors
   // --------------------------------------------------
   uint32_t valid = required & expected;
   if (valid == 0)
   {
      // None left; weird but possible...
      return 0;
   }

   //printf ("Builder:build looking for %8.8" PRIx32 " %8.8" PRIx32 "\n", 
   //       valid, sequence);

   uint32_t seed;
   if (m_new)
   {
      // -------------------------------------------------------
      // Since there is no prior sequence number and the valid
      // set is not empty, this method will return the set of
      // contributors with a common 'lowest' sequence number.
      // While this cannot be empty, it may not be the complete
      // set.  An missing contributions must be newer. What is
      // in hand is promoted and the 'newer' contributions are
      // just left on the list.
      // -------------------------------------------------------
      m_new = false;
      seed  = seedNew (lists, valid, &sequence);
   }
   else
   {
      seed  = seedNew (lists, valid, &sequence);

      ///seed = seedExisting (lists, valid, m_oldest+1, &sequence);

      // --------------------------------------------------------
      // !!! NOTE !!!
      // ------------
      // The logic here will allow the sequence number to go in
      // reverse if and only if this set of the contributions
      // completes the fragment.  
      //
      // If not, then, because the sequence number for any given
      // contributor always increases , any missing contributions
      // must have newer sequence numbers. The contributions with 
      // older sequence numbers are treated as stragglers, for
      // example as arriving after the fragment they belonged to
      // timed out.  In this case, that ship has sailed and those
      // contributions are discarded.
      // ---------------------------------------------------------
      uint32_t needed = valid & ~seed;


      // If any more contributions are needed...
      if (needed)
      {
         build::reportNeeded (needed, seed, sequence, m_oldest);
         

         // Check if older than expected...
         if (sequence <= m_oldest)
         {
            // ------------------------------------------------------------
            // The oldest set was both incomplete and too old,
            // -> discard them
            // Also remove from the valid list if they were the only entry
            // ------------------------------------------------------------
            uint32_t emptied = remove (lists, seed);
            m_oldest = sequence;
            return emptied;
         }
         else
         {
            // --------------------------------------------------------
            // Know that the set is at least as new as the expected
            // sequence. By logic, any contributions not in this 
            // oldest set are newer and, because the contributions are
            // time ordered, this fragment is as complete as it ever 
            // will be, so it will be promoted as incomplete.
            // --------------------------------------------------------
         }
      }
      else
      {
         // -------------------------------------------------------
         // All required contributions are in. 
         //
         // Design choice
         // -------------
         // If this fit is older than what was expected, should this
         // set be discarded or just promoted as is....
         //
         // ===> The choice is to promote it.
         //
         // If this changes then this logic should be enabled.
         // The code is structured so can eventually put some
         // diagnostic counters in here.
         // ----------------------------------------------------
         if (sequence <= m_oldest)
         {
            if (false)
            {
               uint32_t emptied = remove (lists, seed);
               needed           = valid & ~emptied;

               build::reportReseed (valid, emptied, needed);

               return needed;
            }
         }
      }
   }



   // ------------------------------------------------------
   // Construct a new fragment. 
   // This action is based on two assumptions
   //   1) This method is not called before all contributors
   //      have at least one contribution.
   //   2) The contributions in the contributor lists are
   //      ordered by sequence number.
   // ------------------------------------------------------
   uint32_t   emptySet;
   m_oldest = sequence;
   uint64_t  timestamp = ldmx::utl::Timestamp::current ();
   emptySet  = promote (timestamp, sequence, m_expected, seed, lists);
   emptySet |= (expected & ~required);



   return emptySet;
}
/* ---------------------------------------------------------------------- */


inline static uint32_t seedNew (ldmx::utl::List *lists, 
                                uint32_t         valid,
                                uint32_t       *seqRet)
{
   uint32_t     seed = 0;
   uint32_t sequence = ~static_cast<decltype(sequence)>(0);

   // --------------------------------------------------
   // Brand new. Since there is no sequence number to 
   // key off of, need to find the set of contributions
   // with the oldest sequence number.
   // -------------------------------------------------- 
   while (valid)
   {
      int            id =  __builtin_ctz (valid);
      uint32_t     mask =  (1 << id);
      auto        *list = &lists[id];
      auto        *flnk =  list->first ();

      if (list->isEmpty ())
      {
         fprintf (stderr, "Fatal error list %d from valid set %8.8x is empty\n",
                  id, valid);
         exit (-1);
      }

      Contribution *ctb = reinterpret_cast<decltype(ctb)>(flnk);
      uint32_t      seq = ctb->m_sequence;


      // ----------------------------------------------------
      // If this is the smallest sequence number seen so far,
      // use as the intial contributor to the seed.
      // ----------------------------------------------------
      if      (seq  < sequence) { sequence = seq;  seed  = mask; }


      // -----------------------------------------------------------
      // Else if this is equal the current smallest sequence number,
      // add it to current seed list of contributors
      // -----------------------------------------------------------
      else if (seq == sequence) {                  seed |= mask; }


      // -----------------------
      // Remove this contributor
      // -----------------------
      valid &= ~mask;
   }


   *seqRet = sequence;
   return seed;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Finds the set of contributions that either matches the expected
          sequence number or the oldest one among the valid contributors.
  \return The set of contributors in the fragment

  \param[in:out] lists  The lists of contributors
  \param[in]     valid  The set   of valid contributors
  \param[in]  sequence  The expected sequence number
  \param[out]   seqRtn  Returned as the sequence number of the fragment

  \note
   Any contributions that have a sequence number less than the expected
   sequence number are removed from the list and returned to their parent
   pool.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t seedExisting (ldmx::utl::List           *lists, 
                              uint32_t                   valid, 
                              uint32_t                sequence,
                              uint32_t                 *seqRet)
{
   // ----------------------------------------------------
   //
   // Look for either the set of contributions consistent 
   // their sequence number being >= the expected sequence
   // number.
   //
   // The common case is that the next set of contributors
   // will have a sequence number equal to the expeced
   // sequence number.
   //
   // -----------------------------------------------------
   uint32_t       seed = 0;
   uint32_t oldestSeq  = static_cast<decltype(oldestSeq)>(~0);
   uint32_t oldestSeed = 0;

   while (valid)
   {
      int            id =  __builtin_ctz (valid);
      uint32_t     mask =  (1 << id);
      auto        *list = &lists[id];

      // -------------------------------------------------
      // Find the oldest candidate that is at least as new
      // as the expected one
      // -------------------------------------------------
      do
      {
         auto        *node = list->first ();
         Contribution *ctb = reinterpret_cast<decltype(ctb)>(node);
         uint32_t      seq = ctb->m_sequence;
         

         if (seq == sequence) 
         {
            // -------------------------------------------------
            // Got our man: 
            // Aadd to seed list and move to the contributor
            // ------------------------------------------------- 
            seed |= mask;
            break;
         }
         else if (seq  < sequence)
         {
            // ----------------------------------------------------
            // Discard: too old
            // This is either a straggler (its fragment has already
            // been completed) or a duplicate.
            // ----------------------------------------------------
            list->remove ();
            delete ctb;
         }
         else if (seq < oldestSeq)
         {
            oldestSeq  = seq;
            oldestSeed = mask;
            break;
         }
         else
         {
            oldestSeed |= mask;
            break;
         }
      }
      while (!list->isEmpty ());


      // Remove this contributor
      valid  &= ~mask;
   }


   // -------------------------------------------------
   // If did not find the next expected sequence number
   // try using the oldest viable one found.
   // ------------------------------------------------
   if (seed == 0)
   {
      if (oldestSeed == 0)
      {
         // Nothing viable found
         return 0;
      }
      else
      {
         seed     = oldestSeed;
         sequence = oldestSeq;
      }
   }

   *seqRet = sequence;
   return seed;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Removes the first contribution in the specified \a set of \a lists
  \return  The set of lists that went empty.

  \param[in]  lists  The complete set of lists
  \param[in]    set  The target set
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t Builder::remove (ldmx::utl::List *lists, uint32_t set)
{
   int           id = 0;
   uint32_t emptied = 0;

   while (set)
   {
      id = __builtin_ctz (set);
         
      ldmx::utl::List *list = &lists[id];


      // ---------------------------------------
      // Remove each contribution from this list
      // ---------------------------------------
      Contribution *ctb = reinterpret_cast<decltype(ctb)>(list->remove ());
      ctb->print ();

         
      // Free the contribution back to it's appropriate pool 
      delete ctb;

      uint32_t mask = (1 << id);


      // If this contribution list is now empty, note it
      if (list->isEmpty ()) 
      {
         emptied |= mask;
      }

      set &= ~mask;
   }

   return emptied;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Emptys all the contributions in the specified \a set of \a lists

  \param[in]  lists  The complete set of lists
  \param[in]    set  The target set
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Builder::removeAll (ldmx::utl::List *lists, uint32_t set)
{
   int id = 0;
   while (set)
   {
      id = __builtin_ctz (set);
         
      ldmx::utl::List *list = &lists[id];

      Contribution *ctb;


      // ---------------------------------------
      // Remove each contribution from this list
      // ---------------------------------------
      while ((ctb = reinterpret_cast<decltype(ctb)>(list->remove ())))
      {
         ctb->print ();
         
         // Free the contribution back to it's appropriate pool 
         delete ctb;
      }
         
      set &= ~(1 << id);
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief   Allocates a new fragment, fills it out and posts it
  \return  The set of lists that went empty as a result of their 
           contributions being promoted.
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t Builder::promote (uint64_t timestamp,
                           uint32_t sequence,
                           uint32_t expected,
                           uint32_t seed,
                           ldmx::utl::List *lists)
{
   uint32_t emptySet;


   // --------------------------------------------------------------
   // Allocate and construct a new event fragment.
   // -------------------------------------------
   // This also returns the set of contributions that are not empty, 
   // i.e. those that needed more incoming data.
   // --------------------------------------------------------------
   Fragment *fragment = new (&m_pool) Fragment (timestamp,
                                                sequence,
                                                expected,
                                                seed,
                                                lists,
                                               &emptySet);

   // ---------------------------------
   // Post the completed event fragment
   // ---------------------------------
   post (fragment);

   
   // --------------------------------------------------------
   // Return the set of emptied sets as those needed before
   // attempting to build the next fragment.  The empty set
   // includes those that where emptied when posting (only
   // contribution on the list) and those that were originally
   // missing.
   // --------------------------------------------------------
   promote::reportEmpty (emptySet);


   return emptySet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Default fragment poster

  \param[in] fragment The fragment to post

  \par
   This is only for diagnostic purposes.  All it does is delete the 
   fragment.  Any \e real fragment poster will do much more, but likely
   include deleting the fragment when it is finished with it.
                                                                          */
/* ---------------------------------------------------------------------- */
int Builder::post (Fragment *fragment)
{
   ///puts ("Posting fragment");
   m_postList.insert (fragment);

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Creates the builder thread, but in a wait state
                                                                          */
/* ---------------------------------------------------------------------- */
int Builder::launch ()
{
   m_assembler.launch (assembler, this);

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Enable (unblocks) the builder's assembler thread
                                                                          */
/* ---------------------------------------------------------------------- */
bool Builder::enable ()
{
   bool   success = m_assembler.enable ();
   return success;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Assembles individual contributions into an event fragment

  \param[in]  builder The builder control structure

  \par
   This is the entry point for the event fragment building thread.  It is
   blocked until it receives an enable message. This ensures that the 
   context is completely setup before any attempt to build events begins
                                                                          */
/* ---------------------------------------------------------------------- */
void Builder::assembler (Builder *builder)
{
   ldmx::utl::Thread *thread = &builder->m_assembler;

   // Wait till the enable message arrives before attempting to build fragments
   reinterpret_cast<decltype(thread)>(thread->block ());

   printf ("Builder::assembler Enabled expected = %8.8" PRIx32 "\n", 
           builder->m_expected);


   //  Start assembling event fragments
   uint32_t expected = builder->m_expected;
   uint32_t   needed = expected;
   uint32_t   inHand = 0;



   // Get the list of contributions to the fragment
   Contributions &ctbs = *builder->m_ctbs;


   ldmx::utl::List lists[32];
   while (1)
   {
      uint32_t valid;

      // Transfer the active lists of list to the local lists
      if (inHand == 0)
      {
         // ---------------------------------------------------
         // If looking for all contributors, no timeout
         // Note that the arrival of the next event is entirely
         // dependent on the trigger rate, so there is no 
         // sensible timeout until some contribution arrives
         //
         // Eventually will add control lists so the 0 will 
         // no longer be 0.  Therefore, need to trim the inHand
         // so only the list expected.
         // ---------------------------------------------------
         valid   = ctbs.appendW (lists, 0, needed);

         builder::report (valid, needed);

         inHand  = expected &   valid;
         needed  = expected & ~inHand;
      }
      else
      {
         // --------------------------------------------------------
         // Awaiting for an partially complete fragment to complete.
         // Here a timeout is warranted.  All contributors should
         // be reasonably prompt in promoting their contributions.
         // --------------------------------------------------------
         builder->m_timeout = 1000 * 1000 * 1000;
         builder::reportTimeout (builder->m_timeout);


         ldmx::utl::Timeout timeout (builder->m_timeout);
         uint32_t valid  = ctbs.appendW (lists, 0, needed, &timeout);
         uint32_t newSet = valid & expected;


         // -----------------
         // Check for timeout
         // -----------------
         if (newSet == 0)
         {
            // -----------------------------------
            // Timed out, promote what is in hand.
            // -----------------------------------
            builder::reportTimedout (inHand);
            needed = 0;
         }
         else
         {
            // ---------------------------
            // Got some new contributions.
            // Add to the inHand set
            // ---------------------------
            builder::reportAppending (newSet);
            inHand |= newSet;
            needed  = expected & ~inHand;
         }
      }

      builder::reportSeeding (needed, inHand);



      // ------------------------------------------------------------
      // Attempt build only after all needed contributors are present
      //
      // Note: this is in a loop, so as long as no more contributors
      // are needed, just keep attempting to build fragments.
      // ------------------------------------------------------------
      while  (needed == 0)
      {
         uint64_t timestamp = ldmx::utl::Timestamp::current ();
         uint32_t   emptied = builder->build (lists, inHand, timestamp);

         builder::reportSeeded (needed);

         inHand &= ~emptied;
         needed  =  expected & ~inHand;
      }

   }


   return;
}
/* ====================================================================== */




/* ====================================================================== */
/* DIAGNOSTICS                                                            */
/* ---------------------------------------------------------------------- */
namespace builder
{
   
   #if REPORT_ROGUE_BUILDER

   inline static void report (uint32_t valid, uint32_t needed)
   {
      printf ("LdmxBuilder::Valid set = %8.8" PRIx32 " %8.8" PRIx32 "\n", 
              valid, needed);
      return;
   }

   inline static void reportTimeout (uint64_t timeout)
   {
      printf ("\nLdmxBuilder::Timeout set to = %" PRId64 "\n", timeout);
      return;
   }

   inline static void reportTimedout (uint32_t inHand)
   {   
      printf ("LdmxBuilder::promote  promoting %8.8" PRIx32 "\n", inHand);
      return;
   }


   inline static void reportAppending (uint32_t newSet)
   {
      printf ("LdmxBuilder::appendW newSet = %8.8" PRIx32 "\n", newSet);
      return;
   }


   inline static void reportSeeding (uint32_t needed, uint32_t inHand)
   {
      printf ("LdmxBuilder::Contribution set needed = %8.8" PRIx32 
                                         ": inHand = %8.8" PRIx32 "\n", 
              needed, inHand);
      return;
   }

   inline static void reportSeeded (uint32_t needed)
   {
      printf ("LdmxBuilder::build still needs = %8.8" PRIx32 "\n", needed);
      return;
   }


   #else

   inline static void report          (uint32_t   valid,
                                       uint32_t  needed) { return; }
   inline static void reportTimeout   (uint64_t timeout) { return; }
   inline static void reportTimedout  (uint32_t  inHand) { return; }
   inline static void reportAppending (uint32_t  newSet) { return; }
   inline static void reportSeeding   (uint32_t  needed,
                                       uint32_t  inHand) { return; }
   inline static void reportSeeded    (uint32_t  needed) { return; }

   #endif
}


namespace post
{
   #if ANNOUNCE_POST

   inline static void announce (uint32_t sequence)
   {
      printf ("Posting fragment #: %8.8" PRIx32 "\n", sequence);
      return;
   }

   #else

   inline static void announce (uint32_t sequence) { return;

   #endif



   #if REPORT_POST

   inline static void reportTiming (int id, 
                                    int ctbId,
                                    uint32_t sequence,
                                    uint64_t timestamp,
                                    uint64_t earliest,
                                    uint64_t latest)
   {
      
      printf ("Timestamp[%d:%d.%5.5" PRIx32 "] = "
              "%16.16" PRIx64 " earliest:latest = %16.16" PRIx64 ":"
              "%16.16" PRIx64 " detla = %" PRId64 "\n", 
              id, ctbId, sequence, timestamp, earliest, latest, latest - earliest);
      return;
   }


   inline static void reportStatus (uint32_t status)
   {
      printf ("Returning status = %d\n", status);
      return;
   }


   #else

   inline static void reportTiming (int             id, 
                                    int          ctbId,
                                    uint32_t  sequence,
                                    uint64_t timestamp,
                                    uint64_t  earliest,
                                    uint64_t   lastest) { return; }

   inline static void reportStatus (uint32_t   status) { return; }

   #endif
}
/* ====================================================================== */
