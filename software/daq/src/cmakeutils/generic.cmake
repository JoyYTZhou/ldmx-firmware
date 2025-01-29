# ---------------------------------------------------------------------- 
#
# Generic, i.e. platform independent CMAKE add-on functions
#
# ---------------------------------------------------------------------- 


message ("Including")


# ---------------------------------------------------------------------- 
#
# Excludes any include files protected by the construct
#
#   #include PYTHON_CONDITIONAL_INCLUDE (include_file)
#
# It does this by defining the macro as almost a noop.  This must be
# done in two places
#
#   1. The CMake preprocessor
#      Method: Defines the macro out of existence
#               PYTHON_CONDITIONAL_INCLUDE(%)=
#
#   2. The gcc   preprocessor
#      Method: Defines the macro to be an beign include
#                PYTHON_CONDITIONAL(\h\)=<cstddef>
# ---------------------------------------------------------------------- 
function (target_exclude_python target)

   set_property(TARGET ${target} 
                APPEND PROPERTY IMPLICIT_DEPENDS_INCLUDE_TRANSFORM
               "PYTHON_CONDITIONAL_INCLUDE(%)=")

   # ---------------------------------------------------------
   # Appending the compile flags property is buggy in CMake: 
   # it produces semicolons in the output
   # ---------------------------------------------------------
   get_property (flags TARGET ${target} PROPERTY COMPILE_FLAGS)
   set (flags "${flags} -D\"PYTHON_CONDITIONAL_INCLUDE\(h\)=<cstddef>\"")
   set_property (TARGET ${target} PROPERTY COMPILE_FLAGS ${flags})

endfunction (target_exclude_python target)
# ---------------------------------------------------------------------- 





# ---------------------------------------------------------------------- 
#
# Includes any include files protected by the construct
#
#   #include PYTHON_CONDITIONAL_INCLUDE (include_file)
#
# It does this by defining the macro as almost a noop.  This must be
# done in two places
#
#   1. The CMake preprocessor
#      Method: Defines the macro out of existence
#               PYTHON_CONDITIONAL_INCLUDE(%)=
#
#   2. The gcc   preprocessor
#      Method: Defines the macro to be an beign include
#                PYTHON_CONDITIONAL(\h\)=<cstddef>
# ---------------------------------------------------------------------- 
function (target_include_python target)

   set_property(TARGET ${target} 
                APPEND PROPERTY IMPLICIT_DEPENDS_INCLUDE_TRANSFORM
               "PYTHON_CONDITIONAL_INCLUDE(%)=<%>")

   # ---------------------------------------------------------
   # Appending the compile flags property is buggy in CMake: 
   # it produces semicolons in the output
   # ---------------------------------------------------------
   get_property (flags TARGET ${target} PROPERTY COMPILE_FLAGS)
   set (flags "${flags} -D\"PYTHON_CONDITIONAL_INCLUDE\(%)=<%>")
   set_property (TARGET ${target} PROPERTY COMPILE_FLAGS ${flags})

endfunction (target_inlude_python target)
# ---------------------------------------------------------------------- 




