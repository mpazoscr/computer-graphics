cmake_minimum_required (VERSION 2.8)
project (mk-physics)

find_package(CUDA QUIET REQUIRED)

set(MK_PHYSICS_SOURCES src/physics/waves/Ocean.h
                       src/physics/waves/Ocean.cpp
                       src/physics/waves/cuda/Ocean_cu.h
                       src/physics/waves/cuda/Ocean.cu)

set(MK_PHYSICS_CUDA_SOURCES src/physics/waves/cuda/Ocean.cu)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/
                    ${MK_RENDERER_INCLUDE_DIR}
                    ${CUDA_INCLUDE_DIRS})

cuda_add_library(${PROJECT_NAME}_cu STATIC ${MK_PHYSICS_CUDA_SOURCES})

target_link_libraries(${PROJECT_NAME}_cu mk-renderer mk-cuda)

add_library(${PROJECT_NAME} STATIC ${MK_PHYSICS_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} mk-renderer mk-cuda ${PROJECT_NAME}_cu)
