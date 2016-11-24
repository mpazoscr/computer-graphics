cmake_minimum_required (VERSION 2.8)
project (mk-renderer)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(GLFW3 3.2 REQUIRED)
find_package(SOIL REQUIRED)

set(MK_RENDERER_SOURCES src/renderer/VertexTypes.hpp
						src/renderer/Image.hpp
                        src/renderer/Image.cpp
						src/renderer/assets/ResourceLoader.hpp
                        src/renderer/assets/ResourceLoader.cpp
						src/renderer/gl/Vao.hpp
                        src/renderer/gl/Vao.cpp
                        src/renderer/gl/Fbo.hpp
                        src/renderer/gl/Fbo.cpp
                        src/renderer/gl/ShaderProgram.hpp
                        src/renderer/gl/ShaderProgram.cpp
                        src/renderer/gl/CubeMap.hpp
                        src/renderer/gl/CubeMap.cpp
                        src/renderer/gl/Texture.hpp
                        src/renderer/gl/Texture.cpp
                        src/renderer/mesh/RectVolume.hpp
                        src/renderer/mesh/RectVolume.cpp
                        src/renderer/mesh/RectPatch.hpp
                        src/renderer/mesh/RectPatch.cpp
                        src/renderer/mesh/Quad.hpp
                        src/renderer/mesh/Quad.cpp
                        src/renderer/scene/Camera.hpp
                        src/renderer/scene/Camera.cpp
                        src/renderer/scene/Skybox.hpp
                        src/renderer/scene/Skybox.cpp
						src/glsl/skybox_env_map_default.vert
						src/glsl/skybox_env_map_default.frag)

add_library(${PROJECT_NAME} STATIC ${MK_RENDERER_SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE ${OPENGL_INCLUDE_DIR}
		    ${GLM_INCLUDE_DIRS}
		    ${GLEW_INCLUDE_DIR}
		    ${GLFW3_INCLUDE_DIRS}
			${SOIL_INCLUDE_DIR}
			${MK_MATH_INCLUDE_DIR}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} 
	                  ${GLFW3_STATIC_LIBRARIES}
                      ${SOIL_LIBRARY}
	                  ${GLEW_LIBRARY}
	                  ${OPENGL_gl_LIBRARY}
	                  mk-demofw
					  mk-math)

get_filename_component(INCLUDE_DIR src REALPATH)

set(MK_RENDERER_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)