# ----------------------------------------------------------------------
#
# MACRO
# -----
#  macro (resolve_conditional_includes target definitions)
#
#
# PARAMETERS
# ----------
#         target:  The CMAKE target (i.e. which executable/library)
#    definitions:  A list of the "whats" in the following explanation
#
#
# BRIEF
# -----
# Fancy version to conditionally include or exclude an include file
# protected by the following construct
#
#   #include <what>_CONDITIONAL_INCLUDE(include_file)
#
#   <what> This is a 'package' name, like ROGUE or PYTHON which 
#          conveys what family of includes this is protecting
#
# Whether the include file is included or excluded is determined by
# whether the corresponding compiler property definition is defined
#
#
# EXAMPLE
# -------
# Suppose MyFile.cc has the following lines in it
#
#   #include PYTHON_CONDITIONAL_INCLUDE(<boost/python.hpp>)
#   #include ROGUE_CONDITIONAL_INCLUDE(<rogue/Stream.h>)
#
#   ...and...
#
#   If in the CMakeLists.txt file, ROGUE is defined somewhere by
#
#   add_executable        (MyFileProgram MyFile.cc)
#   set_target_properites (MyFileProgram PROPERTIES COMPILE_DEFINITIONS ROGUE)
#
#   ...then invoking....
#
#   resolve_conditional_includes (MyFile.cc "PYTHON;ROGUE")
#
#   will result in "boost/python.hpp" being excluded and rogue/Strem.h
#   beinginclude, by virtue of ROGUE being defined but PYTHON not.
#
#
# NOTE:
#   For completeness, there should be another version that works on a 
#   source file basis not a target basis
#
#   resolve_source_file_conditional_include (source_file definitions)
#   
# ----------------------------------------------------------------------
macro (resolve_conditional_includes target definitions)

  # -------------------------------------------------------
  # Get the compile definitions currently set on the target
  # -------------------------------------------------------
  get_property(target_defs TARGET ${target} PROPERTY COMPILE_DEFINITIONS)

  message ("target       : ${target}")
  message ("target_defs 0: ${target_defs}")
  message ("definitions  : ${definitions}")
  message ("CXXFLAGS     : ${CMAKE_CXX_FLAGS}")


  # ---------------------------------------------------------
  # Process one definition given to the macro after the other
  # ---------------------------------------------------------
  foreach (def ${definitions})

    # check whether that definition exists on the target
    list(FIND target_defs ${def} ${def}_FOUND)

    message("target_defs: ${target_defs}")
    message("def: ${def}")
    message("def_found: ${${def}_FOUND}")

    if (NOT ${${def}_FOUND} EQUAL -1)

      # ---------------------------------------------------------------------
      # The definition is found:
      # Define the macro as identity for both CMake Parser and C preprocessor
      # ---------------------------------------------------------------------
      message("Found ${def}")

      set_property(TARGET ${target} 
                   APPEND PROPERTY IMPLICIT_DEPENDS_INCLUDE_TRANSFORM
                   "${def}_CONDITIONAL_INCLUDE(%)=<%>")


      # ---------------------------------------------------------
      # Appending the compile flags property is buggy in CMake: 
      # it produces semicolons in the output
      # ---------------------------------------------------------
      get_property(flags TARGET ${target} PROPERTY COMPILE_FLAGS)
      set(flags "${flags} -D\"${def}_CONDITIONAL_INCLUDE\(h\)=<h>\"")
      set_property(TARGET ${target} PROPERTY COMPILE_FLAGS ${flags})

      get_property(flags TARGET ${target} PROPERTY COMPILE_FLAGS)
      message ("flags: ${flags}")

    else(NOT ${${def}_FOUND} EQUAL -1)

      # --------------------------------------------
      # The definition is not found: 
      # Ignore this line in the CMake Parser and 
      # include some trivial macro in C preprocessor
      # --------------------------------------------
      message("Did not find ${def}")


      set_property(TARGET ${target} 
                   APPEND PROPERTY IMPLICIT_DEPENDS_INCLUDE_TRANSFORM
                  "${def}_CONDITIONAL_INCLUDE(%)=")

      # ---------------------------------------------------------
      # Appending the compile flags property is buggy in CMake: 
      # it produces semicolons in the output
      # ---------------------------------------------------------
      get_property(flags TARGET ${target} PROPERTY COMPILE_FLAGS)
      set(flags "${flags} -D\"${def}_CONDITIONAL_INCLUDE\(h\)=<cstddef>\"")
      set_property(TARGET ${target} PROPERTY COMPILE_FLAGS ${flags})

    endif(NOT ${${def}_FOUND} EQUAL -1)

  endforeach(def ${definitions})

endmacro(resolve_conditional_includes)
# ----------------------------------------------------------------------
