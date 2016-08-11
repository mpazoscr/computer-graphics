cmake_minimum_required (VERSION 2.8)
project (flip-demo)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_FOLDER}/${PROJECT_NAME})

set(FLIP_DEMO src/main.cpp)

set(FLIP_DEMO_SHADER_SOURCES resources/shaders/shader_none.vert
                             resources/shaders/shader_blur_horizontal.frag
                             resources/shaders/shader_blur_vertical.frag)

add_executable(${PROJECT_NAME} ${FLIP_DEMO} ${FLIP_DEMO_SHADER_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(${PROJECT_NAME} mk-renderer mk-physics mk-cuda mk-demofw)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/)

file(COPY resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

