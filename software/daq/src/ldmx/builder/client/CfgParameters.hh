// -*-Mode: C++;-*-

#ifndef __LDMX_BUILDER_CLIENT_CFGPARAMETERS_HH__
#define __LDMX_BUILDER_CLIENT_CFGPARAMETERS_HH__


/* ---------------------------------------------------------------------- *//*!

  \file   CfgParameters.hh
  \brief  The parsed parameters from a configuration file
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


#include <map>
#include <string>


/* ---------------------------------------------------------------------- */
class CfgParameters : public std::map<std::string, std::string>
{
public:
   CfgParameters ()
      { return; }

   void print () const;
};
/* ---------------------------------------------------------------------- */

#endif
