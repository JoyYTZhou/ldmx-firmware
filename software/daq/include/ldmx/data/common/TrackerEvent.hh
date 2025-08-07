// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_TRACKEREVENT_HH__
#define __LDMX_DATA_COMMON_TRACKEREVENT_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/TrackerEvent.hh
  \brief  Defines the layout of tracker event data structure.
  \author Sophie Middleton

  \par
   This file defines the only the data layout, not how to write or
   access this data and is meant to be the base class for derived
   classes that do the accessing and writing.  No methods are defined,
   so that they are POD (plain old data), free of any C++ decorations.

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
 * 2019.02.20 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <cinttypes>


/* ====================================================================== */
namespace ldmx       {
namespace data       {
namespace common     {
/* ---------------------------------------------------------------------- *//*!

  \brief Defines the bit fields and the optional 32-bit words that
         comprise the short form of the Tracker Event data
                                                                          */
/* ---------------------------------------------------------------------- */
struct TrackerEventShort
{
   /* ------------------------------------------------------------------- *\
    |                                                                     |
    |  Word 0:                                                            |
    |    31:24 -- 8 bits   Tracker Type                                   |
    |    23:16 -- 8 bits   Must be 0x01                                   |
    |    15:15 -- 1 bit    SYNC Flag                                      |
    |    14:00 -- 15 bits  Event word count,                              |
    |                      word 0 is excluded from the count              |
    |                                                                     |
    |  Word 1:                                                            |
    |    31:00 = 32 bits   Tracker number                                 |
    |                                                                     |
    |                                                                     |
    |   Word 2: Included if Event word count is greater than 1            |
    |    31:00 = 32 bits   Tracker timing bits 31:0 in 4ns steps          |
    |                                                                     |
    |                                                                     |
    |  Word 3: Included if Event word count is greater than 1             |
    |    31:16 = 16 bits   Tracker number bits 47:32,                     |
    |                      to form 48 bit counter with word 1             |
    |    15:00 = 16 bits   Tracker timing bits 47:32,                     |
    |                      to form 48 bit timing with word 2              |
    |                                                                     |
   \* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------ *\
    | Values of the 'must be' fields'                                    |
   \* ------------------------------------------------------------------ */
   static const uint8_t    Mbf0 = 0x01;
   static const bool   SyncFlag = true;


   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      EventWordCount = 15,  /*!< Size of Event word count                 */
      SyncFlag       =  1,  /*!< Size of Sync flag                        */
      Mbf0           =  8,  /*!< Size of must be field (first instance)   */
      Type           =  8   /*!< Size of Tracker Type                     */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The offset, in bits, to the various bit fields

     \note
      This is the value needed to right justify the field
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Offset
   {
      EventWordCount =  0,  /*!< Offset to Event word count               */
      SyncFlag       = 15,  /*!< Offset to Sync flag                      */
      Mbf0           = 16,  /*!< Offset to must be field (first instance) */
      Type           = 24   /*!< Offset to tracker Type                   */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      EventWordCount = 0x7fff, /*!< Mask of Event word count              */
      SyncFlag       =    0x1, /*!< Mask of Sync flag                     */
      Mbf0           =   0xff, /*!< Mask of must be field (first instance)*/
      Type           =   0xff  /*!< Mask of Tracker Type                  */
   };


   /* ------------------------------------------------------------------- *\
    | Only the first word is mandatory, the presence of the others        |
    | depend on the EventWordCount                                        |
   \* ------------------------------------------------------------------- */
   uint32_t      m_w32;    /*!< First 32-bit field                        */
   uint32_t m_numberLo;    /*!< Low 32-bits of the tracker number         */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Defines the bit fields and the optional 48-bit timestamp and
         field that extends the tracker number to 48-bits
         comprise the long form of the Tracker Event data
                                                                          */
/* ---------------------------------------------------------------------- */
struct TrackerEventExtention
{
   uint32_t   m_timeLo;    /*!< Low 32-bits of the tracker time           */
   uint16_t   m_timeHi;    /*!< High 16-bits of the tracker time          */
   uint16_t m_numberHi;    /*!< High 16-bits of the tracker number        */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
