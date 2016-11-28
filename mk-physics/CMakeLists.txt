cmake_minimum_required (VERSION 2.8)
project (mk-physics)

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

set(MK_PHYSICS_SOURCES src/physics/ocean/Ocean.hpp
                       src/physics/ocean/Ocean.cpp
					   src/glsl/ocean_calculate_spectrum.comp
                       src/glsl/ocean_update_mesh.comp
					   src/glsl/ocean_update_normals.comp)
                       #src/physics/fluids/FLIPSolver2D.hpp
                       #src/physics/fluids/FLIPSolver2D.cpp)

add_library(${PROJECT_NAME} STATIC ${MK_PHYSICS_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${MK_MATH_INCLUDE_DIR} ${MK_RENDERER_INCLUDE_DIR} ${MK_GPGPU_INCLUDE_DIR}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} mk-renderer mk-math)

get_filename_component(INCLUDE_DIR src REALPATH)

set(MK_PHYSICS_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
