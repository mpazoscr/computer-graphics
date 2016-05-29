cmake_minimum_required (VERSION 2.8)

project(computer-graphics)

set(OUTPUT_FOLDER ${CMAKE_SOURCE_DIR}/output)
set(OUTPUT_LIB_FOLDER ${OUTPUT_FOLDER}/lib)
set(OUTPUT_BIN_FOLDER ${OUTPUT_FOLDER}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_FOLDER})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB_FOLDER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_FOLDER})

add_subdirectory(mk-renderer)
add_subdirectory(mk-cuda)
add_subdirectory(mk-physics)
add_subdirectory(ocean-demo)
