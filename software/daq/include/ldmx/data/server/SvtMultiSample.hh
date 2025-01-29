// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_SVTMULTISAMPLE_HH__
#define __LDMX_DATA_SERVER_SVTMULTISAMPLE_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/SvtMultiSample.hh
  \brief  Defines filling the SVT multi-sample data structure.
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


#include "ldmx/data/common/SvtMultiSample.hh"
#include "ldmx/utl/BfInsert.hh"
#include <new>
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
class SvtMultiSample : public ldmx::data::common::SvtMultiSample
{
public:
   SvtMultiSample () = delete;
   SvtMultiSample (uint32_t         summary);
   SvtMultiSample (uint16_t const samples[6],
                   uint32_t         summary);

public:
   void *operator new (std::size_t                  nbytes, 
                       ldmx::data::server::SvtEvent *event);


public:
   uint16_t *locateSamples    ();
   void      fillMultiSample  (uint16_t const samples[6]);
   void      fillMultiSample  (uint16_t s0,  uint16_t s1,
                               uint16_t s2,  uint16_t s3,
                               uint16_t s4,  uint16_t s5);

   static uint32_t composeSummary (uint8_t    rceAddress,
                                   uint8_t    febAddress,
                                   uint8_t    apvChannel,
                                   uint8_t     apvNumber,
                                   uint8_t hybridAddress,
                                   bool        readError,
                                   bool             tail,
                                   bool             head,
                                   bool       filterFlag);

public:   
   static uint32_t addRceAddress     (uint8_t    rceAddress);
   static uint32_t addFebAddress     (uint8_t    febAddress);
   static uint32_t addApvChannel     (uint8_t    apvChannel);
   static uint32_t addApvNumber      (uint8_t     apvNumber);
   static uint32_t addHybridAddress  (uint8_t hybridAddress);
   static uint32_t addReadError      (bool         readError);
   static uint32_t addTail           ();
   static uint32_t addTail           (bool             tail);
   static uint32_t addHead           ();
   static uint32_t addHead           (bool             head);
   static uint32_t addFilterFlag     (bool        filterFlag);


private:
   template<typename ValueType>
   static uint32_t add (ValueType                                    value, 
                        ldmx::data::common::SvtMultiSample::Mask      mask,
                        ldmx::data::common::SvtMultiSample::Offset offset);
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

   \brief Constructor for 1 SVT multisample data record. This version
          does not fill the samples, allowing that to be done at a later
          time.

