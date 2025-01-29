// -*-Mode: C++;-*-


#ifndef __LDMX_UTL_FIXED_PACKET_HH__
#define __LDMX_UTL_FIXED_PACKET_HH__


/* ---------------------------------------------------------------------- *//*!
   
  \file   ldmx/utl/FixedPacket.hh
  \brief  Fixed Packet Allocator, memory manager that configures and
          manages a pool of fixed sized packets
  \author JJRussell - russell@slac.stanford.edu


  \b SYNOPSIS \n
   This utility allows one to configure a pool of memory as a collection of
   fixed sized packets. The allocation and deallocation is interlocked, making
   it safe to use in a multi-threaded environment.

   The behaviour when the pool is exhausted is configurable to be either
   blocking or non-blocking. If blocking is chosen, then the blocking can be
   in either FIFO or PRIORITY ordering. This is relevant only when multiple
   allocators use the pool. 

   An initial design choice was to hide both the control structure for the
   pool and the packet management structure. The former is an easy decision,
   the latter caused awkward programming at times,  but would increase
   modularity. The pain is almost exclusively limited to initialization. It
   was  an experiment to see if this worked, but given the rather limited
   exposure (the packet management structure is currently only ListNode
   structure, a single 4-byte word), it was decided to keep the control
   structure hidden, but expose the packet control structure.


  \b USAGE/EXAMPLE \n
   Here is how one would typically use this facility. Suppose one wished to
   manage a pool of packets which contained data which was to be read
   from an external source. Further suppose that each packet was to be have
   a fixed header constrained to be the first word of the data packet
   describing the data type and that this type never changes. This makes
   the \e data_type field a candidate for one-time initialization.  Since
   the \e initializer routine writes the \e data_type field, the packet
   control block used to manage the packet when it is on the freelist
   must not be allowed to occupy the first word. Since, presumably, the
   body of the structure is filled with fresh data each time, the packet
   control block can be located anywhere within that section. The choice
   in the example is to locate it as the first word of the data field.

  \code

  struct _MyPacket
    {                       / * Packet consists of a header ...         * /
        int data_type;      / * Header gives the data type              * /
        union _MyData       / * and a body                              * /
        {                     
            FixedPacket:Pcb pcb;  
                             / * Management when packet is not allocated * /
            char  buf[60];   / * Holds the data when allocated          * /
        } body;
    };
    
    FixedPacket:Fcb    *fcb;
    char   *buffer[1000];

    fcb = malloc (FixedPacket::sizeofFcb ());
                                        / * Allocate the control block * /

    / * Use placment new to locate the Fixed Packet Allocator          * /
    fcb = FixedPacket
             (fcb,                      / * The control structure      * /
              buffer,                   / * Memory pool to manage      * /
              sizeof(buffer),           / * Size of the memory pool    * /
              sizeof(struct MyPacket),  / * Size of each packet        * /
              OFFSET_OF(_struct _MyPacket,
                        body.pcb),      / * Byte offset of PCB         * /
              (FPA_init_cb)initializer, / * Initialization routine     * /
              (void *)data_type);       / * Data type of each packet   * /
    .
    .
    .
    while (ptr = isDataReady ())
    {
        struct _MyPacket *packet = fcb->getW ();
        copy (packet->body.buf, ptr, sizeof (packet->body.buf));
    }
              
    return;
    }


    
    / * Callback routine to initialize each packet * /
    void initializer (int           data_type,
                      struct MyPacket *packet,
                      int         packet_size,
                      int          pcb_offset)
    {
        packet->data_type = data_type;
    }

   \endcode


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
                                                                          */
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- ---------------------------------------------------------
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.01.18 jjr Cloned from EXO version of FPA.h, mashed into C++
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/List.hh"


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
struct timespec;
/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \class FixedPacket
  \brief Class implementing the fixed packet allocator

  \par
   This is done using a pointer implementation model (pImpl).  Doing this
   hides the details of the layout of the class memebers from the user,
   basically implementing almost a pure interface.
                                                                          */
/* ---------------------------------------------------------------------- */
class FixedPacket
{
public:
   class Fcb;   /*!< Fixed Packet Allocator control block                 */
   class Pcb;   /*!< Individual Packet control block                      */


public:
   /* ------------------------------------------------------------------- *//*!

     \var    PlaceAtEop
     \brief  Convenience symbol indicating that the packet control block,
             should be placed at the end of the packet (EOP).
                                                                          */
   /* ------------------------------------------------------------------- */
   static const int PlaceAtEop = -1;
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

