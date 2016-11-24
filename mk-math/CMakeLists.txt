cmake_minimum_required (VERSION 2.8)
project (mk-math)

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

set(MK_MATH_SOURCES src/math/Utils.hpp
                    src/math/Utils.cpp)

add_library(${PROJECT_NAME} STATIC ${MK_MATH_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

get_filename_component(INCLUDE_DIR src REALPATH)

set(MK_MATH_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
