// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFGCOLLECTION_HH__
#define __LDMX_BUILDER_CLIENT_CFGCOLLECTION_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgColeection.cc
  \brief  The parsed collection from a configuration file
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




/* ---------------------------------------------------------------------- *//*!

  \brief A configuation file collection of objects
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgCollection :  public CfgObject
{
public:
   CfgCollection (char const *name, CfgLine *cfgLine) :
      CfgObject (CfgObject::Type::Collection, name, cfgLine)
   {
      return;
   }

   ~CfgCollection ()
      {
         /// std::cout << "Destructor::collection" << std::endl;
         return;
      }

   bool set (char const               **rest, 
             CfgParameters const &parameters,
             CfgLine const             *line)
      {
         return true;
      }

   bool         parse (char **rest,
                       CfgParameters const &parameters);

   virtual bool set   (char const           *fieldName, 
                       CfgParameters const &parameters,
                       char                     **rest);

   void print (std::string const &name, int instance) const;

public:
   std::vector<std::string> m_members;

};
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- */
inline bool CfgCollection::parse (char **rest, CfgParameters const &m_parameters)
{
   static char const *delimiters = " ,";

   /// std::cout << "Define Collection tokens " << *rest << std::endl;

   while (1)
   {
      char const *token = strtok_r (*rest, delimiters, rest);
      if (token == nullptr) break;

      m_members.push_back (token);
      /// std::cout << "Collection <" << token << '>' << std::endl;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
bool CfgCollection::set (char const            *fieldName, 
                          CfgParameters const &parameters,
                         char                      **rest)
{
   // Cannot modify a collection
   return true;
}

void CfgCollection::print (std::string const &name, int instance) const
{
   auto  it = m_members.cbegin ();
   auto end = m_members.cend   ();

   std::cout << "Collection   ["  << std::hex   << instance   << "] "
             << std::setw ( 8) << std::right << name      << ' ';

   while (it != end)
   {
      std::cout << std::setw (8) << std::right << *it;
      it++;
   }

   std::cout << std::endl;
}
/* ---------------------------------------------------------------------- */

#endif
