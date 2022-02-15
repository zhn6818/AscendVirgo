cmake_minimum_required(VERSION 3.5.1)

# project information
project(AscendVirgo)

# Compile options
add_compile_options(-std=c++11)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "./")
# set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -O0 -g -Wall")
# set(CMAKE_CXX_FLAGS_RELEASE "-fPIC -O2 -Wall")


include_directories(
    /data1/3trirdparty/acllib/
)

link_directories(
    /data1/3trirdparty/acllib/lib64/stub
)

set(
    3RDLIBS
    libascendcl.so
)


add_subdirectory(src)
add_subdirectory(test)
