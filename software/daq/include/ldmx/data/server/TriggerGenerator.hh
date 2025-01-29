// -*-Mode: C++;-*-


#ifndef __LDMX_DATA_SERVER_TRIGGERGENERATOR_HH__
#define __LDMX_DATA_SERVER_TRIGGERGENERATOR_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TriggerGenerator.cc
  \brief  Generates a simulated Trigger batch of events
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
 * 2019.03.24 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class TriggerBatch;

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

  \brief  Generates one Trigger batch of events
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerGenerator
{
public:
    TriggerGenerator  ();

   void generate (TriggerBatch    *batch,
                  int            nevents);

public:
   void reset      (ldmx::data::server::TriggerBatch *batch);
   void addHeader  (ldmx::data::server::TriggerBatch *batch,
                    uint8_t                       blockSize,
                    bool                   timestampPresent);
   int  addEvent   (ldmx::data::server::TriggerBatch *batch,
                    uint8_t                            type,
                    uint64_t                      timestamp);


public:
   void setBatchSeqNumber (uint32_t batchSeqNumber);
   void setEventSeqNumber (uint32_t eventSeqNumber);

   
public:
   uint32_t           m_batchSeqNumber;
   uint32_t           m_eventSeqNumber;
   bool             m_timestampPresent;
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
inline void TriggerGenerator::setBatchSeqNumber (uint32_t batchSeqNumber)
{
   m_batchSeqNumber = batchSeqNumber;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the event sequence number

  \param[in] eventSeqNumber The event sequence number value
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TriggerGenerator::setEventSeqNumber (uint32_t eventSeqNumber)
{
   m_eventSeqNumber = eventSeqNumber;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
