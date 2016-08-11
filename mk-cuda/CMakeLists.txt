cmake_minimum_required (VERSION 2.8)
project (mk-cuda)

find_package(CUDA QUIET REQUIRED)

set(MK_CUDA_SOURCES src/cuda/DeviceCaps.hpp
                    src/cuda/DeviceCaps.cpp
                    src/cuda/DeviceMemory.hpp
                    src/cuda/DeviceMemory.cpp
                    src/cuda/FFTSolver.hpp
                    src/cuda/FFTSolver.cpp)

add_library(${PROJECT_NAME} STATIC ${MK_CUDA_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PUBLIC ${CUDA_INCLUDE_DIRS} ${MK_RENDERER_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES} ${CUDA_CUFFT_LIBRARIES} mk-renderer)
