// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_COMMON_TRIGGERHEADER_HH__
#define __LDMX_DATA_COMMON_TRIGGERHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/common/TriggerHeader.hh
  \brief  Defines the layout of the trigger header structure.
  \author JJRussell - russell@slac.stanford.edu

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

   No part of the rogue software platform, including this file, may be 
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

  \brief Defines the bit fields of the 64-bit word that comprises the 
         Trigger Header
                                                                          */
/* ---------------------------------------------------------------------- */
struct TriggerHeader
{

   /* ------------------------------------------------------------------- *\
    |                                                                     |
    | Layout of the trigger header bit fields                             |
    | ---------------------------------------                             |
    |                                                                     |
    |   7: 0  --   8 bits  Block size                                     |
    |  17: 8  --  10 bits  Block number                                   |
    |  21:18  --   4 bits  ID for TI board (must be 0)                    |
    |  26:22  --   5 bits  Board ID (TI specific)                         |
    |  31:27  --   5 bits  Block header indicator, must be 0x10           |
    |                                                                     |
    |  39:32  --   8 bits  Block size                                     |
    |  47:40  --   8 bits  Must be 0x20                                   |
    |  48:48  --   1 bit   Timestamp present                              |
    |  63:49  --  15 bits  0xFF10 >> 1 = 0x7F90                           |
    |                                                                     |
   \* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------ *\
    | Values of the 'must be' fields'                                    |
   \* ------------------------------------------------------------------ */
   static const uint8_t  Mbf0          = 0x20;
   static const uint16_t Mbf1          = (0xFF10 >> 1);
   static const uint8_t  BlockHeaderId = 0x10;
   static const uint8_t  BoardTypeId   = 0x0;
   static const uint8_t  TiBoardId     = 0xa;


   /* ------------------------------------------------------------------- *//*!

     \brief  The size, in bits, of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      BlockSize0    =  8, /*!< Size of Block size (first instance)        */
      BlockNumber   = 10, /*!< Size of Block number                       */
      BoardTypeId   =  4, /*!< Size of Board Type ID                      */
      TIBoardId     =  5, /*!< Size of TI Board identifier                */
      BlockHeaderId =  5, /*!< Size of the Block Header identifier        */

      BlockSize1    =  8,  /*!< Size of Block size field (second instance)*/
      Mbf0          =  8,  /*!< Size of fixed must be 0x20 field          */
      Timestamp     =  1,  /*!< Size of Timestamp present flag            */
      Mbf1          = 15   /*!< Size of fixed must be 0xFF10 >> 1 field   */
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
      BlockSize0    =  0, /*!< Offset to Block size first instance)       */
      BlockNumber   =  8, /*!< Offset to Block number                     */
      BoardTypeId   = 18, /*!< Offset to Board Type ID                    */
      TIBoardId     = 22, /*!< Offset to TI Board identifier              */
      BlockHeaderId = 27, /*!< Offset to the Block Header identifier      */

      BlockSize1    = 32,  /*!< Offset to Block size (second instance)    */
      Mbf0          = 40,  /*!< Offset to fixed must be 0x20              */
      Timestamp     = 48,  /*!< Offset to Timestamp present               */
      Mbf1          = 49   /*!< Offset to fixed must be 0xFF10 >> 1       */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \brief  The right justified bit mask of the various bit fields
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask
   {
      BlockSize0    =   0xff, /*!< Mask for Block size first instance)    */
      BlockNumber   =  0x3ff, /*!< Mask for Block number                  */
      BoardTypeId   =    0xf, /*!< Mask for Board Type ID                 */
      TIBoardId     =   0x1f, /*!< Mask for TI Board identifier           */
      BlockHeaderId =   0x1f, /*!< Mask for the Block Header identifier   */

      BlockSize1    =   0xff, /*!< Mask for Block size (second instance)  */
      Mbf0          =   0xff, /*!< Mask for fixed must be 0x20            */
      Timestamp     =    0x1, /*!< Mask for Timestamp present             */
      Mbf1          = 0x7fff  /*!< Mask for fixed must be 0xFF10 >> 1     */
   };
   /* ------------------------------------------------------------------- */


   uint64_t  m_w64;  /*!< The 64-bit word containing all the bit fields   */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* namespace common                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif



