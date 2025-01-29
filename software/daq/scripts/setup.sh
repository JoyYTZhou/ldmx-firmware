# ----------------------------------------------------------------------
#
# LDMX one-time setup script
#
# USAGE:
# $ source <root>/setup.sh
#
# AUTHOR:
# jjrussell
#
# DATE:
# 2020.04.06
#
#
# METHOD:
# This script auto-locates the appropriate directories relative to the
# directory containing this script.
#
#
# HISTORY
#
#       When  Who   What
# ----------  ---   -----------------------------------------------------
# 2020.04.06  jjr   Cloned from ~/mathusla/daq/scripts/setup.sh
# ----------------------------------------------------------------------
called=${BASH_SOURCE[0]}

os=`uname -s`

### ------------------------------
### Make sure the file was sourced
### ------------------------------
if [ $called != $0 ]; then 

   ### ---------------------------------------
   ### Correctly called by sourcing the script
   ### Get the absolute path to the script 
   ### ---------------------------------------
   if [ `uname` == Linux ]; then script_fn=`readlink -fn $called`
   else script_fn=`perl -e "use Cwd 'abs_path'; print abs_path('$called')"`; fi

else

   ### ---------------------------------------------------
   ### Incorrectly called by directly executing the script
   ### ---------------------------------------------------
   echo "Error: This file must be sourced to set PATH and (DY)LD_LIBRARY_PATH"
   exit -1

fi

script_dir=$(dirname "$script_fn")
root_dir=$(dirname "$script_dir")

# --- echo "script filename  = $script_fn"
# --- echo "script directory = $script_dir"
# --- echo "root   directory = $root_dir"

export lroot=$root_dir
export lsrc=$root_dir/src/ldmx
export linc=$root_dir/include/ldmx

export EUDAQ_MODULE_DIR=$root_dir/install/x86_64-linux/lib/
export EUDAQ_MODULE_IGNORE_DEFALUT=YES


# ------------------------------------------------------------------------------
# !!! KLUDGE !!!
# --------------
# Currently this takes not 1, but 2, kludges
#  1, The location of the rogue library path is a kludge
#  2. Including the rogue library path puts version of libtinfo that is
#     incompatible with /bin/sh. This results in a flood of warning messages
#     from make as it executes shell commands.
#
#     The kludge is to insert /lib/x86_64-linux-gnu before the rogue libraries
#     This is a bit dangerous, since the rogue libraries may depend on
#     shareables that are in the rogue domain, but also exist in the standard
#     place.  This is why it is called a kludge.
#
# The inclusion of the rogue library path allows the image activator to find it.
#
# The correct solution is to fix the version of libtinfo in conda rogue.
# 
# ------------------------------------------------------------------------------
export LD_LIBRARY_PATH_RUN=/lib/x86_64-linux-gnu:${LD_LIBRARY_PATH}:${root_dir}/install/x86_64-linux/lib:/afs/slac.stanford.edu/g/reseng/vol26/anaconda/miniconda3/envs/rogue_v5.6.4/lib

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH_RUN}
source $root_dir/src/scripts/setup_ldmx.sh
