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
# 2021.04.20  jjr  Moved to the EUDAQ subdirectory, were the actual code
#                  resides
# 2021.03.19  jjr  Only install if DO_INSTALL is defined
#                  Suppress up-to-date installation messages
# 2019.05.10  jjr  Added support for the standard CMake build types
#
# -------------------------------------------------------------------------



announce ()

set (EUDAQ_MODULE eudaq_module_ldmx_example)


#message ("EUDAQ_MODULE       = ${EUDAQ_MODULE}")
#message ("EUDAQ_CORE_LIBRARY = ${EUDAQ_CORE_LIBRARY}")


# -------------------------------------------------------------------------
# LIBRARY: LdmxDaq
# ----------------

  add_library (${EUDAQ_MODULE} SHARED    LdmxRunControl.cc
                                           LdmxProducer.cc
                                    LdmxTgDataCollector.cc
                                            LdmxMonitor.cc)

  target_standard_compilation (${EUDAQ_MODULE})
  target_link_libraries       (${EUDAQ_MODULE}
                               ${EUDAQ_CORE_LIBRARY}
                               ${EUDAQ_TTREE_LIBRARY}
                               ${EUDAQ_ROOT_MONITOR_LIBRARY})
                                       

# -------------------------------------------------------------------------
                          


install(TARGETS
  ${EUDAQ_MODULE}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib/eudaq/example
  ARCHIVE DESTINATION lib/eudaq/example)
