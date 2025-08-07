// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_TRACKERHEADER_HH__
#define __LDMX_DATA_CLIENT_TRACKERHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/TrackerHeader.hh
  \brief  Defines accessing the Tracker header structure.
  \author Sophie Middleton

  \par
   This various member fields of the Tracker header structure.

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


#include "ldmx/data/common/TrackerHeader.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the Tracker header
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerHeader : public ldmx::data::common::TrackerHeader
{
public:
   TrackerHeader () { return; }


public:
   uint8_t  getBlockSize0         () const;
   uint16_t getBlockNumber        () const;
   uint8_t  getBoardTypeId        () const;
   uint8_t  getTIBoardId          () const;
   uint8_t  getBlockHeaderId      () const;
   uint8_t  getBlockSize1         () const;
   uint8_t  getMbf0               () const;
   bool     getTimestampPresent   () const;
   uint16_t getMbf1               () const;


private:
   template<typename ValueType>
   static ValueType get (uint64_t                                     w64,
                         ldmx::data::common::TrackerHeader::Mask     mask,
                         ldmx::data::common::TrackerHeader::Offset offset);
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
namespace client     {
/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline ValueType TrackerHeader::
                 get (uint64_t                                     w64,
                      ldmx::data::common::TrackerHeader::Mask     mask,
                      ldmx::data::common::TrackerHeader::Offset offset)
{
   ValueType value =
   ldmx::utl::bf::extract<uint64_t,
                          ValueType,
                          ldmx::data::common::TrackerHeader::Mask,
                          ldmx::data::common::TrackerHeader::Offset>
                         (w64, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the first instance of the block size
  \return  The 8-bit value of the first instance of the block size
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t TrackerHeader::getBlockSize0 () const
{
   uint8_t value = get<uint8_t>(              m_w64,
                                 Mask  ::BlockSize0,
                                 Offset::BlockSize0);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the block number
  \return  The 10-bt block number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t TrackerHeader::getBlockNumber () const
{
   uint16_t value = get<uint16_t>(              m_w64,
                                  Mask  ::BlockNumber,
                                  Offset::BlockNumber);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get value of board type identifier
  \return  The 4-bit board type identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t TrackerHeader::getBoardTypeId () const
{
   uint8_t value = get<uint8_t>(               m_w64,
                                 Mask  ::BoardTypeId,
                                 Offset::BoardTypeId);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get TI board identifier
  \return  The 5-bit TI board identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t TrackerHeader::getTIBoardId () const
{
   uint8_t value = get<bool>(            m_w64,
                             Mask  ::TIBoardId,
                             Offset::TIBoardId);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get second instance of the block size
  \return  The 8-bt value of the second instance of the block size
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t TrackerHeader::getBlockSize1 () const
{
   uint8_t value = get<bool>(             m_w64,
                             Mask  ::BlockSize1,
                             Offset::BlockSize1);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get value of the \e must \e be field
  \return  The 8-bit value of the \e must \e be field

  \par
   This is defined to be 0x20
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t TrackerHeader::getMbf0 () const
{
   uint8_t value = get<uint8_t>(       m_w64,
                                Mask  ::Mbf0,
                                Offset::Mbf0);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the timestamp present flag
  \return  The timestamp present flag

                                                                          */
/* ---------------------------------------------------------------------- */
inline bool TrackerHeader::getTimestampPresent () const
{
   bool   value = get<bool>(            m_w64,
                            Mask  ::Timestamp,
                            Offset::Timestamp);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the value of the second \e must \e be field
  \return  The 15-bi value of the second \e must \e be field

  \par
   This is defined to be 0xFF10 >> 1 = 7F88
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t TrackerHeader::getMbf1 () const
{
   uint16_t value = get<uint16_t>(       m_w64,
                                  Mask  ::Mbf1,
                                  Offset::Mbf1);
   return   value;
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
