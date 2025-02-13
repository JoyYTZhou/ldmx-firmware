# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/builder/server/CMakeLists.txt
# \brief  Drives the building of the LDMX Event Builder server software 
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  The LDMX Builder server is a multiple data source emulator meant
#  to test the LDMX Event Builder, basically being a surrogate for the
#  RCE data sources.
#
#  The products consist of
#   -# An executable which produces 'N' data sources.
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
# 2021.04.21  jjr  Adapted from HPS version
#
# -------------------------------------------------------------------------


announce ()


# -------------------------------------------------------------------------
# LIBRARY: LDMX event builder 
# -----------------------------

  add_library              (ldmx-builder-server SHARED Configuration.cc
                                                       RssiContributor.cc
                                                       RssiConnection.cc
                                                       RssiServers.cc
                                                       TriggerServer.cc)

  set_target_soversion     (ldmx-builder-server 0 0 0)
  target_rogue_compilation (ldmx-builder-server)
  target_link_libraries    (ldmx-builder-server PRIVATE ${ROGUE_LIBRARIES})

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# EXECUTABLE: LDMX event builder server emulator
# ----------------------------------------------
  add_executable           (LdmxBuilderServer  LdmxBuilderServer.cc
                                               Parameters.cc)

  target_rogue_compilation (LdmxBuilderServer)
  target_link_libraries    (LdmxBuilderServer PRIVATE  ldmx-builder-server
                                                       ldmx-data-server)
  target_link_libraries    (LdmxBuilderServer PRIVATE ${ROGUE_LIBRARIES})
  target_link_libraries    (LdmxBuilderServer PRIVATE  LdmxUtl)
  target_link_libraries    (LdmxBuilderServer PRIVATE -lrt)
  set_target_properties    (LdmxBuilderServer PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  LdmxBuilderServer ldmx-builder-server
           EXPORT   LdmxBuilderServerTargets
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)


  include  (CMakePackageConfigHelpers)
  write_basic_package_version_file (ConfigVersion.cmake
                                    VERSION       ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
# -------------------------------------------------------------------------
