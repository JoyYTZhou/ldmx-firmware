called=${BASH_SOURCE[0]}

### -----------------------------------------------------------------
### Find root directory
### This depends on whether the file was sourced or directly invoked.
### -----------------------------------------------------------------
if [ $called != $0 ]; then 

   ### ------------------
   ### Script was sourced
   ### ------------------
   if [ `uname` == Linux ]; then script_fn=`readlink -fn $called`
   else script_fn=`perl -e "use Cwd 'abs_path'; print abs_path('$called')"`; fi

else

   ### --------------------------
   ### Scrip was directly invoked
   ### --------------------------
   script_fn=$0

fi


script_dir=$(dirname $script_fn)
$script_dir/build_sub.sh $script_dir "$@"
