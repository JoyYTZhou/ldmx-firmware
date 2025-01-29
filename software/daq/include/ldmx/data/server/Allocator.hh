// -*-Mode: C++;-*-

#ifndef __LDMX_DATA_SERVER_ALLOCATOR_HH__
#define __LDMX_DATA_SERVER_ALLOCATOR_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/data/server/Allocator.hh
  \brief  Manages a simple array allocation, no interlocks, nothing fancy
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
 * 2019.02.21 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <new>
#include <cinttypes>


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace data    {
namespace server  {
/* ---------------------------------------------------------------------- *//*!

  \brief Manages allocating memory from an array of bytes in a very
         simple method;
                                                                          */
/* ---------------------------------------------------------------------- */
class Allocator
{
public:
   Allocator (void *buffer, int bytes);
  ~Allocator ();
   
public:

   void *operator new (size_t nbytes, Allocator *allocator);
   void     *allocate (int nbytes);

   void                  shrink (int nbytes);
   void  operator delete (void *ptr);
   void  operator delete (void *ptr, size_t nbytes);

   // Return a pointer to the buffer
   void           *buffer ();
   void     const *buffer () const;


   // Return a pointer to the current memory to be written
   void           *next  () const;

   // Return the number of bytes left in the buffer
   uint32_t        left  () const;

   // Return the number of bytes currently being used
   uint32_t        used  () const;

   // Reset the allocator to empty
   void            reset ();

private:
   uint8_t  *m_buffer;   /*!< The buffer being managed                    */
   uint32_t     m_cur;   /*!< Index of the next available location        */
   uint32_t     m_max;   /*!< Maximum number of bytes in the buffer       */
};
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace ldmx    {
namespace data    {
namespace server  {
/* ---------------------------------------------------------------------- *//*!

  \brief Buffer allocator constructor

  \param[in] buffer  The buffer to manage
  \param[in] nbytes  The number of bytes in the buffer
                                                                          */
/* ---------------------------------------------------------------------- */
inline Allocator::Allocator (void *buffer, int nbytes) :
   m_buffer (reinterpret_cast<decltype(m_buffer)>(buffer)),
   m_cur    (     0),
   m_max    (nbytes)
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
inline Allocator::~Allocator ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the buffer memory
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *Allocator::buffer ()
{
   return m_buffer;
}
/* ---------------------------------------------------------------------- */






/* ---------------------------------------------------------------------- *//*!

  \brief Return a const pointer to the buffer memory
                                                                          */
/* ---------------------------------------------------------------------- */
inline void const *Allocator::buffer () const
{
   return m_buffer;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates nwrds
  \return A pointer to the allocated memory

  \param[in] nwrds  The number of bytes to allocate.  
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *Allocator::allocate (int nbytes)
{
   /// printf ("Allocates %d bytes\n", nbytes);
   uint32_t cur =  m_cur;
   m_cur       += nbytes;
   return m_buffer + cur;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Allocates nbytes
  \return A pointer to the allocated memory

  \param[in] nbytes The number bytes to allocate
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *Allocator::operator new (size_t nbytes, Allocator *allocator)
{
   void         *ptr = allocator->allocate (nbytes);
   return ptr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return \a nwrds to buffer

  \param[in]  nwrds  The number of words to return
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Allocator::shrink (int nwrds)
{
   m_cur -= nwrds;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets the allocator to empty, \e i.e. no memory allocated
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Allocator::reset ()
{
   m_cur = 0;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the next memory location to be written
  \return A pointer to the next memory location to be written
                                                                          */
/* ---------------------------------------------------------------------- */
inline void *Allocator::next  () const
{
   return m_buffer + m_cur;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of bytes currently being used
  \return The number of bytes currently being used
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t Allocator::used  () const
{
   return m_cur;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of bytes still available
  \return The number of bytes still available
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t Allocator::left () const
{
   return m_max - m_cur;
}
/* ---------------------------------------------------------------------- */




#if 0
/* ---------------------------------------------------------------------- *//*!

  \brief Custom delete operator, deallocates all memory assigned via
         previous new

  \param[in] ptr  Pointer to the memory to be dellocated
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Allocator::operator delete (void *ptr)
{
   m_cur = static_cast<decltype<m_buffer>(ptr)> - m_buffer;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Shrinks the buffer by the specificed number of words


  \param[in] nwrds The number of words to shrink the buffer by
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Allocator::operator delete (void *ptr)
{
   m_cur -= nwrds;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief This effective shrinks the allocated memory

  \param[in] ptr     Unused
  \param[in] nbytes  The number of bytes to return
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Allocator::delete (void *ptr, size_t nbytes)
{
   m_cur -= nbytes * sizeof (*m_buffer);
   return;
}
/* ---------------------------------------------------------------------- */
#endif
/* ---------------------------------------------------------------------- */
} /* namespace server                                                     */
} /* namespace data                                                       */
} /* namespace ldmx                                                       */
/* ====================================================================== */


#endif
