cmake_minimum_required(VERSION 3.5.1)

project(test_classify)

add_executable(test_classify1 main.cpp)

# message(${3RDLIBS})

target_link_libraries(
    test_classify1
    AscendVirgo
    ${3RDLIBS}
)