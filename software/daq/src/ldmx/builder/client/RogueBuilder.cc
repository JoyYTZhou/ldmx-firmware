// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!

  \file   ldmx/builder/client/RogueBuilder.cc
  \brief  LDMX event/fragment builder
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
 * ---------- --- -------------------------------------------------------
 * 2021.04.21 jjr Adapted from HPS version
 * 2019.03.30 jjr Created
 *
\* ---------------------------------------------------------------------- */



#include "Parameters.hh"

#include "ldmx/builder/client/RogueCodaGw.hh"
#include "ldmx/builder/client/Configuration.hh"
#include "ldmx/utl/Thread.hh"


#include <cinttypes>
#include <cstdio>
#include <iostream>


static void fragmentHandler (RogueCodaGw *gw);


/* ---------------------------------------------------------------------- *//*!

  \brief main program for the LDMX Event/Fragment Builder

\* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   /// Parameters prms (argc, argv);
   /// ldmx::builder::client::Configuration cfg (static_cast<int>(prms.m_type),
   ///                                                      prms.m_nfragments);
   ///prms.configure (&cfg);

   RogueCodaGw gw;
   bool   success;


   // Configure the passive elements
   /// success = gw.download (cfg);

   std::cout << "Downloading configuration " << argv[1] << std::endl;
   success = gw.download (argv[1], "RogueEmulator");
   if (!success)
   {
      std::cerr << "RogueBuilder::download failed" << std::endl;
      return -1;
   }


   // ---------------------------------------------------------------
   // Start the thread to handle the events
   // This is purely a emulation function.  
   // In the actual Coda process, Coda implements this functionality
   // ---------------------------------------------------------------
   ldmx::utl::Thread handlerThread;
   handlerThread.launch (fragmentHandler, &gw);


   // Prepare the data sources and sinks for acquisition
   success = gw.prestart ();
   if (!success)
   {
      std::cerr << "RogueBuilder::prestart failed" << std::endl;
      return -1;
   }


   // Start the dataflow
   success = gw.go ();
   if (!success)
   {
      std::cerr << "RogueBuilder::go failed" << std::endl;
      return -1;
   }

   while (1);

   return 0;

}
/* ---------------------------------------------------------------------- */




static void fragmentHandler (RogueCodaGw *gw)
{
   static uint32_t Index;
   // Wait for incoming event
   while (1)
   {
      uint32_t buffer[1000];
      gw->event (buffer, Index++);
   }

   return;
}









