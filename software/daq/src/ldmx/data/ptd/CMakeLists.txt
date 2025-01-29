# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/data/server/CMakeLists.txt
# \brief  Drives the building of the LDMX data server methods
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  This package test LDMX data server server and client side.
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
#  No part of the LDMX software platform, including this file, may be 
#  copied, modified, propagated, or distributed except according to the terms 
#  ontained in the LICENSE.txt file.
# 
# -------------------------------------------------------------------------


# -------------------------------------------------------------------------
#
# DATE        WHO  WHAT
# ----------  ---  --------------------------------------------------------
# 2021.04.20  jjr  Created to drive the building of the batch test routine
# -------------------------------------------------------------------------



announce ()


# -------------------------------------------------------------------------
# EXECUTABLE: LDMX data server emulator
# -------------------------------------
  add_executable              (LdmxBatchTest         LdmxBatchTest.cc)
  target_standard_compilation (LdmxBatchTest)
  target_link_libraries       (LdmxBatchTest PRIVATE ldmx-data-server
                                                     ldmx-data-client)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  LdmxBatchTest
           EXPORT   LdmxDataPtdTargets
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)


  include  (CMakePackageConfigHelpers)
  write_basic_package_version_file (ConfigVersion.cmake
                                    VERSION       ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
# -------------------------------------------------------------------------
