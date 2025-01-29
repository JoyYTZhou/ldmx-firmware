SET(CMAKE_SYSTEM_NAME Linux)  # Tell CMake we're cross-compiling

include(CMakeForceCompiler)

# Prefix detection only works with compiler id "GNU"
# CMake will look for prefixed g++, cpp, ld, etc. automatically




set (CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
set (CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# The better add_compile_defintions is not available till 3.13
add_definitions (-DARM -D_ARM_PCS_VFP)

add_compile_options (-Wall 
                     -Wno-psabi
                     -fno-zero-initialized-in-bss
                     -march=armv7-a
                     -mtune=cortex-a9
                     -mcpu=cortex-a9
                     -mfpu=neon)



SET(target arm_CA9-linux)