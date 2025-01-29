// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Message.cc
  \brief  A generic message that can be inserted on a list
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the LDMX software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.04.02 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/Message.hh"


/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief Callback to initialize a message

  \param[in] fpa         The parent Fixed Packet Allocator
  \param[in] msg         The message to initialize
  \param[in] packet_size The size, in bytes, of the packet
  \param[in] pcb_offset  The offset to the packet control block

                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
void MessagePool<CONTENT>::init (FixedPacket      *fpa,
                                 Message<CONTENT> *msg,
                                 int       packet_size, 
                                 int        pcb_offset)
{
   msg->m_fpa = fpa;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Callback to initialize a message

  \param[in] bundle      The Fixed Packet Allocate and user parameter
  \param[in] msg         The message to initialize
  \param[in] packet_size The size, in bytes, of the packet
  \param[in] pcb_offset  The offset to the packet control block

                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
void MessagePool<CONTENT>::init (Bundle        *bundle,
                                 Message<CONTENT> *msg,
                                 int       packet_size, 
                                 int        pcb_offset)
{
   msg->m_fpa = bundle->fpa;
   new (msg->m_content) Message<CONTENT>(bundle->m_prm);
   return;
}
/* ---------------------------------------------------------------------- */

} /* namespace utl                                                        */
} /* namespace ldmx                                                       */
/* ====================================================================== */
