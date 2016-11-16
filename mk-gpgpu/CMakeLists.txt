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

set(MK_GPGPU_GLFFT_SHADER_SOURCES src/gpgpu/gl/GLFFT/glsl/fft_common.comp
                                  src/gpgpu/gl/GLFFT/glsl/fft_main.comp
                                  src/gpgpu/gl/GLFFT/glsl/fft_radix4.comp
                                  src/gpgpu/gl/GLFFT/glsl/fft_radix8.comp
                                  src/gpgpu/gl/GLFFT/glsl/fft_radix16.comp
                                  src/gpgpu/gl/GLFFT/glsl/fft_radix64.comp
                                  src/gpgpu/gl/GLFFT/glsl/fft_shared.comp)

set(MK_GPGPU_SOURCES ${MK_GPGPU_GL_SOURCES}
                     ${MK_GPGPU_GLFFT_SOURCES}
                     ${MK_GPGPU_GLFFT_SHADER_SOURCES})

source_group("glfft" FILES ${MK_GPGPU_GLFFT_SOURCES})
source_group("glfft\\glsl" FILES ${MK_GPGPU_GLFFT_SHADER_SOURCES})

add_library(${PROJECT_NAME} STATIC ${MK_GPGPU_SOURCES})

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLEW_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR} ${GLM_INCLUDE_DIRS} ${MK_RENDERER_INCLUDE_DIR}
						               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} ${GLFW3_LIBRARY} ${GLEW_LIBRARY} mk-renderer)

get_filename_component(INCLUDE_DIR src REALPATH)
get_filename_component(SHADER_DIR src/gpgpu/gl/GLFFT/glsl REALPATH)

set(MK_GPGPU_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
set(MK_GPGPU_SHADER_DIR ${SHADER_DIR} PARENT_SCOPE)
