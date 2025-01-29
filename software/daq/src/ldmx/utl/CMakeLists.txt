# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/ptd/CMakeLists.txt
# \brief  Drives LDMX basic utility program builds
# \author JJRussell - russell@slac.stanford.edu
#
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


# ----------------------------------------------------------------------
#
#  HISTORY
#  -------
#
# DATE       WHO WHAT
# ---------- --- -------------------------------------------------------
# 2021.04.20 jjr Adapted from HPS version
# 2019.01.18 jjr Cloned for EXO version
# ----------------------------------------------------------------------


announce ()


# -------------------------------------------------------------------------
# LIBRARY: LdmxUtl
# ----------------

  add_library                 (LdmxUtl SHARED ResourceWait.cc
                                              ListInterlocked.cc
                                              ListsInterlocked.cc
                                              FixedPacket.cc
                                              Message.cc
                                              Dumper.cc)

  target_standard_compilation (LdmxUtl)
  target_link_libraries       (LdmxUtl -lpthread)
  set_target_soversion        (LdmxUtl 0 0 0)


# -------------------------------------------------------------------------




# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  LdmxUtl
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)


  #[[
  # -------------------------------------
  # -- These products are not exported --
  # -------------------------------------
  include (CMakePackageConfigHelpers)
  write_basic_package_version_file (ConfigVersion.cmake
                                    VERSION ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
  ]]
# -------------------------------------------------------------------------


add_subdirectory (ptd)
