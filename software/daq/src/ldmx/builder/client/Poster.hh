// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_FRAGMENT_POSTER_HH__
#define __LDMX_BUILDER_CLIENT_FRAGMENT_POSTER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/Poster.hh
  \brief  Posts and an assembled fragment to an output queue
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
 * 2019.04.01 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "Fragment.hh"
#include "ldmx/utl/ListInterlocked.hh"
#include "ldmx/utl/Timestamp.hh"

#include <thread>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
/* <none>                                                                 */
/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Typedef for the list of fragments
                                                                          */
/* ---------------------------------------------------------------------- */
typedef ldmx::utl::List  FragmentList;
/* ---------------------------------------------------------------------- */



class Poster
{
   ldmx::utl::ListInterlocked m_list;
};
