// -*-Mode: C++;-*-

#ifndef __LDMX_UTL_MESSAGE_HH__
#define __LDMX_UTL_MESSAGE_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/utl/Message.hh
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


#include "ldmx/utl/List.hh"
#include "ldmx/utl/FixedPacket.hh"


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

// <none>

/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief Standard message that can be queued to any list
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
class Message : public ListNode
{
public:
   Message ();
   Message (FixedPacket *fpa);
  ~Message ();
   
public:
   FixedPacket     *m_fpa;  /*!< Where to return the message              */
   CONTENT     *m_content;  /*!< The message contents                     */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Manages a pool of messages
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
class MessagePool : public FixedPacket
{
public:
   MessagePool (int nmessages);
   MessagePool (int nmessages, void *prm);
  ~MessagePool ();

private:
   class Bundle 
   {
      Bundle (FixedPacket *fpa, void *prm) :
         m_fpa (fpa),
         m_prm (prm)
      {
         return;
      }

   private:
      FixedPacket *m_fpa;
      void        *m_prm;
   };
         
       

public:
   void init (FixedPacket      *fpa,
              Message<CONTENT> *msg, 
              int       packet_size, 
              int        pcb_offset);

   void init (Bundle        *bundle, 
              Message<CONTENT> *msg, 
              int       packet_size, 
              int        pcb_offset);

};
/* ---------------------------------------------------------------------- */
} /* namespace utl                                                        */
} /* namespace ldmx                                                       */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructior
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
inline Message<CONTENT>::Message ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Standard constructor

  \param[in]  fpa The parent Fixed Packet Allocator
                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
inline Message<CONTENT>::Message (FixedPacket *fpa) :
   m_fpa (fpa)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Constructs a pool of messages

  \param[in] nmessages The number of messages in the pool

                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
inline MessagePool<CONTENT>::MessagePool (int nmessages) :
   FixedPacket (nmessages, sizeof (CONTENT), 0, init, this)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructs a pool of messages

  \param[in] nmessages The number of messages in the pool

                                                                          */
/* ---------------------------------------------------------------------- */
template<typename CONTENT>
inline MessagePool<CONTENT>::MessagePool (int nmessages, void *prm) :
   FixedPacket (nmessages, sizeof (CONTENT), 0, init, &Bundle (this, prm))
{
   return;
}
/* ---------------------------------------------------------------------- */
} /* namespace utl                                                        */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
