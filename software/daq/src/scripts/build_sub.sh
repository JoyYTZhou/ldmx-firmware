#!/bin/bash

# ----------------------------------------------------------------------
#
# PURPOSE
# -------
# Business end of script to run cmake, make and install over specific
# or all platforms
#
# PARAMETERS
# ----------
# The first parameter gives the top level directory.  All other relevant
# directories are found relative to this root.
#
# The remaining parameters drive the build process.
#
# USAGE
# -----
# The -h or --help qualifiers will give the usage help
#
# ----------------------------------------------------------------------


# ----------------------------
# Establish the root directory
# ----------------------------
script_dir=$1; shift;
#echo "script_dir = ${script_dir}"
cmake_dir=`readlink -en ${script_dir}/../`
#echo "cmake_dir = ${cmake_dir}"

# -------------------------------------------------------
# Force the build directory root to the directory up by 1
# -------------------------------------------------------
build_root=`readlink -mn ${cmake_dir}/../build`
#echo "build_root = ${build_root}"


# -------------------
# Extract the options
# -------------------
OPTS=`getopt -o hvap::c::j::m::i::b::p::t:: --long help,jobs::,verbose,dry-run,platforms::,cmake::,make::,install::,build::,type: -n 'Build:' -- "$@"`


# -------------------------
# Check for parsing failure
# -------------------------
if [ $? != 0 ] ; then 
    echo "Failed parsing options.  Do -h for help" >&2 ; 
    exit 1 ;
fi


# -------------------------------------------
# Define the list of all legitimate platforms
# -------------------------------------------
ALL_PLATFORMS=x86_64-linux,arm_CA9-linux


# ----------------
# Set the defaults
# ----------------
VERBOSE=false
HELP=false
PLATFORMS=x86_64-linux
INSTALL=false
CMAKE=false
MAKE=false
BUILD=false
MAKEARGS=
ACTION=false
DRY_RUN=false
JOBS=1
TYPE=Release


# -----------------------------------
# Extract the command line parameters
# -----------------------------------
eval set -- "$OPTS"
while true; do
  case "$1" in
    -v | --verbose ) VERBOSE=true;  shift  ;;
    -h | --help    ) HELP=true;     shift  ;;
    -d | --dry-run ) DRY_RUN=true;  shift  ;;
    -j | --jobs    ) JOBS=$2;       shift; shift;;
    -p | --platforms ) 
          case $2 in
           "") PLATFORMS=all;   shift; shift ;;
            *) PLATFORMS=$2;    shift; shift ;;
         esac ;;

    -t | --type  ) TYPE=$2;     shift; shift;;

    -c | --cmake   ) 
         ACTION=true
         case $2 in
         "") CMAKE=platforms;   shift; shift ;;
          *) CMAKE=$2;        shift; shift ;;
         esac ;;

    -m | --make    )
         ACTION=true 
         case $2 in
         "") MAKE=platforms;    shift; shift ;;
          *) MAKE=$2;         shift; shift ;;
         esac ;;

    -i | --install )
         ACTION=true 
         case $2 in
         "") INSTALL=platforms; shift; shift ;;
          *) INSTALL=$2;      shift; shift ;;
         esac ;;

    -b | --build   )
        ACTION=true 
        case $2 in
        "") BUILD=platforms;    shift; shift ;;
         *) BUILD=$2;         shift; shift ;;
         esac ;;

    -- ) shift; break ;;
    * ) break ;;
  esac
done

#echo "VERBOSE  = ${VERBOSE}"
#echo "INSTALL  = ${INSTALL}"
#echo "HELP     = ${HELP}"
#echo "PLATFORMS= ${PLATFORMS}"
#echo "CMAKE    = ${CMAKE}"
#echo "BUILD    = ${BUILD}"
#echo "MAKE     = ${MAKE}"
#echo "ACTION   = ${ACTION}"
echo "TYPE     = ${TYPE}"



if [ ${HELP} = "true" ] ; then
   man ${script_dir}/Build
   exit -1
fi

# -------------------------------------------
# If no action was specified default to BUILD
# -------------------------------------------
if [ ${ACTION} = "false" ] ; then
   BUILD=platforms
