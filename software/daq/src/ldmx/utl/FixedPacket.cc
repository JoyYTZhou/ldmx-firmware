// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
   
  \file  ldmx/utl/FixedPacket.cc
  \brief Utility for configuring and allocating a pool of fixed sized 
         packets
  \author JJRussell - russell@slac.stanford.edu
   
  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 
 
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
 *  HISTORY
 *  -------
 *
 * DATE       WHO WHAT
 * ---------- --- -------------------------------------------------------
 * 2021.04.20 jjr Adapted from HPS version
 * 2019.01.18 jjr Cloned for EXO version
\* ---------------------------------------------------------------------- */


#include "ldmx/utl/FixedPacket.hh"
#include "ldmx/utl/ResourceWait.hh"
#include "ldmx/utl/List.hh"
#include <time.h>
#include <new>
#include <cinttypes>


/* ====================================================================== */
namespace ldmx {
namespace utl  {
/* ---------------------------------------------------------------------- *//*!
   
  \class   FixedPacket::Fcb
  \brief   Layouts the control structure for managing the list of free
           packets. This structure is private to this utility.
                                                                          */
/* ---------------------------------------------------------------------- */
class FixedPacket::Fcb
{
public:
   Fcb () = delete;

   Fcb (void                     *buffer,
        int                     buf_size,
        int                  packet_size);
        
   Fcb (void                     *buffer,
        int                     buf_size,
        int                  packet_size,
        int                   pcb_offset,
        FixedPacket::InitCb init_routine,
        void             *init_parameter); 

  ~Fcb ();


   void *get      ();
   void *getW     ();
   void *getW     (struct timespec const *to);
   int   free     (void              *packet);


public:
   List                m_list;  /*!< The controlling list                 */
   ResourceWait          m_rw;  /*!< Resource wait control                */
   unsigned int m_outstanding;  /*!< The number of outstanding packets    */
   unsigned int     m_pcktcnt;  /*!< Total number of packets              */
   unsigned int    m_pcktsize;  /*!< Size of an individual packet         */
   unsigned int      m_pcboff;  /*!< Offset to the packet control block   */
   void             *m_buffer;  /*!< The user buffer                      */
   unsigned int     m_bufsize;  /*!< Size of the original pool            */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
class FixedPacket::Pcb : public ListNode
{
   Pcb() = delete;
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates memory for the FixedPacket implementation class and
          its associated buffer pool
  \return The allocated memory

  \param[in] packet_count The number of packets in the pool
  \param[in] packet_size  The size of each packet

  \note
   This is a convenience function. It main use is in conjunction with the
   constructor