     \typedef InitCb (void *parameter,
                      void    *packet,
                      int packet_size,
                      int  pcb_offset)
     \brief   Function signature for the packet initialization callback
              routine
           
     \param parameter   A user provided context parameter
     \param packet      The packet being initialized
     \param packet_size The size, in bytes, of the packet.
     \param pcb_offset  The offset, in bytes, to the packets management
                        links

      The callback routine must take care not to write in the area 
      reserved for the Packet Control Block. The last parameter of the
      callback \e pcb_offset allows the user to determine exactly what 
      memory the packet control block occupies so that this memory can 
      be avoided during initialization.
                                                                          */
   /* ------------------------------------------------------------------- */
   typedef void (*InitCb)(void  *parameter,
                          void     *packet,
                          int  packet_size,
                          int   pcb_offset);
   /* ------------------------------------------------------------------- */



public:
   // -----------------
   // Dummy constructor
   // -----------------
   FixedPacket ();

   // `--------------------------------------------------------------
   //  Highly simplified Constructors that allocate memory internally
   //  The user has no control over where and how memory for the
   //  control structure and packet pool is allocated.
   // `--------------------------------------------------------------
   FixedPacket (int             packet_count,
                int             packet_size);

   FixedPacket (int            packet_count,
                int             packet_size,
                int              pcb_offset,
                InitCb         init_routine,
                void        *init_parameter);


   // -------------------------------------------------------------
   // Construct to be used when memory for the FixedPacket class is
   // in place, but the implementation class (fcb) and the buffer
   // pool are to be carved from the memory starting at the fcb.
   //
   // This is a very common use case, with the user directly 
   // declaring the FixedPacket class, but allocating memory for
   // implementation class and the packet pool.
   // -------------------------------------------------------------
   FixedPacket (void                   *fcb,
                int                buf_size,
                int             packet_size);

   FixedPacket (void                   *fcb,
                int                buf_size,
                int             packet_size,
                int              pcb_offset,
                InitCb         init_routine,
                void        *init_parameter);



   // -------------------------------------------------------------
   // Constructors that can be used to place the control structures
   // and the buffer pool in distinct pieces of memory
   // -------------------------------------------------------------
   FixedPacket (FixedPacket::Fcb       *fcb,
                void                *buffer,
                int                buf_size,
                int             packet_size);

   FixedPacket (FixedPacket::Fcb       *fcb,
                void                *buffer,
                int                buf_size,
                int             packet_size,
                int              pcb_offset,
                InitCb         init_routine,
                void        *init_parameter);

  ~FixedPacket ();


public:
   // -------------------------------------------------------------------
   // Helper meethods used to size
   //   1) the underlying control class (Fcb)
   //
   //   2) the buffer pool
   //
   //   3) an FixedPacket allocator for a specified pool_size.
   //      This is used when placing all the elements in the same block
   //      of memory.
   // -------------------------------------------------------------------
   static int sizeofFcb  ();
   static int sizeofPool (int    npackets,
                          int packet_size);


   static FixedPacket::Fcb  *allocateFcb (int npackets, int packet_size);
   static FixedPacket::Fcb *constructFcb (int npackets, int packet_size);
   static FixedPacket::Fcb *constructFcb (int      packet_count,
                                          int       packet_size,
                                          int        pcb_offset,
                                          InitCb   init_routine,
                                          void  *init_parameter);



   // -------------------------------------------------------------------
   // Allocators
   //  1) get  -- returns immediately, returns NULL if no packet is 
   //             available
   //
   //  2) getW -- waits (with no timeout), indefinitely until a packet
   //             becomes available; never return NULL (unless an error)
   //
   //  3) getW -- waits (with a timeout), until a packet is available or
   //             the timeout period expires in which case NULL is 
   //             returned.
   // -------------------------------------------------------------------
   void *get  ();
   void *getW ();
   void *getW (struct timespec const *to);


   // -----------------------------
   // Return the packet to the pool
   // -----------------------------
   int   free (void *packet);

private:
   FixedPacket::Fcb *m_fcb;  /*!< The underlying implementation class     */
};
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!

  \brief Dummy constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline FixedPacket::FixedPacket ()
{
   return;
}
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */


#endif
