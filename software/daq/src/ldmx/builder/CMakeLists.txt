# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/builder/CMakeLists.txt
# \brief  Drives the building of the LDMX Event Builder client and an 
#         emulation of a server to be used for testing purposes.
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  In addition to driving the build of the client and server products
#  this also builds a library that is common to and shared by both.
#
# \par
#  This file is part of the LDMX software platform. It is subject to 
#  the license terms in the LICENSE.txt file found in the top-level directory 
#  of this distribution and at: 
#
# \verbatim
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
#  \endverbatim
#
#  No part of the rogue software platform, including this file, may be 
#  copied, modified, propagated, or distributed except according to the terms 
#  ontained in the LICENSE.txt file.
# 
# -------------------------------------------------------------------------


announce ()

# -------------------------------------------------------------------------
# LIBRARY: LDMX event builder 
# ---------------------------

  add_library              (ldmx_eb STATIC Logging.cc)
  target_rogue_compilation (ldmx_eb)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# SUBBUILDS: client, server
# ------------------------ 

  add_subdirectory(client)
  add_subdirectory(server)

# -------------------------------------------------------------------------


