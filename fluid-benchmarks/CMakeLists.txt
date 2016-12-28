cmake_minimum_required (VERSION 2.8)
project (fluid-benchmarks)

find_package(benchmark REQUIRED)
find_package(BOOST REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_FOLDER}/${PROJECT_NAME})

set(FLUID_BENCHMARK_SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${FLUID_BENCHMARK_SOURCES})

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${Boost_INCLUDE_DIRS}
                                   ${BENCHMARK_INCLUDE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/src/)

if (WIN32)
  target_link_libraries(${PROJECT_NAME} ${BENCHMARK_LIBRARY} shlwapi.lib)
else()
  target_link_libraries(${PROJECT_NAME} ${BENCHMARK_LIBRARY})
endif()
