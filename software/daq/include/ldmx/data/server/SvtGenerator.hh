// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_SVTGENERATOR_HH__
#define __LDMX_DATA_SERVER_SVTGENERATOR_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/SvtGenerator.cc
  \brief  Generates a simulated SVT batch of events
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
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.03.05 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class SvtBatch;

} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Generates one SVT batch of events
                                                                          */
/* ---------------------------------------------------------------------- */
class SvtGenerator
{
public:
   SvtGenerator  (uint8_t rceAddress);

   void generate (SvtBatch        *batch,
                  int            nevents,
                  int      nmultisamples);

public:
   void reset      (ldmx::data::server::SvtBatch *batch);
   void addHeader  (ldmx::data::server::SvtBatch *batch);
   int  addEvent   (ldmx::data::server::SvtBatch *batch,
                    int                   nmultisamples);

public:
   void setBatchSeqNumber (uint32_t batchSeqNumber);
   void setEventSeqNumber (uint32_t eventSeqNumber);

   
public:
   uint32_t           m_batchSeqNumber;
   uint32_t           m_eventSeqNumber;
   uint8_t                m_rceAddress;
};
/* ---------------------------------------------------------------------- */      
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!
  \brief  Sets the batch sequence number

  \param[in] batchSeqNumber The batch sequence number value
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtGenerator::setBatchSeqNumber (uint32_t batchSeqNumber)
{
   m_batchSeqNumber = batchSeqNumber;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the event sequence number

  \param[in] eventSeqNumber The event sequence number value
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtGenerator::setEventSeqNumber (uint32_t eventSeqNumber)
{
   m_eventSeqNumber = eventSeqNumber;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
