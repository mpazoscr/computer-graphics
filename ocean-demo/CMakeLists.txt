cmake_minimum_required (VERSION 2.8)
project (ocean-demo)

find_package(GLEW REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_FOLDER}/${PROJECT_NAME})

set(OCEAN_DEMO src/main.cpp)

set(OCEAN_DEMO_SHADER_SOURCES resources/shaders/ocean.vert
                              resources/shaders/ocean.frag
                              resources/shaders/skybox.vert
                              resources/shaders/skybox.frag
                              resources/shaders/skybox_env.vert
                              resources/shaders/skybox_env.frag)

add_executable(${PROJECT_NAME} ${OCEAN_DEMO} ${OCEAN_DEMO_SHADER_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(${PROJECT_NAME} mk-renderer mk-physics mk-gpgpu mk-demofw)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${GLEW_INCLUDE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src/)

file(COPY resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
