# ----------------------------------------------------------------------
# Find directory of this script
# ----------------------------------------------------------------------

set called=($_)

set os=`uname -s`

if ( "$called" != "" ) then 

   ### ---------------------------------------
   ### Correctly called by sourcing the script
   ### ---------------------------------------
   if ( $os == "Linux" ) then
      set script_fn=`readlink -fn $called[2]`
   else
      set script_fn=`perl -e "use Cwd 'abs_path'; print abs_path('$called[2]')"`; fi
   endif

else

   ### ---------------------------------------------------
   ### Incorrectly called by directly executing the script
   ### ---------------------------------------------------
   set script_fn = $0

endif

set script_dir=`dirname $script_fn`

# -- echo "script  file name= $script_fn"
# -- echo "script  dir      = $script_dir"


$script_dir/build_sub.sh $script_dir $argv






