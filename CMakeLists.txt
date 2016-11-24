cmake_minimum_required (VERSION 2.8)

project(computer-graphics)

set(OUTPUT_FOLDER ${CMAKE_SOURCE_DIR}/output)
set(OUTPUT_LIB_FOLDER ${OUTPUT_FOLDER}/lib)
set(OUTPUT_BIN_FOLDER ${OUTPUT_FOLDER}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_FOLDER})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB_FOLDER})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DGLEW_STATIC")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(mk-math)
add_subdirectory(mk-demofw)
add_subdirectory(mk-renderer)
add_subdirectory(mk-gpgpu)
add_subdirectory(mk-physics)
add_subdirectory(ocean-demo)
#add_subdirectory(flip-demo)
