cmake_minimum_required (VERSION 2.8)
project (mk-demofw)

find_package(PkgConfig REQUIRED)

set(MK_DEMOFW_SOURCES src/demofw/IMouseProvider.hpp
                      src/demofw/IKeyboardProvider.hpp
                      src/demofw/IntervalTimer.hpp
                      src/demofw/IntervalTimer.cpp
                      src/demofw/glfw/MouseProvider.hpp
                      src/demofw/glfw/MouseProvider.cpp
                      src/demofw/glfw/KeyboardProvider.hpp
                      src/demofw/glfw/KeyboardProvider.cpp
                      src/demofw/glfw/BaseDemoApp.hpp
                      src/demofw/glfw/BaseDemoApp.cpp)

pkg_search_module(GLFW REQUIRED glfw3)

add_library(${PROJECT_NAME} STATIC ${MK_DEMOFW_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=c++11")

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${GLFW_INCLUDE_DIRS}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} ${GLFW_STATIC_LIBRARIES})
