# -*-Mode: CMake;-*-

# -------------------------------------------------------------------------
#
# \file   CMakeLists.txt
# \brief  Drives the building of the LDMX project
# \author JJRussell - russell@slac.stanford.edu
#
#
# \par 
#  Sets up the common features and capabilities used by all the sub-packages
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
# 2021.03.19  jjr  Only install if DO_INSTALL is defined
#                  Suppress up-to-date installation messages
# 2019.05.10  jjr  Added support for the standard CMake build types
#
# -------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.3)


# ======================================================================
# BEGIN: FUNCTION/MACRO DEFINITIONS
# ----------------------------------------------------------------------

# --------------------------------------------------------
# \brief Announce package/subdirectory being built
# --------------------------------------------------------
function (announce)

  message ("Entering: ${CMAKE_CURRENT_SOURCE_DIR}")

endfunction ()
# --------------------------------------------------------


# -----------------------------------------------------------
# \brief Find the rogue package and configure this build
#        to use it.
#
#  In addition, this method removes a number of compiler
#  flagss that rogue adds that interfer with the build types,
#  namely -O2, -g, -fPIC.
# -----------------------------------------------------------
macro (setup_rogue)

   if (DEFINED ENV{ROGUE_DIR})
      set(Rogue_DIR $ENV{ROGUE_DIR}/lib)
   else()
      set(Rogue_DIR ${CMAKE_PREFIX_PATH}/lib)
   endif()

   find_package(Rogue QUIET REQUIRED)


   # ------------------------------------------------
   # Remove any inherited -O2 flag from the CXX flags
   # ------------------------------------------------
   if(NOT "${CMAKE_CXX_FLAGS}" STREQUAL "")
      string(REPLACE -O2 "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
   endif ()


   # ------------------------------------------------
   # Remove any inherited -g flag from the CXX flags
   # ------------------------------------------------
   if(NOT "${CMAKE_CXX_FLAGS}" STREQUAL "")
      string(REPLACE -g  "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
   endif ()


   # ------------------------------------------------
   # Remove any inherited -fPIC flag from the CXX flags
   # ------------------------------------------------
   if(NOT "${CMAKE_CXX_FLAGS}" STREQUAL "")
      string(REPLACE -fPIC  "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
   endif ()


   # --- message ("After removal CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")


endmacro ()
# --------------------------------------------------------


# --------------------------------------------------------
# \brief Diagnostic print of the rogue configuration 
#        (directory, includes libraries)
# --------------------------------------------------------
function (print_rogue)

   message ("ROGUE_DIR         : ${ROGUE_DIR}")
   message ("ROGUE_INCLUDE_DIRS: ${ROGUE_INCLUDE_DIRS}")
   message ("ROGUE_LIBRARIES   : ${ROGUE_LIBRARIES}")

endfunction ()
# --------------------------------------------------------



# --------------------------------------------------------
# \brief Setup the CONDA enviroment if defined
# --------------------------------------------------------
macro (setup_conda)

   if (DEFINED ENV{CONDA_PREFIX})
       set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}")
       link_directories($ENV{CONDA_PREFIX}/lib)
   endif ()

endmacro ()
# --------------------------------------------------------



# --------------------------------------------------------
# \brief Diagnostic print of the conda configuration 
# --------------------------------------------------------
function (print_conda)

   if (DEFINED ENV{CONDA_PREFIX})
       message ("CONDA/lib     : $ENV{CONDA_PREFIX}/lib")
   endif ()


endfunction ()
# --------------------------------------------------------


# --------------------------------------------------------
function (print_tools)

   message (STATUS
            "C++:  ${CMAKE_CXX_COMPILER}")
   message (STATUS
            "C  :  ${CMAKE_C_COMPILER}")

#            "CPP:  ${CPP}"
#            "LD :  ${LD}"
#            "ASM"  ${ASM}")

endfunction ()
# --------------------------------------------------------


# --------------------------------------------------------
#
# \brief Extract the target platorm from the build
#         directory and verify it is one of the known
#         target platforms
#
# \param[out] platform Returned as the target platform
#                      \i e.g. x86_64-linux, etc
#
# \note
#  The placeholder parameter is just that, a placeholder
#  with no real impact.  The actual value is returned in
#  platform.  Welcome to the bizarre world of CMake macros
#  and functions
#  
# --------------------------------------------------------
macro (get_platform placeholder)

   # ----------------------------------------------------
   # Check that a legitimate platform has been specified
   # ----------------------------------------------------
   set (LEGITIMATE_PLATFORMS x86_64-linux arm_CA9-linux)

   # Remove the build type
   get_filename_component(platform ${CMAKE_BINARY_DIR} DIRECTORY)
   get_filename_component(platform ${platform}         NAME)

   if (${platform} IN_LIST LEGITIMATE_PLATFORMS)
      # Legitimate platform
      # message ("Target Platform <${platform}> is okay")
   else ()
      # Bad platform
      message (FATAL_ERROR 
      "\nTarget Platform <${platform}> must be one of: ${LEGITIMATE_PLATFORMS}")
    endif ()

endmacro ()
# --------------------------------------------------------



# --------------------------------------------------------
function (set_platform_toolchain platform)

   set (CMAKE_TOOLCHAIN_FILE 
        ${CMAKE_CURRENT_SOURCE_DIR}/cmakeutils/${platform}.cmake
        PARENT_SCOPE)

   #message ("CMAKE_TOOLCHAIN_FILE     = ${CMAKE_TOOLCHAIN_FILE}")   
   #message ("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

endfunction ()
# --------------------------------------------------------



# --------------------------------------------------------
function (setup_paths project_ret install_ret platform)

   get_filename_component (project ${CMAKE_SOURCE_DIR}/../        ABSOLUTE)
   get_filename_component (install ${project}/install/${platform} ABSOLUTE)

   set (${project_ret} ${project} PARENT_SCOPE)
   set (${install_ret} ${install} PARENT_SCOPE)

endfunction ()
# --------------------------------------------------------


# --------------------------------------------------------
function (print_paths project_root install_prefix)

   message ("-----------------------------------------------------")
   message ("project_root   = ${project_root}")
   message ("install_prefix = ${install_prefix}")
   message ("-----------------------------------------------------")

endfunction ()
# --------------------------------------------------------



# --------------------------------------------------------
#
# \brief      Composes and sets the standard SO versioning
#
# \param[in]  target The so target library
# \param[in]   major The major version number
# \param[in]   minor The minor version number
# \param[in]   patch The patch version number
#
# --------------------------------------------------------
function (set_target_soversion target major minor patch)

  set_target_properties (${target} PROPERTIES 
                                   SOVERSION ${major}
                                     VERSION ${major}.${minor}.${patch})

endfunction ()
# --------------------------------------------------------

macro (setup_git_hash)

   # Set version from git tag
   find_package(Git QUIET)
   if (GIT_FOUND)
      execute_process (
         COMMAND ${GIT_EXECUTABLE} --git-dir=${CMAKE_SOURCE_DIR}/../../../.git  describe --tags --dirty
         OUTPUT_VARIABLE ROGUE_CODA_VERSION
         OUTPUT_STRIP_TRAILING_WHITESPACE)
   else()
      message("")
      message(FATAL_ERROR "Git is required to build rogue_coda!")
   endif()

   # Configuration File
   #configure_file (
   #   ${PROJECT_SOURCE_DIR}/../include/mathusla/RogueCodaVersion.h.in
   #   ${PROJECT_BINARY_DIR}/RogueCodaVersion.h
   #)

endmacro ()


# --------------------------------------------------------
# \brief Setup the CONDA enviroment if defined
# --------------------------------------------------------
macro (setup_qt5)

  #set (CMAKE_AUTOMOC ON)
  #set (CMAKE_AUTORCC ON)
  #set (CMAKE_AUTOUIC ON)

  set (Qt5_DIR ${ROGUE_DIR}/cmake/Qt5)

  set (Qt5_LIBRARIES /usr/lib64/libglapi.so /usr/lib64/libselinux.so /usr/lib64/libXext.so /usr/lib64/libXdamage.so /usr/lib64/libXfixes.so /usr/lib64/libXxf86vm.so /usr/lib64/libXau.so) # /usr/lib64/libxcb-glx.so)


  find_package (Qt5 COMPONENTS Widgets REQUIRED)

endmacro ()
# --------------------------------------------------------


# ----------------------------------------------------------------------
# END  : FUNCTION/MACRO DEFINITIONS
# ======================================================================





# ======================================================================
# BEGIN:
# ----------------------------------------------------------------------


message ("Entering 0: ${CMAKE_CURRENT_SOURCE_DIR}")

# ----------------------------------------------------------------------
# The toolchain must be set before the project declaration
# NOTE: The toolchain is dependent on the build target platform
# -------------------------------------------------------------


get_platform           (platform)
set_platform_toolchain (${platform})
#print_tools ()


# ----------------------------------------------------------------------



# ----------------------------------------------------------------------
# Set the project up
# ------------------

   project (LDMX
            VERSION      0.0.0
            LANGUAGES    C CXX)

  set (PROJECT_DESCRIPTION “LDMX”)

# ----------------------------------------------------------------------



# ----------------------------------------------------------------------
# Setup the rogue/conda environment
# ---------------------------------

# +---------------------------------------------------------+
# | *** Reinstate if/when LDMX becomes a git repository *** |
# +---------------------------------------------------------+
# setup_git_hash ()

setup_conda  ()
# print_conda ()


setup_rogue ()
# print_rogue ()

# -----------------
# ---  setup_qt5 ()
# -----------------

# ----------------------------------------------------------------------

# ------------------------------------------
# Suppress noisy up-to-date install messages
# Does not suppress initial install messages
# ------------------------------------------
  set(CMAKE_INSTALL_MESSAGE LAZY)

# ----------------------------------------------------------------------
# Establish the project and installation root directories
# -------------------------------------------------------

  setup_paths (  project_root    CMAKE_INSTALL_PREFIX ${platform})
  print_paths (${project_root} ${CMAKE_INSTALL_PREFIX})

# ----------------------------------------------------------------------



# ----------------------------------------------------------------------
# Set the build POSTFIX (suffix)
# ------------------------------

  if (CMAKE_BUILD_TYPE STREQUAL Debug)
      set(BUILD_POSTFIX "_d")
  else(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(BUILD_POSTFIX "")
  endif(CMAKE_BUILD_TYPE STREQUAL Debug)

# ----------------------------------------------------------------------



# ----------------------------------------------------------------------
# Set global compile options + include path
# -----------------------------------------

  set     (eudaq_root $ENV{EUDAQ})
  #message ("EUDAQ_ROOT = ${eudaq_root}")
  set     (EUDAQ_INCLUDES     "${eudaq_root}/include")
  set     (EUDAQ_CORE_LIBRARY "${eudaq_root}/lib/libeudaq_core.so")

  include_directories(${PROJECT_BINARY_DIR})
  include_directories (${project_root}/include ${EUDAQ_INCLUDES})



  # --------------------------
  # Only install if instrcuted
  # --------------------------
  if (DEFINED DO_INSTALL)
    
      install (DIRECTORY               ${project_root}/include
               DESTINATION             .
               FILES_MATCHING PATTERN "*.hh" PATTERN "*.h")
             
  endif (DEFINED DO_INSTALL)

# ----------------------------------------------------------------------



# ----------------------------------------------------------------------
# Start the build tree
# --------------------

  add_subdirectory (ldmx)


# ----------------------------------------
# ---  Can't get this to work, qt madness
# ---  add_subdirectory (qt)
# ----------------------------------------

# ----------------------------------------------------------------------
