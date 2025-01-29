// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_TRIGGEREVENT_HH__
#define __LDMX_DATA_SERVER_TRIGGEREVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TriggerEvent.hh
  \brief  Defines filling the trigger data structure.
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
 * 2019.02.20 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/TriggerEvent.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>
#include <cstdio>

/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Fill the short form of the trigger data
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerEventShort : public ldmx::data::common::TriggerEventShort
{
public:
   TriggerEventShort () = delete;

   TriggerEventShort (uint8_t     type,
                      uint32_t  number);

   TriggerEventShort (uint32_t eventWordCount,
                      bool           syncFlag,
                      uint8_t            mbf0,
                      uint8_t            type,
                      uint32_t        number);
public:
   static uint32_t addEventWordCount (uint32_t eventWordCount);
   static uint32_t addSyncFlag       (bool           syncFlag);
   static uint32_t addMbf0           ();
   static uint32_t addMbf0           (uint8_t            mbf0);
   static uint32_t addType           (uint8_t            type);

private:
   template  <typename ValueType>
   static uint32_t add (ValueType                                      value, 
                        ldmx::data::common::TriggerEventShort::Mask     mask,
                        ldmx::data::common::TriggerEventShort::Offset offset);
};
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Fill the long form of the trigger event data
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerEventLong : public ldmx::data::server::TriggerEventShort,
                         public ldmx::data::common::TriggerEventExtention
{
public:
   TriggerEventLong () = delete;

   TriggerEventLong (uint8_t        type,
                     uint64_t     number,
                     uint64_t  timestamp);

   TriggerEventLong (uint32_t eventWordCount,
                     bool           syncFlag,
                     uint8_t            mbf0,
                     uint8_t            type,
                     uint64_t         number,
                     uint64_t      timestamp);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- */
inline TriggerEventShort::TriggerEventShort (uint32_t eventWordCount,
                                             bool           syncFlag,
                                             uint8_t            mbf0,
                                             uint8_t            type,
                                             uint32_t  triggerNumber)
{
   m_w32 = addEventWordCount (eventWordCount)
         | addSyncFlag       (      syncFlag)                        
         | addMbf0           (          mbf0)
         | addType           (          type);

   m_numberLo = triggerNumber;

   printf ("EventWordCount: %8.8" PRIx32 " sync:%1d mbf0:%2.2" PRIx8 ""
           "Type:%2.2" PRIx8 "trgNum:%8.8" PRIx32 "\n",
           eventWordCount, syncFlag, mbf0, type, triggerNumber);
   printf ("W32 = %8.8" PRIx32 "\n", m_w32);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline TriggerEventShort::TriggerEventShort (uint8_t     type,
                                             uint32_t  number)
{
   m_w32 = addEventWordCount (       1)
         | addSyncFlag       (ldmx::data::common::TriggerEventShort::SyncFlag)
         | addMbf0           (ldmx::data::common::TriggerEventShort::Mbf0)
         | addType           (    type);

   m_numberLo = number;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Convenience method to customize the general template method
         to add a field to be specific for the trigger event data word.

  \param[in]  value  The value of the field to add
  \param[in]   mask  The right justified bit mask of the bit field
  \param[in] offset  The right justified shift
                                                                          */
/* ---------------------------------------------------------------------- */
template   <typename ValueType>
inline uint32_t TriggerEventShort::
                add (ValueType                                      value, 
                     ldmx::data::common::TriggerEventShort::Mask     mask,
                     ldmx::data::common::TriggerEventShort::Offset offset)
{
   uint32_t field = ldmx::utl::bf::
                        add<uint32_t, 
                            ValueType, 
                            ldmx::data::common::TriggerEventShort::Mask, 
                            ldmx::data::common::TriggerEventShort::Offset>
                           (value, mask, offset);
   return field;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the event word count
  \return    The correctly masked and justified field

  \param[in] eventWordCount The event word count
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerEventShort::addEventWordCount (uint32_t eventWordCount)
{
   uint32_t field = add<uint16_t>(        eventWordCount,
                                  Mask  ::EventWordCount,
                                  Offset::EventWordCount);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the sync flag
  \return    The correctly masked and justified field

  \param[in] syncFlag The sync flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerEventShort::addSyncFlag (bool syncFlag)
{
   uint32_t field = add<bool>(        syncFlag,
                              Mask  ::SyncFlag,
                              Offset::SyncFlag);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief      Creates the field to set the must be value field to its
              default value.
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerEventShort::addMbf0 ()
{
   uint32_t field = add<uint8_t>(        Mbf0,
                                 Mask  ::Mbf0,
                                 Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the must be value field to the
             specified value
  \return    The correctly masked and justified field

  \param[in] mbf0  The value of the must be field

  \note
   Since this field is designed to be a fixed, well-known value, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerEventShort::addMbf0 (uint8_t mbf0)
{
   uint32_t field = add<uint8_t>(        mbf0,
                                 Mask  ::Mbf0,
                                 Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the trigger type to the specified 
             value
  \return    The correctly masked and justified field

  \param[in] type  The trigger type
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerEventShort::addType (uint8_t type)
{
   uint32_t field = add<uint8_t>(        type,
                                 Mask  ::Type,
                                 Offset::Type);
   return   field;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline TriggerEventLong::TriggerEventLong (uint32_t eventWordCount,
                                           bool           syncFlag,
                                           uint8_t            mbf0,
                                           uint8_t            type,
                                           uint64_t  triggerNumber,
                                           uint64_t      timestamp) :
   TriggerEventShort (eventWordCount, 
                      syncFlag,
                      mbf0,
                      type,
                      triggerNumber)
{
   m_timeLo   = timestamp;
   m_numberHi = triggerNumber >> 32;
   m_timeHi   = timestamp     >> 32;
   return;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Long form of the trigger event with the fixed parameters defaulted

  \param[in]      type  The trigger type
  \param[in]    number  The trigger number
  \param[in] timestamp  The trigger time
                                                                          */
/* ---------------------------------------------------------------------- */
inline TriggerEventLong::TriggerEventLong (uint8_t        type,
                                           uint64_t     number,
                                           uint64_t  timestamp) :
   TriggerEventLong (3, 
                     ldmx::data::common::TriggerEventShort::SyncFlag, 
                     ldmx::data::common::TriggerEventShort::Mbf0,
                     type,
                     number, 
                     timestamp)
{
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
