// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_TRACKERHEADER_HH__
#define __LDMX_DATA_SERVER_TRACKERHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/TrackerHeader.hh
  \brief  Defines filling the tracker header structure.
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


#include "ldmx/data/common/TrackerHeader.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Fill the tracker header
                                                                          */
/* ---------------------------------------------------------------------- */
class TrackerHeader : public ldmx::data::common::TrackerHeader
{
public:
   TrackerHeader () = delete;

   TrackerHeader (uint8_t         blockSize0,
                  uint16_t       blockNumber,
                  uint8_t        boardTypeId,
                  uint8_t          tiBoardId,
                  uint8_t      blockHeaderId,
                  uint8_t         blockSize1,
                  bool      timestampPresent,
                  uint8_t               mbf0,
                  uint16_t              mbf1);

   TrackerHeader (uint8_t          blockSize,
                  uint16_t       blockNumber,
                  bool      timestampPresent);

public:
   void            setBlockSize (uint8_t blockSize);

public:
   static uint64_t addBlockSize0    (uint8_t     blockSize);
   static uint64_t addBlockNumber   (uint16_t  blockNumber);
   static uint64_t addBoardTypeId   (uint8_t   boardTypeId);
   static uint64_t addTiBoardId     (uint8_t     tiBoardId);
   static uint64_t addBlockHeaderId (uint8_t blockHeaderId);

   static uint64_t addBlockSize1    (uint8_t  blockSize);
   static uint64_t addMbf0          ();
   static uint64_t addMbf0          (uint8_t mbf0);
   static uint64_t addTimestamp     (bool  present);
   static uint64_t addMbf1          ();
   static uint64_t addMbf1          (uint16_t mbf1);

private:
   template  <typename  ValueType>
   static uint64_t add (ValueType                                  value,
                        ldmx::data::common::TrackerHeader::Mask     mask,
                        ldmx::data::common::TrackerHeader::Offset offset);
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
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tracker header

