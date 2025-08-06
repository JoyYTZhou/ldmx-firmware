// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_TRACKEREVENT_HH__
#define __LDMX_DATA_SERVER_TRACKEREVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TrackerEvent.hh
  \brief  Defines filling the tracker data structure.
  \author Sophie Middleton
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



#include "ldmx/data/common/TrackerEvent.hh"
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

  \brief  Fill the short form of the tracker data
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerEventShort : public ldmx::data::common::TrackerEventShort
{
public:
   TrackerEventShort () = delete;

   TrackerEventShort (uint8_t     type,
                      uint32_t  number);

   TrackerEventShort (uint32_t eventWordCount,
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
                        ldmx::data::common::TrackerEventShort::Mask     mask,
                        ldmx::data::common::TrackerEventShort::Offset offset);
};
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Fill the long form of the tracker event data
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerEventLong : public ldmx::data::server::TrackerEventShort,
                         public ldmx::data::common::TrackerEventExtention
{
public:
   TrackerEventLong () = delete;

   TrackerEventLong (uint8_t        type,
                     uint64_t     number,
                     uint64_t  timestamp);

   TrackerEventLong (uint32_t eventWordCount,
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
inline TrackerEventShort::TrackerEventShort (uint32_t eventWordCount,
                                             bool           syncFlag,
                                             uint8_t            mbf0,
                                             uint8_t            type,
                                             uint32_t  trackerNumber)
{
   m_w32 = addEventWordCount (eventWordCount)
         | addSyncFlag       (      syncFlag)
         | addMbf0           (          mbf0)
         | addType           (          type);

   m_numberLo = trackerNumber;

   printf ("EventWordCount: %8.8" PRIx32 " sync:%1d mbf0:%2.2" PRIx8 ""
           "Type:%2.2" PRIx8 "trgNum:%8.8" PRIx32 "\n",
           eventWordCount, syncFlag, mbf0, type, trackerNumber);
   printf ("W32 = %8.8" PRIx32 "\n", m_w32);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline TrackerEventShort::TrackerEventShort (uint8_t     type,
                                             uint32_t  number)
{
   m_w32 = addEventWordCount (       1)
         | addSyncFlag       (ldmx::data::common::TrackerEventShort::SyncFlag)
         | addMbf0           (ldmx::data::common::TrackerEventShort::Mbf0)
         | addType           (    type);

   m_numberLo = number;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Convenience method to customize the general template method
         to add a field to be specific for the tracker event data word.

  \param[in]  value  The value of the field to add
  \param[in]   mask  The right justified bit mask of the bit field
  \param[in] offset  The right justified shift
                                                                          */
/* ---------------------------------------------------------------------- */
template   <typename ValueType>
inline uint32_t TrackerEventShort::
                add (ValueType                                      value,
                     ldmx::data::common::TrackerEventShort::Mask     mask,
                     ldmx::data::common::TrackerEventShort::Offset offset)
{
   uint32_t field = ldmx::utl::bf::
                        add<uint32_t,
                            ValueType,
                            ldmx::data::common::TrackerEventShort::Mask,
                            ldmx::data::common::TrackerEventShort::Offset>
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
inline uint32_t TrackerEventShort::addEventWordCount (uint32_t eventWordCount)
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
inline uint32_t TrackerEventShort::addSyncFlag (bool syncFlag)
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
inline uint32_t TrackerEventShort::addMbf0 ()
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
inline uint32_t TrackerEventShort::addMbf0 (uint8_t mbf0)
{
   uint32_t field = add<uint8_t>(        mbf0,
                                 Mask  ::Mbf0,
                                 Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the tracker type to the specified
             value
  \return    The correctly masked and justified field

  \param[in] type  The tracker type
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TrackerEventShort::addType (uint8_t type)
{
   uint32_t field = add<uint8_t>(        type,
                                 Mask  ::Type,
                                 Offset::Type);
   return   field;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline TrackerEventLong::TrackerEventLong (uint32_t eventWordCount,
                                           bool           syncFlag,
                                           uint8_t            mbf0,
                                           uint8_t            type,
                                           uint64_t  trackerNumber,
                                           uint64_t      timestamp) :
   TrackerEventShort (eventWordCount,
                      syncFlag,
                      mbf0,
                      type,
                      trackerNumber)
{
   m_timeLo   = timestamp;
   m_numberHi = trackerNumber >> 32;
   m_timeHi   = timestamp     >> 32;
   return;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Long form of the tracker event with the fixed parameters defaulted

  \param[in]      type  The tracker type
  \param[in]    number  The tracker number
  \param[in] timestamp  The tracker time
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerEventLong::TrackerEventLong (uint8_t        type,
                                           uint64_t     number,
                                           uint64_t  timestamp) :
   TrackerEventLong (3,
                     ldmx::data::common::TrackerEventShort::SyncFlag,
                     ldmx::data::common::TrackerEventShort::Mbf0,
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
