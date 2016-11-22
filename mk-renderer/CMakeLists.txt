cmake_minimum_required (VERSION 2.8)
project (mk-renderer)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(GLFW3 3.2 REQUIRED)
find_package(SOIL REQUIRED)

set(MK_RENDERER_SOURCES src/assets/ResourceLoader.hpp
                        src/assets/ResourceLoader.cpp
                        src/core/VertexTypes.hpp
						src/gl/Vao.hpp
                        src/gl/Vao.cpp
                        src/gl/ShaderProgram.hpp
                        src/gl/ShaderProgram.cpp
                        src/image/EnvironmentMap.hpp
                        src/image/EnvironmentMap.cpp
                        src/image/Image.hpp
                        src/image/Image.cpp
                        src/image/Texture.hpp
                        src/image/Texture.cpp
                        src/math/IFFTSolver.hpp
                        src/math/Utils.hpp
                        src/math/Utils.cpp
                        src/mesh/RectVolume.hpp
                        src/mesh/RectVolume.cpp
                        src/mesh/RectPatch.hpp
                        src/mesh/RectPatch.cpp
                        src/mesh/Quad.hpp
                        src/mesh/Quad.cpp
                        src/scene/Camera.hpp
                        src/scene/Camera.cpp
                        src/scene/Skybox.hpp
                        src/scene/Skybox.cpp
						src/glsl/skybox_env_map_default.vert
						src/glsl/skybox_env_map_default.frag)

add_library(${PROJECT_NAME} STATIC ${MK_RENDERER_SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE ${OPENGL_INCLUDE_DIR}
		    ${GLM_INCLUDE_DIRS}
		    ${GLEW_INCLUDE_DIR}
		    ${GLFW3_INCLUDE_DIRS}
			${SOIL_INCLUDE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} 
	                  ${GLFW3_STATIC_LIBRARIES}
                      ${SOIL_LIBRARY}
	                  ${GLEW_LIBRARY}
	                  ${OPENGL_gl_LIBRARY}
	                  mk-demofw)

get_filename_component(INCLUDE_DIR src REALPATH)

set(MK_RENDERER_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
