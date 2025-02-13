// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFGPARSER_HH__
#define __LDMX_BUILDER_CLIENT_CFGPARSER_HH__

/* ---------------------------------------------------------------------- *//*!

  \file   CfgParser.hh
  \brief  Parses a configuration file
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
 * 2019.04.04 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include <fstream>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace ldmx    { 
namespace builder {
namespace client  {

 class Configuration; 

}
}
}
/* ====================================================================== */


/* ====================================================================== */
/* DEFINITION                                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Class to parse a configuration file and fill the configuration
         class
                                                                          */
/* ---------------------------------------------------------------------- */
class CfgParser
{
public:
   CfgParser ();
   CfgParser (char const *filename);
  ~CfgParser ();
   
public:
   int  open   (const char                      *filename);
   bool parse  (ldmx::builder::client::Configuration *cfg);
   void report (int lineNum,          char const *orgLine);

public:
   char const  *m_filename;
   std::ifstream  m_stream;
};
/* ====================================================================== */


#endif