  \note
   There is one tracker header for each batch of tracker events.  There
   is no tracker trailer.
                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerHeader::TrackerHeader (uint8_t         blockSize0,
                                     uint16_t       blockNumber,
                                     uint8_t        boardTypeId,
                                     uint8_t          tiBoardId,
                                     uint8_t      blockHeaderId,
                                     uint8_t         blockSize1,
                                     bool      timestampPresent,
                                     uint8_t               mbf0,
                                     uint16_t              mbf1)
{
   m_w64 = addBlockSize0  (      blockSize0)
         | addBlockNumber (     blockNumber)
         | addBoardTypeId (     boardTypeId)
         | addTiBoardId   (       tiBoardId)
         | addBlockSize1  (      blockSize1)
         | addMbf0        (            mbf0)
         | addTimestamp   (timestampPresent)
         | addMbf1        (            mbf1);

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tracker header with fixed fields filled with
         there default values.

  \note
   There is one tracker header for each batch of tracker events.  There
   is no tracker trailer.


                                                                          */
/* ---------------------------------------------------------------------- */
inline TrackerHeader::TrackerHeader (uint8_t          blockSize,
                                     uint16_t       blockNumber,
                                     bool      timestampPresent) :

   TrackerHeader (blockSize,
                  blockNumber,
                  ldmx::data::common::TrackerHeader::BoardTypeId,
                  ldmx::data::common::TrackerHeader::TiBoardId,
                  ldmx::data::common::TrackerHeader::BlockHeaderId,
                  blockSize,
                  timestampPresent,
                  ldmx::data::common::TrackerHeader::Mbf0,
                  ldmx::data::common::TrackerHeader::Mbf1)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Convenience method to customize the general template method
          to add a field to be specific for the tracker header word.
  \return The properly justfied and masked 64-bit value that can be
          or'd into the trailer word.

  \param[in]  value  The value of the field to add
  \param[in]   mask  The right justified bit mask of the bit field
  \param[in] offset  The right justified shift
                                                                         */
/* --------------------------------------------------------------------- */
template  <typename  ValueType>
inline uint64_t TrackerHeader::
                add (ValueType                                  value,
                     ldmx::data::common::TrackerHeader::Mask     mask,
                     ldmx::data::common::TrackerHeader::Offset offset)
{
   uint64_t field = ldmx::utl::bf::
                          add<uint64_t,
                              ValueType,
                              ldmx::data::common::TrackerHeader::Mask,
                              ldmx::data::common::TrackerHeader::Offset>
                             (value, mask, offset);
     return field;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
  \brief     Creates the field to set the first instance of block size
             field.
  \return    The correctly masked and justified field

  \param[in] blockSize The block size
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addBlockSize0  (uint8_t blockSize)
{
   uint64_t field = add<uint8_t>(        blockSize,
                                 Mask  ::BlockSize0,
                                 Offset::BlockSize0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Set the two blockSize fields with the specified values

  \param[in] blockSize  The block size (actually the tracker event count)
                        to add
                                                                          */
/* ---------------------------------------------------------------------- */
inline void  TrackerHeader::setBlockSize (uint8_t blockSize)
{
   m_w64 |= addBlockSize0 (blockSize)
         |  addBlockSize1 (blockSize);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the block number.
  \return    The correctly masked and justified field

  \param[in] blockNumber The block nubmer
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addBlockNumber (uint16_t blockNumber)
{
   uint64_t field = add<uint16_t>(        blockNumber,
                                  Mask  ::BlockNumber,
                                  Offset::BlockNumber);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the board type identifier.
  \return    The correctly masked and justified field

  \param[in] blockTypeId The board type identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addBoardTypeId (uint8_t boardTypeId)
{
   uint64_t field = add<uint8_t>(        boardTypeId,
                                 Mask  ::BoardTypeId,
                                 Offset::BoardTypeId);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the TI board identifier
  \return    The correctly masked and justified field

  \param[in] tiBoardTypeId The TI board identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addTiBoardId (uint8_t tiBoardId)
{
   uint64_t field = add<uint8_t> (        tiBoardId,
                                  Mask  ::TIBoardId,
                                  Offset::TIBoardId);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the block header identifier
  \return    The correctly masked and justified field

  \param[in] tiBoardTypeId The TI board identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addBlockHeaderId (uint8_t blockHeaderId)
{
   uint64_t field = add<uint8_t> (        blockHeaderId,
                                  Mask  ::BlockHeaderId,
                                  Offset::BlockHeaderId);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the second instance of the block
             size
  \return    The correctly masked and justified field

  \param[in] blockSize The block size
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addBlockSize1 (uint8_t blockSize)
{
   uint64_t field = add<uint8_t>(        blockSize,
                                 Mask  ::BlockSize1,
                                 Offset::BlockSize1);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the first instance of the must be
             value field to its default value.
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addMbf0 ()
{
   uint64_t field = add<uint8_t>(        Mbf0,
                                 Mask  ::Mbf0,
                                 Offset::Mbf0);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the first instance of the must be
             value field to the specified value.
  \return    The correctly masked and justified field

  \param[in] mbf0  The value of the must be field

  \note
   Since this field is designed to be a fixed, well-known value, this
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addMbf0 (uint8_t mbf0)
{
   uint64_t field = add<uint8_t>(        mbf0,
                                 Mask  ::Mbf0,
                                 Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the boolean flag indicating whether
             a timestamp is present
  \return    The correctly masked and justified field

  \param[in] present  Flag indicating the presence/absence of the timestamp
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addTimestamp (bool present)
{
   uint64_t field = add<bool>(        present,
                              Mask  ::Timestamp,
                              Offset::Timestamp);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the second instance of the must be
             value field to its default value.
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addMbf1 ()
{
   uint64_t field = add<uint16_t>(        Mbf1,
                                  Mask  ::Mbf1,
                                  Offset::Mbf1);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the second instance of the must be
             value field to the specified value
  \return    The correctly masked and justified field

  \param[in] mbf1  The value of the must be field

  \note
   Since this field is designed to be a fixed, well-known value, this
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TrackerHeader::addMbf1 (uint16_t mbf1)
{
   uint64_t field = add<uint16_t>(        mbf1,
                                  Mask  ::Mbf1,
                                 Offset::Mbf1);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
