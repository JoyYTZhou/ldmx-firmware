# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   CMakeLists.txt
# \brief  Builds the eudaq for the LDMX project
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
# 2021.04.23  jjr  Cloned for example/CMakeLists.txt
#
# -------------------------------------------------------------------------



announce ()

set (EUDAQ_MODULE eudaq_module_ldmx_emulator)
set (LDMX_BUILDER_CLIENT ${CMAKE_INSTALL_PREFIX}/lib/libldmx-builder-client.so)
set (LDMX_UTL            ${CMAKE_INSTALL_PREFIX}/lib/libLdmxUtl.so)

message  ("CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_PREFIX}/lib")
message ("EUDAQ_MODULE       = ${EUDAQ_MODULE}")
message ("EUDAQ_CORE_LIBRARY = ${EUDAQ_CORE_LIBRARY}")
message  ("LDMX_BUILD_CLIENT  = ${LDMX_BUILD_CLIENT}")


# -------------------------------------------------------------------------
# LIBRARY: LdmxDaq
# ----------------

  add_library (${EUDAQ_MODULE} SHARED    LdmxRunControl.cc
                                           LdmxProducer.cc
                                    LdmxTgDataCollector.cc
                                            LdmxMonitor.cc)

  target_rogue_compilation (${EUDAQ_MODULE})
#  target_link_libraries       (${EUDAQ_MODULE} PRIVATE -lrt)
#  target_link_libraries       (${EUDAQ_MODULE} PRIVATE ${LDMX_UTL})
#  set_target_properties       (${EUDAQ_MODULE} PROPERTIES   INSTALL_RPATH "$ORIGIN/../lib")


  target_link_libraries       (${EUDAQ_MODULE} PRIVATE
                               -lrt
                               ${LDMX_BUILDER_CLIENT}
                               ${LDMX_UTL}
                               ${EUDAQ_CORE_LIBRARY}
                               ${EUDAQ_TTREE_LIBRARY}
                               ${EUDAQ_ROOT_MONITOR_LIBRARY})
                                       

# -------------------------------------------------------------------------
                          


install(TARGETS
  ${EUDAQ_MODULE}
  RUNTIME DESTINATION bin/eudaq/emulator
  LIBRARY DESTINATION lib/eudaq/emulator
  ARCHIVE DESTINATION lib/eudaq/emulator)
