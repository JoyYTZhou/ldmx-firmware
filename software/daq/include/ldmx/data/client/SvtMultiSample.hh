// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_CLIENT_SVTMULTISAMPLE_HH__
#define __LDMX_DATA_CLIENT_SVTMULTISAMPLE_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/client/SvtMultiSample.hh
  \brief  Defines accessing the SVT multi-sample structure.
  \author JJRussell - russell@slac.stanford.edu

  \par
   This various member fields of the SVT multi-sample record.


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
 * 2019.02.27 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/data/common/SvtMultiSample.hh"
#include "ldmx/utl/BfExtract.hh"
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Class to access the members of the SVT multi-sample data record
                                                                          */
/* ---------------------------------------------------------------------- */
class SvtMultiSample : public ldmx::data::common::SvtMultiSample
{
public:
   SvtMultiSample () = delete;


public:
   uint16_t const *locateSamples   () const;

   uint8_t  getRceAddress    () const;
   uint8_t  getFebAddress    () const;
   uint8_t  getApvChannel    () const;
   uint8_t  getApvNumber     () const;
   uint8_t  getHybridAddress () const;
   bool     getReadError     () const;
   bool     getTail          () const;
   bool     getHead          () const;
   bool     getFilterFlag    () const;

   static void printTitle ();
   static void print (SvtMultiSample const *ms,
                      int                  ims);
   void        print (int                  ims) const;


private:
   template<typename ValueType>
   static ValueType get (uint32_t                                      w32,
                         ldmx::data::common::SvtMultiSample::Mask     mask,
                         ldmx::data::common::SvtMultiSample::Offset offset);
};
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */




#include <cstdio>

/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx       {
namespace data       {
namespace client     {
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the 8 samples
  \return A pointer to the 8 samples
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t const *SvtMultiSample::locateSamples () const
{
   return reinterpret_cast<uint16_t const *>(m_samples);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
template<typename ValueType>
inline ValueType SvtMultiSample::
                 get (uint32_t                                      w32,
                      ldmx::data::common::SvtMultiSample::Mask     mask,
                      ldmx::data::common::SvtMultiSample::Offset offset)
{
   ValueType value = 
   ldmx::utl::bf::extract<uint32_t,
                          ValueType,
                          ldmx::data::common::SvtMultiSample::Mask,
                          ldmx::data::common::SvtMultiSample::Offset>
                         (w32, mask, offset);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the RCE address/identifier
  \return  The 8-bit value of the RCE address/identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t SvtMultiSample::getRceAddress () const
{
   uint8_t value = get<uint8_t>(             m_w32,
                                Mask  ::RceAddress,
                                Offset::RceAddress);
   return value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the FEB address/identifier
  \return  The 8-bit value of the FEB address/identifier
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t SvtMultiSample::getFebAddress () const
{
   uint8_t value = get<uint8_t>(             m_w32,
                                Mask  ::FebAddress, 
                                Offset::FebAddress);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the APV channel number
  \return  The 7-bit APV channel number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t SvtMultiSample::getApvChannel () const
{
   uint8_t value = get<uint8_t>(             m_w32, 
                                Mask  ::ApvChannel, 
                                Offset::ApvChannel);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the APV number
  \return  The 3-bit APV number
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t SvtMultiSample::getApvNumber () const
{
   uint8_t value = get<uint8_t>(            m_w32, 
                                Mask  ::ApvNumber, 
                                Offset::ApvNumber);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the hybrid address
  \return  The 2-bit hybrid address
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint8_t SvtMultiSample::getHybridAddress () const
{
   uint8_t value = get<uint8_t>(                m_w32, 
                                Mask  ::HybridAddress, 
                                Offset::HybridAddress);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the read error flag
  \return  The read error flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool SvtMultiSample::getReadError () const
{
   uint8_t value = get<bool>(            m_w32, 
                             Mask  ::ReadError, 
                             Offset::ReadError);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the tail flag
  \return  The tail flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool SvtMultiSample::getTail () const
{
   uint8_t value = get<bool>(       m_w32, 
                             Mask  ::Tail, 
                             Offset::Tail);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the head flag
  \return  The head flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool SvtMultiSample::getHead () const
{
   uint8_t value = get<bool>(       m_w32, 
                             Mask  ::Head, 
                             Offset::Head);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the filter flag
  \return  The filter flag
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool SvtMultiSample::getFilterFlag () const
{
   uint8_t value = get<bool>(       m_w32, 
                             Mask  ::FilterFlag, 
                             Offset::FilterFlag);
   return  value;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the multi-sample data record

  \param[in]  ims  The instance number of the multi-sample data record
                                                                          */
/* ---------------------------------------------------------------------- */
inline void SvtMultiSample::print (int ims) const
{
   print (this, ims);
}
/* ---------------------------------------------------------------------- */
} /* namespace client                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */

#endif
