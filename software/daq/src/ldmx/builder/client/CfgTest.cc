// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   CfgTest.cc
  \brief  Tests the LDMX client side file parsing
  \author JJRussell - russell@slac.stanford.edu

  \par
   This file is part of the LDMX software platform. It is subject to 
   the license terms in the LICENSE.txt file found in the top-level directory 
   of this distribution and at: 

   \verbatim
     https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
   \endverbatim

   No part of the rogue software platform, including this file, may be 
   copied, modified, propagated, or distributed except according to the terms 
   ontained in the LICENSE.txt file.

\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\
 * 
 * HISTORY
 * -------
 *
 * DATE       WHO WHAT
 * ---------- --- -------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.04.04 jjr Created
 *
\* ---------------------------------------------------------------------- */


#include "ldmx/builder/client/CfgParser.hh"
#include "ldmx/builder/client/Configuration.hh"
#include <iostream>



/* ---------------------------------------------------------------------- *//*!

  \brief Tests the LDMX Builder client side file parsing
                                                                          */
/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   CfgParser               parser;
   char *const filename = argv[1];

   {
      std::cout << "Testing: " << filename << std::endl;
      int err = parser.open (filename);

      if (err)
      {
         std::cout << "Failed to open: " << filename << std::endl;
         return -1;
      }
   }

   {
      ldmx::builder::client::Configuration cfg;
      bool err = parser.parse (&cfg);
      if (err)
      {
         std::cout << "Failed to parse: " << filename << std::endl;
         return -1;
      }
      else
      {
         cfg.print ();
      }
   }


   return 0;
}
/* ---------------------------------------------------------------------- */
