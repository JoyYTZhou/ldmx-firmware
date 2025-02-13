// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CONTRIBUTIONS_HH__
#define __LDMX_BUILDER_CLIENT_CONTRIBUTIONS_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Contributions.hh
  \brief  Manages the collection of all contributions to the LDMX event
           builder.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level
   directory of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
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


#include "Contribution.hh"
#include "ldmx/utl/FixedPacket.hh"
#include <cstdio>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
/* <none>                                                                 */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Manages the collection of event contributions and serves as the
         conduit to pass the individual contributions to a central
         aggregator.

  \par
   When a contribution is received from a contributor an event contribution
   structure is allocated from a pool of such contributions and filled
   in with the relevant information.  It is then placed one the appropriate
   list which is managed in an interlocked fashion (ListsInterlocked)
                                                                          */
/* ---------------------------------------------------------------------- */
class Contributions
{
public: 
   Contributions  () { return; }
   Contributions  (int ncontributors);

   typedef ldmx::utl::ListsInterlocked::Set Set;

public:
   void free (int id, Contribution *ctb);


   uint32_t appendW (ldmx::utl::List         *lists,
                     uint32_t                   any,
                     uint32_t                   all);

   uint32_t appendW (ldmx::utl::List         *lists, 
                     uint32_t                   any,
                     uint32_t                   all,
                     struct timespec const *timeout);

public:
   ldmx::utl::ListsInterlocked m_lists; 
                               /*!< Interlocked set of lists of fragments */
};
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for lists of contributions

  \param[in] ncontributors  The number of contributors
                                                                          */
/* ---------------------------------------------------------------------- */
inline Contributions::Contributions (int ncontributors) :
   m_lists (ncontributors)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Returns the specified Contribution, \a ctb, back to it 
         appropriate pool, designated by \a id

  \param[in]  id The id of the pool to return \a ctb to
  \param[in] ctb The contribution to return
                                                                          */
/* ---------------------------------------------------------------------- */
inline 
void Contributions::free (int            id, 
                          Contribution *ctb)
{
   ldmx::utl::FixedPacket *fpa = reinterpret_cast<decltype(fpa)>
                                (m_lists.getUser (id));

   ///printf ("Deleting frame %p\n", (void *)&ctb->m_frame);
   printf ("Need to add the code to do delete\n");
   ///printf ("Use Count = %ld\n", ctb->m_frame.use_count());
   fpa->free (ctb);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline uint32_t Contributions::appendW (ldmx::utl::List *lists,
                                        uint32_t           any,
                                        uint32_t           all)

{
   auto set = m_lists.appendW (lists, any, all);

   ///printf ("Contributions::appendW set = %8.8x\n", (unsigned)set);

   return set;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline uint32_t Contributions::appendW (ldmx::utl::List     *lists,
                                        uint32_t               any,
                                        uint32_t               all,
                                        struct timespec const  *to)
{
   auto set = m_lists.appendW (lists, to);

   return set;
}
/* ====================================================================== */

#endif

