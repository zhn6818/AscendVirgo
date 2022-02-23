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

set(
    OpenCV_INCLUDE_DIRS
    /data1/3trirdparty/opencv/opencv4.5.2/include/opencv4

)

include_directories(
    /data1/3trirdparty/acllib/
    ${OpenCV_INCLUDE_DIRS}
)

link_directories(
    /data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/lib64
    /data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/lib64
    /data1/3trirdparty/Ascend/driver/lib64/driver
    /data1/3trirdparty/opencv/opencv4.5.2/lib
    /data1/3trirdparty/usr
    /data1/3trirdparty/tbb/lib
)

set(
    3RDLIBS
    libascendcl.so
    libruntime.so
    c_sec
    ascend_hal
    acl_dvpp
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    tbb
    libtbb.so.2
    png
)


add_subdirectory(src)
add_subdirectory(test)

# /data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/ccec_compiler/bin:/root/.vscode-server/bin/f80445acd5a3dadef24aa209168452a3d97cc326/bin/remote-cli:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/ccec_compiler/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/fwkacllib/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/bin:/data1/3trirdparty/Ascend/ascend-toolkit/latest/atc/ccec_compiler/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin