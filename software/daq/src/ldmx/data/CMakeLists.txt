# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/data/CMakeLists.txt
# \brief  Drives the building of the client and server side data formatters.
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  This file is part of the HPS software platform. It is subject to 
#  the license terms in the LICENSE.txt file found in the top-level directory 
#  of this distribution and at: 
#
# \verbatim
#    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
#  \endverbatim
#
#  No part of the LDMX software platform, including this file, may be 
#  copied, modified, propagated, or distributed except according to the terms 
#  ontained in the LICENSE.txt file.
# 
# -------------------------------------------------------------------------


# -------------------------------------------------------------------------
#
# DATE        WHO  WHAT
# ----------  ---  --------------------------------------------------------
# 2021.04.20  jjr  Created to drive the building of the ldmx/data
#                  subdirectories
# -------------------------------------------------------------------------




announce ()


# -------------------------------------------------------------------------
# SUBBUILDS: client, server, ptd
# ------------------------------

  add_subdirectory(client)
  add_subdirectory(server)
  add_subdirectory(   ptd)

# -------------------------------------------------------------------------

