// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFGOBJECT_HH__
#define __LDMX_BUILDER_CLIENT_CFGOBJECT_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgObject.hh
  \brief  Base class for a parsed configuration object
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
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.04.11 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include <string>
#include <iostream>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

class CfgLine;
class CfgParameters;

/* ====================================================================== */




/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Base class for all configuration parsed objects
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgObject
{
public:
   enum class Type
   {
      Unknown       = 0,
      Collection    = 1,
      Contributor   = 2,
      Configuration = 3,
   };


public:
   CfgObject (Type              type, 
              char    const    *name,
              CfgLine const *cfgLine);

   virtual ~CfgObject ();

public:
   void         print (int instance) const;

   virtual bool set   (char const           *fieldName, 
                       CfgParameters const &parameters, 
                       char                     **rest) = 0;

   virtual void print (std::string const &name, 
                       int            instance) const = 0;
      
public:
   Type           m_type;  /*!< The object type                           */
   std::string    m_name;  /*!< The name of the object                    */
   CfgLine const *m_line;  /*!< Line and file where the object was defined*/
};
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief  Basic constructor of all configuration objects

  \param[in]    type  The object type
  \param[in]    name  The number of the object
  \param[in] cfgLine  The line and file where the object was defined,
                      This is used for error reporting
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgObject::CfgObject (Type              type, 
                             char    const    *name,
                             CfgLine const *cfgLine) :
   m_type (type),
   m_name (name),
   m_line (cfgLine)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Destructor for the base class of all configuration objects
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgObject::~CfgObject ()
{
   /// std::cout << "Cfgobject::destructor: " << m_name << std::endl;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print driver for the object
                                                                          */
/* ---------------------------------------------------------------------- */
inline void CfgObject::print (int instance) const
{
   print (m_name, instance);
}
/* ====================================================================== */

#endif
