# ----------------------------------------------------------------------
#
# LDMX one-time setup script
#
# USAGE:
# $ source <root>/setup_ldmx.sh
#
# AUTHOR:
# jjrussell
#
# DATE:
# 2021.04.08
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
# 2021.04.08  jjr   Cloned from ~/Experiments/mathusla/src/scripts/setup_eb.sh
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

### --- echo "script filename  = $script_fn"
### --- echo "script directory = $(dirname $script_fn)"

script_dir=$(dirname "$script_fn")
alias Build=$script_dir/Build.sh

