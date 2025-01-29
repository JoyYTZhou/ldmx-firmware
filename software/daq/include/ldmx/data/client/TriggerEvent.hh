// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_TRIGGEREVENT_HH__
#define __LDMX_DATA_CLIENT_TRIGGEREVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/TriggerEvent.hh
  \brief  Defines accessing the Trigger Event data structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   Provides access to the member fields of the Trigger Event data structure.
   The trigger event data comes in both a short and an long form.

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
 * 2019.02.27 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/TriggerEvent.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the short form of the Trigger data
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerEvent : public ldmx::data::common::TriggerEventShort
{
public:
   TriggerEvent () { return; }


public:
   bool     isLongForm        () const;

   uint16_t getEventWordCount () const;
   bool     getSyncFlag       () const;
   uint8_t  getMbf0           () const;
   uint8_t  getType           () const;
   uint32_t getTriggerNumber  () const;

private:
   template<typename ValueType>
   static ValueType get (uint32_t                                         w32,
                         ldmx::data::common::TriggerEventShort::Mask     mask,
                         ldmx::data::common::TriggerEventShort::Offset offset);
};
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the long form of the Trigger data
                                                                          */
/* ---------------------------------------------------------------------- */
class TriggerEventLong : public ldmx::data::client::TriggerEvent,
                         public ldmx::data::common::TriggerEventExtention
{
public:
   TriggerEventLong () { return; }


public:
   uint64_t getTriggerNumber48 () const;
   uint64_t getTime            () const;
} __attribute ((packed));
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
namespace client     {
/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline ValueType TriggerEvent::
                 get (uint32_t                                         w32,
                      ldmx::data::common::TriggerEventShort::Mask     mask,
                      ldmx::data::common::TriggerEventShort::Offset offset)
{
   ValueType value = 
   ldmx::utl::bf::extract<uint32_t,
                          ValueType,
                          ldmx::data::common::TriggerEventShort::Mask,
                          ldmx::data::common::TriggerEventShort::Offset>
                         (w32, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get TI board identifier
  \return  The 5-bit TI board identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool TriggerEvent::isLongForm () const
{
   return getEventWordCount () > 1 ? true : false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get event word count
  \return  The 15-bit event word count

  \par
   This will be 1 if short form and 2 is the long form
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t TriggerEvent::getEventWordCount () const
{
   uint16_t value = get<uint8_t>(                 m_w32,
                                 Mask  ::EventWordCount,
                                 Offset::EventWordCount);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the sync flag
  \return  The sync flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool TriggerEvent::getSyncFlag () const
{
   bool value = get<uint16_t>(           m_w32,
                              Mask  ::SyncFlag, 
                              Offset::SyncFlag);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the \e must \e be field
  \return  The 8-bit value of the \e must \e be field

  \par
   The set value of this field is 0x01
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t TriggerEvent::getMbf0 () const
{
   uint8_t value = get<uint8_t>(       m_w32,
                                Mask  ::Mbf0, 
                                Offset::Mbf0);
   return  value;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief   Get the 48-bit trigger number
  \return  The 48-bit trigger number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TriggerEvent::getTriggerNumber () const
{
   return m_numberLo;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the 48-bit trigger number
  \return  The 48-bit trigger number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TriggerEventLong::getTriggerNumber48 () const
{
   uint64_t n = m_numberHi;
   n          = (n << 32) | m_numberLo;
   return n;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the 48-bit time
  \return  The 48-bit trigger number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TriggerEventLong::getTime () const
{
   uint64_t n = m_timeHi;
   n          = (n << 32) | m_timeLo;
   return n;
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
