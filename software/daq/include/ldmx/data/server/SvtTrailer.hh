// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_SVTTRAILER_HH__
#define __LDMX_DATA_SERVER_SVTTRAILER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/SvtTrailer.hh
  \brief  Defines filling the SVT event trailer structure.
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
 * 2019.02.22 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/SvtTrailer.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {

   class SvtEvent;

} /* namespace server                                                     */
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

  \brief  Fill the SVT Multi-Sample data
                                                                          */
/* ---------------------------------------------------------------------- */
class SvtTrailer : public ldmx::data::common::SvtTrailer
{
public:
   SvtTrailer () = delete;
   SvtTrailer (uint16_t multiSampleCount, 
               uint16_t        skipCount);

public:
   void *operator new (std::size_t                  nbytes, 
                       ldmx::data::server::SvtEvent *event);

               
   static uint32_t addMultiSampleCount   (uint16_t multiSampleCount);
   static uint32_t addSkipCount          (uint16_t        skipCount);
   static uint32_t addUndefined0         ();
   static uint32_t addUndefined0         (uint8_t        undefined0);
   static uint32_t addApvSyncError       (bool            syncError);
   static uint32_t addApvFifoBackupError (bool   apvFifoBackupError);
   static uint32_t addUndefined1         ();
   static uint32_t addUndefined1         (uint8_t        undefined1);


private:
   template<typename ValueType> 
   static uint32_t add (ValueType                               value, 
                        ldmx::data::common::SvtTrailer::Mask     mask,
                        ldmx::data::common::SvtTrailer::Offset offset);
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include "ldmx/data/server/SvtEvent.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace server     {
/* ---------------------------------------------------------------------- *//*!

  \brief Svt event trailer constructor

  \param[in] multiSampleCount  The number of multi-sample data records
  \param[in[        skipCount  The number of multi-sample data records
                               dropped because of FIFO overflow errors
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtTrailer::SvtTrailer (uint16_t multiSampleCount, 
                               uint16_t        skipCount)
{
   m_w32 = addMultiSampleCount (multiSampleCount)
         | addSkipCount        (       skipCount);

   m_pad = 0;

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Custom SVT Event Trailer allocator
  \return Pointer to the allocated class
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *SvtTrailer::operator new (std::size_t                  nbytes, 
                                       ldmx::data::server::SvtEvent *event)
{
   /// printf ("SvtTrailer::new %8" PRIx32 "\n", (int)nbytes);
   return event->getTrailer (nbytes);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Convenience method to customize the general template method
           to add a field to be specific for the SVT trailer word.
   \return The properly justfied and masked 32-bit value that can be
           or'd into the trailer word.
            

   \param[in]  value  The value of the field to add
   \param[in]   mask  The right justified bit mask of the bit field
   \param[in] offset  The right justified shift
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline uint32_t SvtTrailer::
             add (ValueType                               value, 
                  ldmx::data::common::SvtTrailer::Mask     mask,
                  ldmx::data::common::SvtTrailer::Offset offset)
{
   uint32_t field = ldmx::utl::bf::
                       add<uint32_t, 
                           ValueType, 
                           ldmx::data::common::SvtTrailer::Mask, 
                           ldmx::data::common::SvtTrailer::Offset>
                          (value, mask, offset);
   return field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the multi-sample count, \e i.e. the number of SVT 
             multi-sample data records and sets it to the specified value
  \return    The correctly masked and justified field

  \param[in] multiSampleCount  The multi sample count
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addMultiSampleCount (uint16_t multiSampleCount)
{
   uint32_t field = add<uint16_t> (        multiSampleCount,
                                   Mask  ::MultiSampleCount,
                                   Offset::MultiSampleCount);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the skip count, this is the number of 
             multi-sample data records dropped because of FIFO overflow
  \return    The correctly masked and justified field

  \param[in] skipCount The skip count
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addSkipCount (uint16_t skipCount)
{
   uint32_t field = add<uint16_t> (        skipCount,
                                   Mask  ::SkipCount,
                                   Offset::SkipCount);
   return field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the undefined field to its default
             value
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addUndefined0 ()
{
   uint32_t field = add<uint8_t> (        Undefined0,
                                  Mask  ::Undefined0, 
                                  Offset::Undefined0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the first undefined field instance
             and sets it to the specified value
  \return    The correctly masked and justified field          

  \param[in] undefined0  The value to set undefined field to


  \note
   Since this field is designed to be a fixed, well-known value, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addUndefined0 (uint8_t undefined0)
{
   uint32_t field = add<uint8_t>(        undefined0,
                                 Mask  ::Undefined0, 
                                 Offset::Undefined0);
   return   field;
}
/* ---------------------------------------------------------------------- */

                         

/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the APV address sync error flag
  \return    The correctly masked and justified field

  \param[in] apvSyncError The APV address sync error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addApvSyncError (bool apvSyncError)
{
   uint32_t field = add<bool>(        apvSyncError,
                              Mask  ::ApvSyncError,
                              Offset::ApvSyncError);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field the APV Fifo backup error field,
             \e i.e. FIFO overflow and sets it to the specified value
  \return    The correctly masked and justified field

  \param[in] apvFifoBackupError  The APV APV Fifo backup error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addApvFifoBackupError (bool apvFifoBackupError)
{
   uint32_t field = add<bool> (        apvFifoBackupError,
                               Mask  ::ApvFifoBackupError,
                               Offset::ApvFifoBackupError);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the second undefined field instance
             and sets it to its default value

  \brief Sets the undefined field to its default value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addUndefined1 ()
{
   uint32_t field = add<uint8_t> (        Undefined1,
                                  Mask  ::Undefined1, 
                                  Offset::Undefined1);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the second undefined field instance
             and sets it to the specified value
  \return    The correctly masked and justified field

  \param[in] undefined1  The value to set undefined field to

  \note
   Since this field is designed to be a fixed, well-known value, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtTrailer::addUndefined1 (uint8_t undefined1)
{
   uint32_t field = add<uint8_t>(        undefined1,
                                 Mask  ::Undefined1, 
                                 Offset::Undefined1);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                        */
/* ====================================================================== */


#endif
