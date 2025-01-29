// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_SVTHEADER_HH__
#define __LDMX_DATA_SERVER_SVTHEADER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/SvtHeader.hh
  \brief  Defines filling the SVT header structure.
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


#include "ldmx/data/common/SvtHeader.hh"
#include "ldmx/utl/BfInsert.hh"
#include <cinttypes>



/* ====================================================================== */
/* FORWARD REFERENCE                                                      */
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

  \brief  Fill the SVT header
                                                                          */
/* ---------------------------------------------------------------------- */
class SvtHeader : public ldmx::data::common::SvtHeader
{
public:
   SvtHeader () = delete;
   SvtHeader (uint32_t eventCounter, uint8_t rceAddress);
   SvtHeader (uint32_t eventCounter, uint8_t rceAddress, uint32_t mbf0);


public:
   void *operator new (std::size_t                  nbytes, 
                       ldmx::data::server::SvtEvent *event);

public:
   static uint32_t addRceAddress (uint32_t  rceAddress);
   static uint32_t addMbf0       ();
   static uint32_t addMbf0       (uint32_t        mbf0);


private:
   template<typename ValueType>
   static uint32_t add (ValueType value, 
                        ldmx::data::common::SvtHeader::Mask     mask,
                        ldmx::data::common::SvtHeader::Offset offset);
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

  \brief      Minimal SVT header constructor

  \param[in] eventCounter  The 32-bit event counter
  \param[in]   rceAddress  An 8-bit value giving the RCE address/id
                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtHeader::SvtHeader (uint32_t eventCounter,
                             uint8_t    rceAddress)
{
   m_eventCounter = eventCounter;
   m_w32          = addRceAddress (rceAddress) | addMbf0 ();

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Complete SVT header constructor

  \param[in] eventCounter  The 32-bit event counter
  \param[in]   rceAddress  An 8-bit value giving the RCE address/id

                                                                          */
/* ---------------------------------------------------------------------- */
inline SvtHeader::SvtHeader (uint32_t eventCounter, 
                             uint8_t    rceAddress,
                             uint32_t         mbf0)
{
   m_eventCounter = eventCounter;
   m_w32          = addRceAddress (rceAddress) | addMbf0 (mbf0);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Custom SVT Event Header allocator
  \return Pointer to the allocated class
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *SvtHeader::operator new (std::size_t                  nbytes, 
                                      ldmx::data::server::SvtEvent *event)
{
   return event->getHeader (nbytes);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Convenience method to customize the general template method
         to add a field to be specific for the SVT header word.

  \param[in]  value  The value of the field to add
  \param[in]   mask  The right justified bit mask of the bit field
  \param[in] offset  The right justified shift
                                                                          */
/* ---------------------------------------------------------------------- */
template   <typename ValueType>
inline uint32_t SvtHeader::
                add (ValueType                              value, 
                     ldmx::data::common::SvtHeader::Mask     mask,
                     ldmx::data::common::SvtHeader::Offset offset)
{
   uint32_t field = ldmx::utl::bf::
                         add<uint32_t, 
                              ValueType, 
                              ldmx::data::common::SvtHeader::Mask, 
                              ldmx::data::common::SvtHeader::Offset>
                             (value, mask, offset);
   return   field;
}

/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief     Creates the field to set the RCE address field
  \return    The correctly masked and justified field

  \param[in] rceAddress The RCE address
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtHeader::addRceAddress (uint32_t  rceAddress)
{
   uint32_t field = add<uint8_t>(       rceAddress, 
                                 Mask  ::RceAddress,
                                 Offset::RceAddress);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief    Creates the field to set the must be field to its default
             value
  \return    The correctly masked and justified field
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtHeader::addMbf0 ()
{
   uint32_t field = add<uint32_t>(        Mbf0,
                                  Mask  ::Mbf0,
                                  Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!


   \brief    Creates the field to set the must be field to the specified
             value.
  \return    The correctly masked and justified field

  \param[in] mbf0  The must be field value
   
  \note
   Since this field is designed to be a fixed, well-known value, this 
   method is primarily provided for gobbering it so that any downstream
   verifier can be tested.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t SvtHeader::addMbf0 (uint32_t  mbf0)
{
   uint32_t field  = add<uint32_t>(        mbf0,
                                   Mask  ::Mbf0,
                                   Offset::Mbf0);
   return   field;
}
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif






