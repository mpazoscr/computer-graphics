cmake_minimum_required (VERSION 2.8)
project (mk-gpgpu)

find_package(GLEW REQUIRED)
find_package(GLFW3 3.2 REQUIRED)

set(MK_GPGPU_GL_SOURCES src/gpgpu/gl/DeviceMemory.hpp
						            src/gpgpu/gl/DeviceMemory.cpp
                        src/gpgpu/gl/FFTSolver.hpp
                        src/gpgpu/gl/FFTSolver.cpp)

set(MK_GPGPU_GLFFT_SOURCES src/gpgpu/gl/GLFFT/glfft.hpp
						               src/gpgpu/gl/GLFFT/glfft.cpp
						               src/gpgpu/gl/GLFFT/glfft_common.hpp
						               src/gpgpu/gl/GLFFT/glfft_gl_api_headers.hpp
						               src/gpgpu/gl/GLFFT/glfft_gl_interface.hpp
						               src/gpgpu/gl/GLFFT/glfft_gl_interface.cpp
						               src/gpgpu/gl/GLFFT/glfft_interface.hpp
						               src/gpgpu/gl/GLFFT/glfft_wisdom.hpp
						               src/gpgpu/gl/GLFFT/glfft_wisdom.cpp)

set(MK_GPGPU_SOURCES ${MK_GPGPU_GL_SOURCES}
                     ${MK_GPGPU_GLFFT_SOURCES})

source_group("gl" FILES ${MK_GPGPU_GL_SOURCES})
source_group("gl\\glfft" FILES ${MK_GPGPU_GLFFT_SOURCES})

add_library(${PROJECT_NAME} STATIC ${MK_GPGPU_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLEW_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${MK_RENDERER_INCLUDE_DIR}
						               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} ${GLFW3_LIBRARY} ${GLEW_LIBRARY} mk-renderer)

get_filename_component(INCLUDE_DIR src/ REALPATH)
set(MK_GPGPU_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
