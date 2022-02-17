cmake_minimum_required(VERSION 3.5.1)

# project information
project(AscendVirgo)

# Compile options
add_compile_options(-std=c++11)
add_definitions(-O0 -g)
set(CMAKE_CXX_STANDARD 11)


# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "./")
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -O2 -Wall")
add_definitions(-DENABLE_DVPP_INTERFACE)


include_directories(
    /data1/3trirdparty/acllib/
)

link_directories(
    /data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/lib64
    /data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/lib64
    /data1/3trirdparty/Ascend/driver/lib64/driver
)

set(
    3RDLIBS
    libascendcl.so
    libruntime.so
    c_sec
    ascend_hal
    acl_dvpp
)


add_subdirectory(src)
add_subdirectory(test)
