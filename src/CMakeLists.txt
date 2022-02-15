cmake_minimum_required(VERSION 3.5.1)


# message(${PROJECT_SOURCE_DIR})
add_compile_options(-std=c++11)


file(
    GLOB_RECURSE
    SOURCES_FILES
    classify/*.cpp
)
file(
    GLOB_RECURSE
    HEADER_FILES
    include/*.h
)
# message(${CMAKE_CURRENT_LIST_DIR})

include_directories(
    # ${HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
)

add_library(
    ${PROJECT_NAME} SHARED
    ${SOURCES_FILES}
    ${HEADER_FILES}
)
target_link_libraries(${PROJECT_NAME} ${3RDLIBS})

install(TARGETS ${PROJECT_NAME}
DESTINATION lib)
install(DIRECTORY ../src/include
DESTINATION include)


# message(${PROJECT_SOURCE_DIR})