          FixedPacket;:Fcb *fcb = FixedPacket::allocateFcb (packet_count,
                                                            packet_size);
          FixedPacket::Fcb (fcb)
       
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::Fcb *FixedPacket::allocateFcb (int packet_count,
                                            int  packet_size)
{
   int         sizeofFcb = FixedPacket::sizeofFcb ();
   int          poolSize = FixedPacket::sizeofPool (packet_count, packet_size);
   int              size = sizeofFcb + poolSize;
   uint64_t         *p64 = new uint64_t[size/sizeof (uint64_t)];
   FixedPacket::Fcb *fcb = reinterpret_cast<decltype(fcb)>(p64);

   fcb->m_pcktcnt  = packet_count;
   fcb->m_pcktsize = packet_size;
   fcb->m_bufsize  = poolSize;

   return fcb;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
FixedPacket::Fcb *FixedPacket::constructFcb (int packet_count,
                                             int  packet_size)
{
   FixedPacket::Fcb *fcb = allocateFcb (packet_count, packet_size);
   new (fcb) Fcb (fcb + 1, fcb->m_bufsize, fcb->m_pcktsize);
   return fcb;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
FixedPacket::Fcb *FixedPacket::constructFcb (int      packet_count,
                                             int       packet_size,
                                             int        pcb_offset,
                                             InitCb   init_routine,
                                             void  *init_parameter)
{
   FixedPacket::Fcb *fcb = allocateFcb (packet_count, packet_size);

   new (fcb) Fcb (fcb + 1, 
                  fcb->m_bufsize, 
                  fcb->m_pcktsize,
                  pcb_offset,
                  init_routine,
                  init_parameter);
   return fcb;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Internal helper routine to locate the buffet pool relative to the
          FixedPacket memory.
  \return Pointer to the memory for the FixedPacket's pool of packets

  \param[in] fpa  Pointer to the FixedPacket allocator class
                                                                          */
/* ---------------------------------------------------------------------- */
inline static void *locateBuffer (FixedPacket *fpa)
{

   // Buffer is located after the FixedPacket class and the Fcb
   void  *buffer = reinterpret_cast<char *>(fpa) 
                 + sizeof (*fpa)
                 + sizeof (FixedPacket::Fcb);

   return buffer;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Internal helper routine to locate the buffet pool relative to the
          FixedPacket memory.
  \return Pointer to the memory for the FixedPacket's pool of packets

  \param[in] fcb  Pointer to the FixedPacket allocator implementation
                  class
                                                                          */
/* ---------------------------------------------------------------------- */
inline static void *locateBuffer (FixedPacket::Fcb *fcb)
{

   // Buffer is located after the FixedPacket class and the Fcb
   void  *buffer = reinterpret_cast<char *>(fcb) 
                 + sizeof (FixedPacket::Fcb);

   return buffer;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Basic constructor to fully initialize the Fixed Packet Allocator

  \param[in]      buf_size  The size, in bytes, of the buffer pool
  \param[in]   packet_size  The size, in bytes, of one packet

                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::FixedPacket (int            packet_count,
                          int             packet_size) :
   m_fcb (constructFcb (packet_count, packet_size))
{
   return;
}

/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Basic constructor to fully initialize the Fixed Packet Allocator

  \param[in]  packet_count  The number of packets in the pool
  \param[in]   packet_size  The size, in bytes, of one packet
  \param[in]    pcb_offset  The offset, in bytes, to the packet's control
                            block
  \param[in]  init_routine  Packet initialization routine
  \param[in]      init_ctx  Packet initialization context
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::FixedPacket (int            packet_count,
                          int             packet_size,
                          int              pcb_offset,
                          InitCb         init_routine,
                          void        *init_parameter) :
   m_fcb (constructFcb (packet_count,     // Packet count   
                        packet_size,      // Packet size
                        pcb_offset,       // Offset to the packet control block
                        init_routine,     // Initialization callback 
                        init_parameter))  // Initialization parameter
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor to completing the initialization of the 
          implementation class

  \param[in]           fcb  Pointer to memory large enough to hold the
                            implementation class + the buffer pool
  \param[in]      buf_size  The size, in bytes, of the buffer pool
  \param[in]   packet_size  The size, in bytes, of one packet

  \par
   The template for using this constructor is

        int          poolSize = sizeofPool (packetCount, packetSize);
        int           fcbSize = sizeofFcb  (poolSize);
        void             *fcb = uint64_t[fcbSize];
        FixedPacket       fpa (fcb, poolSize, packetSize);
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::FixedPacket (void                   *fcb,
                          int                buf_size,
                          int             packet_size) :
   m_fcb (reinterpret_cast<FixedPacket::Fcb *>(fcb))
{
   new (m_fcb) FixedPacket::Fcb (m_fcb,
                                 buf_size, 
                                 packet_size, 
                                 0,
                                 0,
                                 0);
   return;
}
/* ---------------------------------------------------------------------- */


   

/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor to completing the initialization of the 
          implementation class

  \param[in]           fcb  Pointer to memory large enough to hold the
                            implementation class + the buffer pool
  \param[in]      buf_size  The size, in bytes, of the buffer pool
  \param[in]   packet_size  The size, in bytes, of one packet
  \param[in] packet_offset  The offset, in bytes, to the packet's control
                            block
  \param[in]  init_routine  Packet initialization routine
  \param[in]      init_ctx  Packet initialization context


  \par
   The template for using this constructor is

        int          poolSize = sizeofPool (packetCount, packetSize);
        int           fcbSize = sizeofFcb  (poolSize);
        void             *fcb = uint64_t[fcbSize];
        FixedPacket       fpa (fcb,
                               poolSize,
                               packetSize,
                               pcb_offset,
                               init_routine,
                               init_parameter);
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::FixedPacket (void                   *fcb,
                          int                buf_size,
                          int             packet_size,
                          int              pcb_offset,
                          InitCb         init_routine,
                          void        *init_parameter) :
   m_fcb (reinterpret_cast<FixedPacket::Fcb *>(fcb))
{
   new (m_fcb) FixedPacket::Fcb (m_fcb,
                                 buf_size, 
                                 packet_size, 
                                 pcb_offset,
                                 init_routine,
                                 init_parameter);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Basic constructor to fully initialize the Fixed Packet Allocator

  \param[in]           fcb  The Fixed Packet Control Block
  \param[in]        buffer  The buffer pool
  \param[in]      buf_size  The size, in bytes, of the buffer pool
  \param[in]   packet_size  The size, in bytes, of one packet
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::FixedPacket (FixedPacket::Fcb       *fcb,
                          void                *buffer,
                          int                buf_size,
                          int             packet_size) :
   m_fcb (fcb)
{
   new (fcb) FixedPacket::Fcb (buffer,
                               buf_size, 
                               packet_size);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor to fully initialize the Fixed Packet Allocator

  \param[in]           fcb  The Fixed Packet Control Block
  \param[in]        buffer  The buffer pool
  \param[in]      buf_size  The size, in bytes, of the buffer pool
  \param[in]   packet_size  The size, in bytes, of one packet
  \param[in] packet_offset  The offset, in bytes, to the packet's control
                            block
  \param[in]  init_routine  Packet initialization routine
  \param[in]      init_ctx  Packet initialization context

  \par
   This is most generalized form of the constructor, allowing complete
   control in placing the implementation class, the \a fcb, and the
   buffer pool memory, \a buffer.

  \par
   The template for using this constructor is

   #define PACKET_CNT   8
   #define PACKET_SIZE 16

   uint64_t      buffer[(PACKET_CNT, PACKET_SIZE)/sizeof (uint64_t)];

   FixedPacket::Fcb *fcb = new uint64_t[fpaSize/sizeof (uint64_t)];

   FixedPacket fpa  (fcb, 
                     buffer, 
                     sizeof (buffer),
                     PACKET_SIZE, 
                     initCb, 
                     prm);
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::FixedPacket (FixedPacket::Fcb            *fcb,
                          void                     *buffer,
                          int                     buf_size,
                          int                  packet_size,
                          int                   pcb_offset,
                          FixedPacket::InitCb init_routine,
                          void                   *init_ctx) :
   m_fcb (fcb)
{
   new (fcb) FixedPacket::Fcb (buffer,
                               buf_size, 
                               packet_size, 
                               pcb_offset,
                               init_routine,
                               init_ctx);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Fixed Packet Destructor
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::~FixedPacket ()
{
   m_fcb->~Fcb ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Rounds up to the nearest integral value of 8
  \return The rounded up value

  \param[in] n The value to round up
                                                                          */
/* ---------------------------------------------------------------------- */
static inline int rndUp (int n)
{
   return (n + sizeof (uint64_t) - 1) & ~(sizeof (uint64_t) - 1);
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
  
  \fn     int sizeof Fcb (void)
  \brief  Returns the size of the Fixed Packet Allocator control block.
  \return The size, in bytes of a Fixed Packet Allocator control block.
  
   This routine is the first step in creating a pool. The user first
   enquires about the size of the control block and then allocates it
   either from existing piece of memory or from some other allocator like
   \e malloc.
  
   This call is for modularity reasons. The user can learn the size of
   memory needed by the utility to manage a pool of packets without
   needing to know the details of how it is laid out. This could have
   also been achieved by having the initialization routine allocate
   the control block, but this takes away the freedom of the user to
   control his own allocation and deallocation
                                                                          */
/* ---------------------------------------------------------------------- */
int FixedPacket::sizeofFcb ()
{
   // Round up to 8-byte boundary
   return rndUp (sizeof (FixedPacket::Fcb));

}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
int FixedPacket::sizeofPool (int    npackets,
                             int packet_size)
{
   // Round up to the nearest 8-byte boundary
   packet_size   = rndUp (packet_size);
   int pool_size = npackets * packet_size;

   return pool_size;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \fn     FixedPacket::Pcb *allocate (FixedPacket::Fcb *fcb, void *unused)
  \brief  Basic non-interlocked version of the routine to allocate a 
          packet from the free list
  \retval NULL, on failure (empty free list)
  \retval Pointer to the allocated packet, on success

  \param fcb     Fixed Packet Control Block
  \param unused  Not used.
									  */
/* ---------------------------------------------------------------------- */
static __inline FixedPacket::Pcb *allocate (FixedPacket::Fcb *fcb,
                                            void          *unused)
{
   uint8_t *node;
   node = reinterpret_cast<decltype(node)>(fcb->m_list.List::remove ());
   if (node)
   {
      fcb->m_outstanding += 1;             /* Count the number of packets */
      node               -= fcb->m_pcboff; /* Displace to the beginning   */
   }
  
   return reinterpret_cast<FixedPacket::Pcb *>(node);
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn         void *get ()
  \brief      Get or allocate a packet from the free list.
  \return     If successful, the address of the allocated packet, else NULL
              if no packets where available.
  
   This is a non-blocking allocation. See getW() for a blocking version.
   While this call is non-blocking, the allocation is fully interlocked.
                                                                          */
/* ---------------------------------------------------------------------- */
void *FixedPacket::get ()
{
   void  *packet = m_fcb->get ();
   return packet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
   
  \fn            void *getW ()
  \brief         Get or allocate a packet from the free list with a
                 indefinite blocking

  \return        If successful, the address of the allocated packet.
                 NULL is returned on error.
  
   This is a blocking allocation, ie if the pool is exhausted, then the
   routine blocks indefinitely.
                                                                          */
/* ---------------------------------------------------------------------- */
void *FixedPacket::getW ()
{
   void  *packet = m_fcb->getW ();
   return packet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn         void *getW (struct timespace const *to)
  \brief      Get or allocate a packet from the free list using the
              timeout specified by \a to


  \param  to  The timeout
  \returns    If successful, the address of the allocated packet. NULL
              is returned on error or timeout.
  
   This is a blocking allocation, ie if the pool is exhausted, then the
   routine blocks until a packet becomes available or the timeout period
   expires.
                                                                          */
/* ---------------------------------------------------------------------- */
void *FixedPacket::getW (struct timespec const *to)
{
   void  *packet = m_fcb->getW (to);
   return packet;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
int  FixedPacket::free (void *packet)
{
   int    outstanding = m_fcb->free (packet);
   return outstanding;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Simplified constuctor for the implementation class

  \param[in]      buffer  The buffer pool
  \param[in]    buf_size  The size of the buffer pool
  \param[in] packet_size  The size of a packet
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::Fcb::Fcb (void                     *buffer,
                       int                     buf_size,
                       int                  packet_size) :
   Fcb (buffer,
        buf_size,
        packet_size,
        0,
        0,
        0)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
   
  \fn     FixedPacket::Fcb (void                     *buffer,
                            int                     buf_size,
                            int                  packet_size,
                            int                   pcb_offset,
                            FixedPacket::InitCb init_routine,
                            void             *init_parameter)
  
  \brief                Constructs the fixed size packet allocator
   
  \param         buffer A pointer to the user-supplied buffer. This buffer
                        must be 32-bit aligned. The packets will be carved
                        from this memory.
  
  \param       buf_size The size, in bytes, of the user-supplied buffer. 
 
  \param    packet_size The size, in bytes, of the packets. This number must
                        be an integral number of 32-bit words. This ensures 
                        that all packets will be properly aligned.
  
  \param     pcb_offset Offset, in bytes, to the packet control block.
                        This block is necessary for managing the packet while
                        it is on in the free pool, but is free for the user
                        to do with as he wishes when once the packet has been
                        allocated. This parameter allows the user to control
                        the placement of the control block. This offset is
                        generally specified as 0, in which case the control
                        structure is at the top of the packet, or -1,
                        in which case the links are at the bottom of the
                        packet. The latter case is especially useful if one
                        wishes to initialize the top portion of the packet
                        with a static header of some sort, while the bottom
                        of the packet contains data to be filled in later
                        when the packet is allocated.
  
  \param   init_routine The address of the entry point of a user supplied
                        routine which is called as each new packet is placed
                        on the free list. This parameter may be NULL if no
                        initialization routine is needed. It is the user's
                        responsibility not so use the control area. This is
                        possible, since the user knows the size and controls
                        its placement in the packet. Set FPA_init_cb for
                        the definition of the callback signature.
  
  \param init_parameter A user supplied parameter which is passed to
                        the callback routine.
  
  
   This routine is responsible for configuring a user-supplied piece of
   memory for use as a source of fixed sized packets. As each packet is
   placed on the free list, an optionally supplied user callback routine
   allows user initialization of the packets.
  
   Various options allow the user to place where the control structure lives
   within the packet and determine the blocking style. The most usual choice
   for the placement of the packet control structure is at the beginning
   \a pcb_offset = 0, or at the end \a pcb_offset = -1, although the user
   is allowed to specify any offset, provided it is within the packet. The
   usual blocking style is FPA_K_TYPE_FIFO_BLOCKING.

   \warning Note that while this Packet Control Block offset \a pcb_offset
   is specified in bytes it \e must \e be 4 byte aligned.   
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::Fcb::Fcb (void                     *buffer,
                       int                     buf_size,
                       int                  packet_size,
                       int                   pcb_offset,
                       FixedPacket::InitCb init_routine,
                       void             *init_parameter)
{
   unsigned int packet_cnt;
   FixedPacket::Pcb   *pcb;

   new (reinterpret_cast<void *>(&m_rw)) ResourceWait ();

   /* Round the size of a packet up to the nearest integer multiple */
   packet_size +=  (sizeof (int) - 1);
   packet_size &= ~(sizeof (int) - 1);

   /*
    |  Now check that the packet size is large enough to include a packet
    |  control block.
   */
   if (packet_size < int (sizeof (FixedPacket::Pcb))) return;


   /* If -1 is used as the packet control block offset, place it at the end */
   if (pcb_offset == FixedPacket::PlaceAtEop)
       pcb_offset = packet_size - sizeof (FixedPacket::Pcb);

   
   /* Make sure the offset to the links is within the packet */
   if (pcb_offset < 0 ||
       pcb_offset > packet_size - int (sizeof (FixedPacket::Pcb))) return;

   
   /* Initialize the FCB */
   pcb              = (FixedPacket::Pcb *)((char *)buffer + pcb_offset);
   m_outstanding    = 0;
   m_pcktsize       = packet_size;
   m_pcboff         = pcb_offset;
   m_buffer         = pcb;
   m_bufsize        = buf_size;
   packet_cnt       = 0;
   new (&m_list) List ();

   
   /* Create the packets, initializing the individual packet control blocks */
   while ( (buf_size -= packet_size) >= 0)
   {
       /*
        | Don't really need to put these nodes on in an interlocked fashion,
        | but this keeps the code clean if LI__insert does something more
        | than just link the nodes
       */
      m_list.insert (pcb);
      if (init_routine) (*init_routine)(init_parameter,
                                        (char *)pcb - pcb_offset,
                                        packet_size,
                                        pcb_offset);

       /* Advance to the next block of memory to be configured as a packet */
       pcb = (FixedPacket::Pcb *)((char *)pcb + packet_size);
   
       packet_cnt++;
   }

   
   /* Complete the control block */
   m_pcktcnt = packet_cnt;
   

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn         ~Fcb ()
  \brief      Releases any resources gathered at initialization time.

  \return     Status
  
   Releases any resources gathered at initialization time. Note that this
   does not include either releasing the user supplied memory buffer or
   the control structure itself. Both of those are property of the user and
   are the responsibility of the user to release them as appropriate.
                                                                          */
/* ---------------------------------------------------------------------- */
FixedPacket::Fcb::~Fcb ()
{
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn         void *get ()
  \brief      Get or allocate a packet from the free list.
  \param  fcb The handle of the Fixed Packet Allocator
  \returns    If successful, the address of the allocated packet, else NULL
              if no packets where available.
  
   This is a non-blocking allocation. See getW() for a blocking version.
   While this call is non-blocking, the allocation is fully interlocked.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *FixedPacket::Fcb::get ()
{
   /* Get a pointer to the RW control block */
   ResourceWait &rw = m_rw;


   /* Lock the list */
   rw.lock ();


   /* Allocate a packet from the free list */
   void *pkt = allocate (this, 0);


   /* Unlock the list */
   rw.unlock ();


   /* Return the packet */
   return pkt;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn            void *getW ()
  \brief         Get or allocate a packet from the free list with a
                 indefinite blocking

  \return        If successful, the address of the allocated packet.
                 NULL is returned on error.
  
   This is a blocking allocation, ie if the pool is exhausted, then the
   routine blocks indefinitely.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *FixedPacket::Fcb::getW ()
{
   void *pkt;


   /* Make one attempt to allocate the packet before going into wait       */
   if ((pkt = get ())) return pkt;
   else
   {
      return m_rw.getW ((ResourceWait::GetCb)allocate, 
                        reinterpret_cast<void *>(this),
                        reinterpret_cast<void *>(0));
   }
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn         void *getW (struct timespec const *to)
  \brief      Get or allocate a packet from the free list using the
              timeout specified by TOC.

  \param  to  The timeout 
  \returns    If successful, the address of the allocated packet. NULL
              is returned on error or timeout.
  
   This is a blocking allocation, ie if the pool is exhausted, then the
   routine blocks until a packet becomes available or the timeout period
   expires.
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *FixedPacket::Fcb::getW (struct timespec const *to)
{
   void *pkt;
 
   /* Make one attempt to remove the packet before going into wait       */
   if ((pkt = get ())) return pkt;

   return m_rw.getW ((ResourceWait::GetCb)allocate, 
                     reinterpret_cast<void *>(this),
                     reinterpret_cast<void *>(0),
                     to);
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!
   
  \fn            int FixedPacket::Fcb::free (void *packet)
  \brief         Returns a previously allocated packet to the free list.

  \param  packet The packet to be freed.
  \return        The number of outstanding packets
  
   This routine frees, or returns a previously allocated packet to the
   free list.
                                                                          */

/* ---------------------------------------------------------------------- */
inline int FixedPacket::Fcb::free (void *packet)
{
   ResourceWait &rw = m_rw;


   /* Address of the packet control block */
   FixedPacket::Pcb *pcb = (FixedPacket::Pcb *)((char *)packet + m_pcboff);


   /* Lock the list */
   rw.lock ();


   /* Replace the packet on the list */
   m_list.insert (pcb);


   /* Wake up anyone waiting for a packet */
   rw.wake ();


   /* Decrease the number of outstanding packets */
   int outstanding = m_outstanding -= 1;


   /* Unlock the list */
   rw.unlock ();


   /* Return the number of outstanding packets */
   return outstanding;
}
/* ---------------------------------------------------------------------- */


#if 0
#include <stdio.h>


void dump (const FixedPacket::Fcb *fcb)
{
   printf ("FCB at         = %8.8x\n"
           "list.head.flnk = %8.8x\n"
           "         .blnk = %8.8x\n"
           "      pcktsize = %8.8x\n"
           "        pcboff = %8.8x\n"
           "        buffer = %8.8x\n"
           "       bufsize = %8.8x\n",
           fcb,
           fcb->list.head.node.flnk,
           fcb->list.head.node.blnk,
           fcb->pcktsize,
           fcb->pcboff,
           fcb->buffer,
           fcb->bufsize);

   return;
}
#endif
/* ---------------------------------------------------------------------- */
} /* NAMESPACE: utl                                                       */
} /* NAMESPACE: ldmx                                                      */
/* ====================================================================== */