fi
# -------------------------------------------



# -----------------------------------------------------------------------
# If platforms is specified as all, then reset PLATFORMS to ALL_PLATFORMS
# -----------------------------------------------------------------------
if [ ${PLATFORMS} = "all" ] ; then
   PLATFORMS=${ALL_PLATFORMS[@]}
fi
# -----------------------------------------------------------------



# ----------------------------------------------------------------------
# Get and vet the platform list
#
# $1: The platform list specification.  This may be either a list of 
#     named platforms, e.g. x86_64-linux, or logical specifications
#     e.g. 'platforms or '
# ----------------------------------------------------------------------
get_platforms()
{
   # -------------------------------------------------
   # Translate the platform list to physical platforms
   # -------------------------------------------------
   if [ $1 = "platforms" ] ; then
       platforms_comma=${PLATFORMS}
   elif [ $1 = 'all'   ] ; then
       platforms_comma=${ALL_PLATFORMS}
   else
       platforms_comma=$1
   fi

   # -----------------
   # Vet the platforms
   # -----------------
   tgts=(${platforms_comma//,/ })
   lgts=(${ALL_PLATFORMS//,/ })
   for i in "${!tgts[@]}" ; do

     tgt=${tgts[i]}
     #echo "Tgt = ${tgt}"
     found=false

     j=0
     for j in "${!lgts[@]}" ; do
         #echo "Lgt = ${lgts[j]}"
         if [ ${tgts} = ${lgts[j]} ] ; then 
            found=true
            break 
         fi
     done

     if [ ${found} = "false" ] ; then
        echo "Platform \"${tgt}\" is not one of the recognized platforms: <${ALL_PLATFORMS}>"
        exit -1
     fi

   done
}
# ----------------------------------------------------------------------



# ----------------------------------------------------------------------
#
# Execute a command on a platform list with options
#
#  $1 The command to execute           -- make
#  $2 The untranslated platform list   -- all
#  $3 The rest of the command options  -- e.g. VERBOSE=1, -jobs=2, etc
#
# ----------------------------------------------------------------------
doit ()
{
   cmd=$1
   platform=$2
   shift 2



   get_platforms ${platform}
   platform_array=(${platforms_comma//,/ })

   if [ ${DRY_RUN} = "true" ] ; then
       dry_run=echo
   else
       dry_run=""
   fi

   for i in "${!platform_array[@]}" ; do
     ${dry_run} ${cmd}${build_root}/${platform_array[i]}/${TYPE} $@
   done
}
# ----------------------------------------------------------------------

verbose=""
if [ ${VERBOSE} != "false" ] ; then
   verbose="VERBOSE=1"
fi

make_install=""
cmake_install=""
if [ ${TYPE} = "Release" ] ; then
    make_install=install
    cmake_install="-DDO_INSTALL=true"
fi

if [ ${INSTALL} != "false" ] ; then
    make_install=install
    cmake_install="-DDO_INSTALL=true"
fi

# -------------------------------
# BUILDs
# ------
if [ ${BUILD} != "false" ] ; then
   
   doit "cmake -H${cmake_dir} -B" ${BUILD} "-DCMAKE_BUILD_TYPE="${TYPE} ${cmake_install}
   doit "make  -C "     ${BUILD} "--no-print-directory all ${make_install}"
   exit 0
fi
# -------------------------------


# -------------------------------
# CMAKE's
# ------
if [ ${CMAKE} != "false" ] ; then
   doit "cmake -H${cmake_dir} -B" ${CMAKE} "-DCMAKE_BUILD_TYPE="${TYPE}
fi
# -------------------------------


# -------------------------------
# MAKEs
# -----
if [ ${MAKE} != "false" ] ; then
   doit "make -C " ${MAKE} -j${JOBS} --no-print-directory ${verbose}
fi
# -------------------------------


# -------------------------------
# INSTALLs
# --------
if [ ${INSTALL} != "false" ] ; then
   doit "make -C " ${INSTALL} -j${JOBBS} install --no-print-directory ${verbose}
fi
# -------------------------------