  \param[in] summary  the 32-bit summary word
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtMultiSample::SvtMultiSample (uint32_t summary)
{
   m_w32 = summary;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for 1 SVT multisample data record

  \param[in] samples  The 6 samples
  \param[in] summary  the 32-bit summary word
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtMultiSample::SvtMultiSample (uint16_t const samples[6],
                                       uint32_t         summary)
{
   fillMultiSample (samples);
   m_w32 = summary;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief   Composes the bit-packed summary word
   \return  The bit-packed summary word

   \param[in] rceAddress    The 8-bit RCE address/identifier
   \param[in] febAddress    The 8-bit FEB address
   \param[in] apvChannel    The 7-bit APV channel number
   \param[in] apvNumber     The 3-bit APV number
   \param[in] hybridAddress The 2-bit hybrid address
   \param[in] readError     Flag indicating an APV address sync error
   \param[in] tail          The tail flag (this is not used)
   \param[in] head          The head flag (this is not used)
   \param[in] filterFlag    The filter flag

   \par
    This value is generally used as an argument to the constructor.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::composeSummary (uint8_t    rceAddress,
                                                uint8_t    febAddress,
                                                uint8_t    apvChannel,
                                                uint8_t     apvNumber,
                                                uint8_t hybridAddress,
                                                bool        readError,
                                                bool             tail,
                                                bool             head,
                                                bool       filterFlag)
{
   uint32_t summary = addRceAddress    (   rceAddress)
                    | addFebAddress    (   febAddress)
                    | addApvChannel    (   apvChannel)
                    | addApvNumber     (    apvNumber)
                    | addHybridAddress (hybridAddress)
                    | addReadError     (    readError)
                    | addTail          (         tail)
                    | addHead          (         head)
                    | addFilterFlag    (   filterFlag);

   return summary;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Custom SVT Event MultiSample allocator
  \return Pointer to the allocated class
                                                                          */
/* ---------------------------------------------------------------------- */
inline void 
      *SvtMultiSample::operator new (std::size_t                  nbytes, 
                                     ldmx::data::server::SvtEvent *event)
{
   return event->getMultiSample (nbytes);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Locates where the sample data goes
  \return Pointer to where the sample data goes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t *SvtMultiSample::locateSamples ()
{
   return reinterpret_cast<uint16_t *>(m_samples);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Set the multisample data values  by array

  \param[in] samples The 6 sample values
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtMultiSample::fillMultiSample (uint16_t const samples[6])
{
   m_samples[0] = ((samples[1] << 16) | samples[0]);
   m_samples[1] = ((samples[3] << 16) | samples[2]);
   m_samples[2] = ((samples[5] << 16) | samples[4]);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the mutlisample data values by individual values

  \param[in] s0  Sample 0's 16-bit value
  \param[in] s1  Sample 1's 16-bit value
  \param[in] s2  Sample 2's 16-bit value
  \param[in] s3  Sample 3's 16-bit value
  \param[in] s4  Sample 4's 16-bit value
  \param[in] s5  Sample 5's 16-bit value
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtMultiSample::fillMultiSample (uint16_t s0, uint16_t s1,
                                             uint16_t s2, uint16_t s3,
                                             uint16_t s4, uint16_t s5)
{
   m_samples[0] = ((s1 << 16) | s0);
   m_samples[1] = ((s3 << 16) | s2);
   m_samples[2] = ((s5 << 16) | s4);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Convenience method to customize the general template method
          to add a field to be specific for the SVT multi sample data 
          record.

   \param[in]  value  The value of the field to add
   \param[in]   mask  The right justified bit mask of the bit field
   \param[in] offset  The right justified shift
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline uint32_t SvtMultiSample::
                add (ValueType                                   value, 
                     ldmx::data::common::SvtMultiSample::Mask     mask,
                     ldmx::data::common::SvtMultiSample::Offset offset)
{
   uint32_t field = ldmx::utl::bf::
                       add<uint32_t, 
                           ValueType, 
                           ldmx::data::common::SvtMultiSample::Mask, 
                           ldmx::data::common::SvtMultiSample::Offset>
                          (value, mask, offset);
   return field;
}
/* ------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the RCE address

  \param[in] rceAddress The RCE address
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addRceAddress (uint8_t rceAddress)
{
   uint32_t field = add<uint8_t>(        rceAddress,
                                 Mask  ::RceAddress,
                                 Offset::RceAddress);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the FEB address

  \param[in] febAddress The FEB address
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addFebAddress (uint8_t febAddress)
{
   uint32_t field = add<uint8_t>(        febAddress,
                                 Mask  ::FebAddress,
                                 Offset::FebAddress);
   return   field;
}
/* ---------------------------------------------------------------------- */
   


/* ---------------------------------------------------------------------- *//*!

  \brief Set APV channel number

  \param[in] apvChannel The APV channel number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addApvChannel (uint8_t apvChannel)
{
   uint32_t field = add<uint8_t>(        apvChannel,
                                 Mask  ::ApvChannel,
                                 Offset::ApvChannel);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set APV number (0-3)

  \param[in] apvNumber The APV number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addApvNumber (uint8_t apvNumber)
{
   uint32_t field = add<uint8_t>(        apvNumber,
                                 Mask  ::ApvNumber,
                                 Offset::ApvNumber);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set hybrid address (0-3)

  \param[in] hybridAddress The hybrid address
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addHybridAddress  (uint8_t hybridAddress)
{
   uint32_t field = add<uint8_t>(        hybridAddress,
                                 Mask  ::HybridAddress,
                                 Offset::HybridAddress);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the read error flag

  \param[in] readError The read error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addReadError (bool readError)
{
   uint32_t field = add<bool>(         readError,
                              Mask   ::ReadError,
                              Offset::ReadError);
   return  field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the tail flag to its default value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addTail ()
{
   uint32_t field = add<bool>(        Tail,
                              Mask  ::Tail,
                              Offset::Tail);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the tail flag

  \param[in] tail  The tail flag value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addTail (bool tail)
{
   uint32_t field = add<bool>(        tail,
                              Mask  ::Tail,
                              Offset::Tail);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ----------------------------------------------------------------------- *//*!

  \brief Set the head flag to its default value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addHead ()
{
   uint32_t field = add<bool>(        Head,
                              Mask  ::Head,
                              Offset::Head);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the head flag

  \param[in] tail  The head flag value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addHead (bool head)
{
   uint32_t field = add<bool>(        head,
                              Mask  ::Head,
                              Offset::Head);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Set the filter flag

  \param[in] tail  The filter flag value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtMultiSample::addFilterFlag (bool filterFlag)
{
   uint32_t field = add<bool>(        filterFlag,
                              Mask  ::FilterFlag,
                              Offset::FilterFlag);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif



