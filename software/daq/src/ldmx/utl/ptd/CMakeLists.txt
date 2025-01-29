# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/utl/ptd/CMakeLists.txt
# \brief  Drives HPS utility basic test programs
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  These server two purposes
#    -# Typical test programs to verify functionality
#    -# Templates or examples of usage
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
# EXECUTABLE: test_list 
# ---------------------

  add_executable               (test_list  test_list.cc)
  target_standard_compilation  (test_list)
  target_link_libraries        (test_list  LINK_PRIVATE LdmxUtl -lrt)

# -------------------------------------------------------------------------




# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  test_list
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)


  #[[
  # -------------------------------------
  # -- These products are not exported --
  # -------------------------------------
  include (CMakePackageConfigHelpers)
  write_basic_package_version_file (LdmxEbConfigVersion.cmake
                                    VERSION ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
  ]]
# -------------------------------------------------------------------------
