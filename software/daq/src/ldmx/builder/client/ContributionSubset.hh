// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CONTRIBUTIONSUBSET_HH__
#define __LDMX_BUILDER_CLIENT_CONTRIBUTIONSUBSET_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ContributionSubset.hh
  \brief  Characterizes what defines a particular subset of all contributors
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
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
 * 2019.04.01 jrr Moved from include -> src, eliminated namespace
 * 2019.03.25 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/ListsInterlocked.hh"
#include <cstdint>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
class Contributions;
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITIONS                                                            */
/* ---------------------------------------------------------------------- */
class ContributionSubset
{
public:
   ContributionSubset ();
   ContributionSubset (Contributions      *ctbs,
                       int               ctbBeg);

   ContributionSubset (Contributions      *ctbs,
                       int               ctbBeg,
                       int               ctbCnt);

public:
   ldmx::utl::ListsInterlocked::Set getSet () const;
   int                              getBeg () const;
   int                              getCnt () const;


public:
   Contributions    *m_ctbs;  /*!< The contribution lists                */
   ldmx::utl::ListsInterlocked::
   Set                m_set;  /*!< Set of the contributors as a bit mask */
   
   uint8_t            m_beg;  /*!< Beginning index                       */
   uint8_t            m_cnt;  /*!< Number of contributors in this subset */
};
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline ContributionSubset::ContributionSubset () 
 {
    return; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Defines a contribution subset consisting of only one contributor

  \param[in]   ctbs  The controlling class of the complete set of
                     contributors
  \param[in] ctbBeg  This contributor's identifying index
                                                                          */
/* ---------------------------------------------------------------------- */
inline
ContributionSubset::
ContributionSubset (Contributions  *ctbs,
                    int           ctbBeg) :
    ContributionSubset (ctbs, ctbBeg, 1)
{
    return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Defines subset of \a ctbCnt contributors starting at identifying
         index \a ctbBeg

  \param[in]   ctbs  The controlling class of the complete set of
                     contributors
  \param[in] ctbBeg  This contributor's identifying index
  \param[in] ctbCnt  The number of contributors in this subset
                                                                          */
/* ---------------------------------------------------------------------- */
inline
ContributionSubset::
ContributionSubset (Contributions *ctbs,
                    int           ctbBeg,
                    int           ctbCnt) :
   m_ctbs (ctbs),
   m_set  (ldmx::utl::ListsInterlocked::listSet (ctbBeg, ctbCnt)),
   m_beg  (ctbBeg),
   m_cnt  (ctbCnt)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the bit mask for this subset
  \return The bit mask for this subset
                                                                          */
/* ---------------------------------------------------------------------- */
inline ldmx::utl::ListsInterlocked::Set ContributionSubset::getSet () const
{
   return m_set;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Return the identifying index for the first contributor in this
          subset
  \return The identifying index for the first contributor in this subset
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ContributionSubset::getBeg () const
{
   return m_beg;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Return the count of contributors in this subset

  \return The identifying index for the first contributor in this subset
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ContributionSubset::getCnt () const
{
   return m_cnt;
}
/* ====================================================================== */


#endif

