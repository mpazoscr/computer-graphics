cmake_minimum_required (VERSION 2.8)
project (mk-renderer)

find_package(PkgConfig REQUIRED)
find_package(ImageMagick REQUIRED COMPONENTS Magick++)

set(MK_RENDERER_SOURCES src/assets/ResourceLoader.hpp
                        src/assets/ResourceLoader.cpp
                        src/core/CoreTypes.h
                        src/core/VertexTypes.h
                        src/input/MouseFilters.h
                        src/input/MouseFilters.cpp
                        src/input/IMouseProvider.h
                        src/input/IKeyboardProvider.h
                        src/input/glfw/MouseProvider.h
                        src/input/glfw/MouseProvider.cpp
                        src/input/glfw/KeyboardProvider.h
                        src/input/glfw/KeyboardProvider.cpp
                        src/gl/Vao.h
                        src/gl/Vao.cpp
                        src/gl/ShaderProgram.h
                        src/gl/ShaderProgram.cpp
                        src/image/EnvironmentMap.hpp
                        src/image/EnvironmentMap.cpp
                        src/image/Image.hpp
                        src/image/Image.cpp
                        src/image/Texture.hpp
                        src/image/Texture.cpp
                        src/math/IFFTSolver.h
                        src/math/Utils.h
                        src/math/Utils.cpp
                        src/mesh/RectVolume.h
                        src/mesh/RectVolume.cpp
                        src/mesh/RectPatch.h
                        src/mesh/RectPatch.cpp
                        src/mesh/Quad.hpp
                        src/mesh/Quad.cpp
                        src/scene/Camera.h
                        src/scene/Camera.cpp
                        src/scene/Skybox.h
                        src/scene/Skybox.cpp
                        src/utils/IntervalTimer.h
                        src/utils/IntervalTimer.cpp)

pkg_search_module(GLFW REQUIRED glfw3)

add_library(${PROJECT_NAME} STATIC ${MK_RENDERER_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11 -DMAGICKCORE_QUANTUM_DEPTH=8 -DMAGICKCORE_HDRI_ENABLE=0")

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLFW_INCLUDE_DIRS} ${ImageMagick_INCLUDE_DIRS}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} ${GLFW_STATIC_LIBRARIES} ${ImageMagick_LIBRARIES} GLEW.a)

get_filename_component(INCLUDE_DIR src/ REALPATH)

set(MK_RENDERER_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
