cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-std=c++11)
add_definitions(-O0 -g)
file(
    GLOB_RECURSE
    SOURCES_FILES
    classify/*.cpp
    Utils/*.cpp
    ModelProcess/*.cpp
    dvpp/*.cpp
)
file(
    GLOB_RECURSE
    HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/../include/*.h
    # ${CMAKE_CURRENT_LIST_DIR}/Utils/*.h
    # /data1/code/Ascend_virgo/src/Utils/*.h
)
message(${HEADER_FILES})
include_directories(
    #${HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/Utils/
    ${CMAKE_CURRENT_LIST_DIR}/ModelProcess/
    ${CMAKE_CURRENT_LIST_DIR}/dvpp/
    ${CMAKE_CURRENT_LIST_DIR}/../include/
)

add_library(
    ${PROJECT_NAME} SHARED
    ${SOURCES_FILES}
    ${HEADER_FILES}
)
target_link_libraries(${PROJECT_NAME} ${3RDLIBS})

install(TARGETS ${PROJECT_NAME}
DESTINATION lib)
install(DIRECTORY ../include
DESTINATION include)


# file(
#     GLOB_RECURSE
#     SOURCES_FILES
#     classify/*.cpp
# )
# file(
#     GLOB_RECURSE
#     HEADER_FILES
#     include/*.h
# )

# include_directories(
#     # ${HEADER_FILES}
#     ${CMAKE_CURRENT_LIST_DIR}/include/
# )

# add_library(
#     ${PROJECT_NAME} SHARED
#     ${SOURCES_FILES}
#     ${HEADER_FILES}
# )
# target_link_libraries(${PROJECT_NAME} ${3RDLIBS})

# install(TARGETS ${PROJECT_NAME}
# DESTINATION lib)
# install(DIRECTORY ../src/include
# DESTINATION include)