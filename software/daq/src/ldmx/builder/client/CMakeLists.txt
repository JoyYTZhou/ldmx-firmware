# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/builder/client/CMakeLists.txt
# \brief  Drives the building of the LDMX Event Builder client software 
# \author JJRussell - russell@slac.stanford.edu
#
# \par
#  The LDMX Event Builder client receives data from multiple servers/sources
#  and coordinates individual event packets into a coherent structure.
#
#  The products consist of
#   -# An underlying library that implements the bulk of the functionality
#   -# An executable which configures the event builder. This mainly
#      consists of parsing command line options and possibly a configuration
#      file to fill in a LDMX Event Builder configuration class
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


announce ()

# -------------------------------------------------------------------------
# LIBRARY: LDMX parser
# -----------------------------

  add_library                 (ldmx-builder-parser STATIC CfgParser.cc)
  target_standard_compilation (ldmx-builder-parser)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# LIBRARY: LDMX builder-core
# -----------------------------

  add_library                (ldmx-builder-core STATIC Configuration.cc
                                                       ConnectionDsc.cc
                                                       TcpConnections.cc
                                                       Contribution.cc
                                                       Fragment.cc
                                                       Builder.cc
                                                       TriggerReceiver.cc
                                                       RssiReceiver.cc
                                                       RssiContributor.cc
                                                       RssiConnection.cc)

  target_rogue_compilation (ldmx-builder-core)
  target_link_libraries    (ldmx-builder-core PRIVATE LdmxUtl)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# LIBRARY: LDMX event builder  
# -----------------------------

  add_library              (ldmx-builder-client SHARED RogueCodaGw.cc)
                                                      
  target_compile_options   (ldmx-builder-client PRIVATE -DLDMX_EMULATION)
  set_target_soversion     (ldmx-builder-client 0 0 0)
  target_rogue_compilation (ldmx-builder-client)
  target_link_libraries    (ldmx-builder-client PRIVATE ${ROGUE_LIBRARIES})
  target_link_libraries    (ldmx-builder-client PRIVATE ldmx-builder-parser)
  target_link_libraries    (ldmx-builder-client PRIVATE -Wl,--whole-archive
                                                         ldmx-builder-core
                                                         -Wl,--no-whole-archive)


  # -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# LIBRARY: LDMX Rouge event builder  
# ---------------------------------

  ##add_library              (ldmx-builder-client-real SHARED RogueCodaGw-Real.cc
  ##                                                          RogueCoda.cc)

  ##set_target_soversion     (ldmx-builder-client-real 0 0 0)
  ##target_rogue_compilation (ldmx-builder-client-real)
  ##target_link_libraries    (ldmx-builder-client-real PRIVATE ${ROGUE_LIBRARIES})
  ##target_link_libraries    (ldmx-builder-client-real PRIVATE ldmx-builder-parser)
  ##target_link_libraries    (ldmx-builder-client-real PRIVATE ldmx-builder-core)

# -------------------------------------------------------------------------





# -------------------------------------------------------------------------
# EXECUTABLE: LDMX event builder 
# -----------------------------

  add_executable           (LdmxBuilder  LdmxBuilder.cc
                                         Parameters.cc)
  target_rogue_compilation (LdmxBuilder)
  target_link_libraries    (LdmxBuilder PRIVATE ldmx-builder-client)
  target_link_libraries    (LdmxBuilder PRIVATE -lrt)
  target_link_libraries    (LdmxBuilder PRIVATE LdmxUtl)
  set_target_properties    (LdmxBuilder PROPERTIES   INSTALL_RPATH "$ORIGIN/../lib")

# -------------------------------------------------------------------------




# -------------------------------------------------------------------------
# EXECUTABLE: Rouge event builder 
# -------------------------------

#  add_executable           (RogueBuilder  RogueBuilder.cc
#                                            Parameters.cc)

#  target_rogue_compilation (RogueBuilder)
#  target_link_libraries    (RogueBuilder PRIVATE ldmx-builder-client)
#  target_link_libraries    (RogueBuilder PRIVATE -lrt)
#  set_target_properties    (RogueBuilder PROPERTIES INSTALL_RPATH "$ORIGIN/../lib")
#  target_link_libraries    (RogueBuilder PRIVATE LdmxUtl)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# EXECUTABLE: Test configuration file parsing
# -------------------------------------------

  add_executable              (CfgTest   CfgTest.cc
                                         Configuration.cc)
  target_standard_compilation (CfgTest)
  target_link_libraries       (CfgTest PRIVATE ldmx-builder-parser)
# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# INSTALLATION
# ------------

install (TARGETS
           LdmxBuilder
#          RogueBuilder
           ldmx-builder-client
#          ldmx-builder-client-real
           EXPORT   LdmxBuilderTargets
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)

  include (CMakePackageConfigHelpers)
  write_basic_package_version_file (ConfigVersion.cmake
                                    VERSION       ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
# -------------------------------------------------------------------------
