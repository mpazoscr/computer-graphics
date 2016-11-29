cmake_minimum_required (VERSION 2.8)
project (fluid-demo)

find_package(GLM REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_FOLDER}/${PROJECT_NAME})

set(FLIP_DEMO_SOURCES src/main.cpp)

set(FLIP_DEMO_SHADER_SOURCES resources/shaders/coloured_vertex.vert
                             resources/shaders/coloured_vertex.frag
                             resources/shaders/blur.vert
                             resources/shaders/blur_horizontal.frag
                             resources/shaders/blur_vertical.frag)

add_executable(${PROJECT_NAME} ${FLIP_DEMO_SOURCES} ${FLIP_DEMO_SHADER_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLM_INCLUDE_DIRS}
                                   ${GLEW_INCLUDE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME}
                      mk-renderer
                      mk-physics
                      mk-demofw)

file(COPY resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
