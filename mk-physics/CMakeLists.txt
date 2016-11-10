cmake_minimum_required (VERSION 2.8)
project (mk-physics)

find_package(GLM REQUIRED)

set(MK_PHYSICS_SOURCES src/physics/waves/Ocean.hpp
                       src/physics/waves/Ocean.cpp
                       src/physics/waves/cuda/Ocean_cu.hpp
                       src/physics/waves/cuda/Ocean.cu
					             src/physics/waves/gl/ocean_calculate_spectrum.comp
                       src/physics/waves/gl/ocean_update_mesh.comp
					             src/physics/waves/gl/ocean_update_normals.comp)
                       #src/physics/fluids/FLIPSolver2D.hpp
                       #src/physics/fluids/FLIPSolver2D.cpp)

set(MK_PHYSICS_CUDA_SOURCES src/physics/waves/cuda/Ocean.cu)

add_library(${PROJECT_NAME} STATIC ${MK_PHYSICS_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLM_INCLUDE_DIRS} ${MK_RENDERER_INCLUDE_DIR}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} mk-renderer)
