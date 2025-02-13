// -*-Mode: C++;-*-


#ifndef __LDMX_BUILDER_CLIENT_CFGCONFIGURATION_HH__
#define __LDMX_BUILDER_CLIENT_CFGCONFIGURATION_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgConfiguration.hh
  \brief  The parsed configuration from a configuration file
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

  \brief Describes the output parameters
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgConfiguration : public CfgObject
{
public:
   CfgConfiguration (char const *name, CfgLine *cfgLine) :
      CfgObject (CfgObject::Type::Configuration, name, cfgLine)
   {
      return;
   }

   ~CfgConfiguration ()
      {
         /// std::cout << "Destructor::configuration" << std::endl;
         return;
      }

   bool parse            (char **rest,
                          CfgParameters const &parameters);
   bool set              (char const           *fieldName,
                          CfgParameters const &parameters,
                          char                     **rest);

   void print (std::string const &name, int instance) const;

public:
   std::vector<std::string>     m_contributors;
   std::string                   m_loggingLevel;
   std::string                     m_nfragments;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
bool CfgConfiguration::parse (char                     **rest,
                              CfgParameters const &parameters)
{
   bool err;

   while (1)
   {
      char const *fieldName = getToken (rest);
      if (fieldName == nullptr) break;

      /// std::cout << "CfgConfiguration::parse fieldName <" 
      ///           << fieldName  << '>' << std::endl;


      err = set (fieldName, parameters, rest);
      if (err) return true;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
bool CfgConfiguration::set (char const           *fieldName,
                            CfgParameters const &parameters,
                            char                     **rest)
{
   ///bool err;

   /// std::cout <<  "Configuration fieldname <" << fieldName << '>' << std::endl;

   if (compare (fieldName, "CONTRIBUTORS"))
   {
      while (1)
      {
         bool last;
         char const *token = getValueComma (rest, &last);
         if (token == nullptr) break;
         m_contributors.push_back (std::move (std::string (token)));
         if (last) break;
      }
      return false;
   }

   else if (compare (fieldName, "LOGGINGLEVEL"))
   {
      // Get the IP/PIPE name
      char const *token = getValue (rest, parameters);
      m_loggingLevel    = token;
      return false;
   }

   else if (compare (fieldName, "NFRAGMENTS"))
   {
      char const *token = getValue (rest, parameters);
      m_nfragments = token;
      return false;
   }

   else
   {
      // Not found
      return true;
   }

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void CfgConfiguration::print (std::string const &name, int instance) const
{
   std::cout << "Configuration[" << std::hex << instance << "] " 
             <<  std::setw (8) << std::right << name << ' ';

   auto  it = m_contributors.cbegin ();
   auto end = m_contributors.cend   ();
   while (it != end)
   {
      std::string const &ctb = *it;
      std::cout <<  std::setw (8) << std::right << ctb;
      it++;
   }

   std::cout << std::setw(8) << std::right << m_loggingLevel
             << std::setw(8) << std::right << m_nfragments 
             << std::endl;

   return;
}
/* ---------------------------------------------------------------------- */

#endif
