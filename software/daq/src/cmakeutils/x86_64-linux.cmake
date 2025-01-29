# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   x86_64-linux.txt
# \brief  Build options for generic x86_64 linux products
# \author JJRussell - russell@slac.stanford.edu
#
#
# \par 
#  Sets up compiler and linker options for generic x86_64 linux products
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
# 2021.04.08  jjr  Suppressed warning for unused functions
#                  Made optimization level dependent on build type
#
# -------------------------------------------------------------------------



# Prefix detection only works with compiler id "GNU"
# CMake will look for prefixed g++, cpp, ld, etc. automatically

#enable_language(CXX)

#message ("------------------------------------------------------")
#message ("!!! KLUDGE in x86_64-linux.cmake, setting c++ to c++0x")
#message ("------------------------------------------------------")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wno-deprecated -D_GLIBCXX_USE_CXX11_ABI=0")

# --------------------------------------------------------------
function (target_standard_compilation target)

  # Set optimization level according to build type
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")

    # Debug optimization level = -O0
    set (opt "-O0")
    
  else ()

    # All others optimization level = -O2
    set (opt "-O2")
    
  endif ()

  #message ("opt = ${opt}")
  
  target_compile_options  (${target} PRIVATE -fPIC   -DNO_PYTHON -Wall ${opt} -Wno-unused-function -gdwarf-3)
  set_target_properties   (${target} PROPERTIES CXX_EXTENSIONS OFF)


endfunction ()
# --------------------------------------------------------------



# --------------------------------------------------------------
function (target_rogue_compilation target)

  target_standard_compilation (${target})
  target_compile_options      (${target} PRIVATE -Wno-deprecated)
  target_include_directories  (${target} SYSTEM PRIVATE ${ROGUE_INCLUDE_ONLY})
  ###target_exclude_python       (${target})
  target_link_libraries       (${target} PUBLIC ${ROGUE_LIBRARIES_ONLY})

endfunction ()
# --------------------------------------------------------------



#add_compile_options (-Wno-deprecated -Wall)

#SET(target x86_64-linux)
