# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   ldmx/ptd/CMakeLists.txt
# \brief  Drives LDMX basic test programs
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
# EXECUTABLE: rssi_sink 
# ---------------------

  add_executable            (rssi_sink  rssi_sink.cc)
  target_rogue_compilation  (rssi_sink)
  target_link_libraries     (rssi_sink PRIVATE ${ROGUE_LIBRARIES})
  target_link_libraries     (rssi_sink PRIVATE -lrt)
  set_target_properties     (rssi_sink PROPERTIES ENABLE_EXPORTS False)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# EXECUTABLE: rssi_send 
# ---------------------

  add_executable            (rssi_send  rssi_send.cc)
  target_rogue_compilation  (rssi_send)
  target_link_libraries     (rssi_send PRIVATE -lrt)
  target_link_libraries     (rssi_send PRIVATE ${ROGUE_LIBRARIES})
  set_target_properties     (rssi_send PROPERTIES ENABLE_EXPORTS False)

# -------------------------------------------------------------------------



# -------------------------------------------------------------------------
# INSTALLATION
# ------------

  install (TARGETS  rssi_sink rssi_send
           LIBRARY  DESTINATION lib
           ARCHIVE  DESTINATION lib
           RUNTIME  DESTINATION bin)


  #[[
  # -------------------------------------
  # -- These products are not exported --
  # -------------------------------------
  include (CMakePackageConfigHelpers)
  write_basic_package_version_file (HpsEbConfigVersion.cmake
                                    VERSION ${PACKAGE_VERSION}
                                    COMPATIBILITY AnyNewerVersion)
  ]]
# -------------------------------------------------------------------------
