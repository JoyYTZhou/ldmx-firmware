# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   CMakeLists.txt
# \brief  Builds the various eudaq implementations for the LDMX project
# \author JJRussell - russell@slac.stanford.edu
#
#
# \par 
#  This incorporates generic LDMX methods and utilities into the EUDAQ
#  framework
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



# -------------------------------------------------------------------------
#
# DATE        WHO  WHAT
# ----------  ---  --------------------------------------------------------
# 2021.04.23  jjr  Created
#
# -------------------------------------------------------------------------



announce ()

add_subdirectory (example)
add_subdirectory (emulator)
