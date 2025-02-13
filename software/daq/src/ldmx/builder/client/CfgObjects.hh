// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFGOBJECTS_HH__
#define __LDMX_BUILDER_CLIENT_CFGOBJECTS_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgObjects.hh
  \brief  The aggregate of all parsed configuration object
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
#include <map>
#include <memory>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

class CfgObject;

/* ====================================================================== */





/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief  The catalog of all parsed objects
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgObjects : public std::map<std::string, std::unique_ptr<CfgObject>>
{
public:
   CfgObjects ();

public:
   void print () const;
};
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Default constructor
                                                                          */
/* ---------------------------------------------------------------------- */
inline CfgObjects::CfgObjects ()
{ 
   return; 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints all the objects
                                                                          */
/* ---------------------------------------------------------------------- */
inline void CfgObjects::print () const
{
   int instance = 0;

   auto  it = this->cbegin ();
   auto end = this->cend   ();
   while (it != end)
   {
      CfgObject const *object = it->second.get ();

      object->print (instance);

      
      it++;
      instance++;
   }

   return;
}
/* ====================================================================== */


#endif
