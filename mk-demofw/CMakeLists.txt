cmake_minimum_required (VERSION 2.8)
project (mk-demofw)

find_package(GLEW REQUIRED)
find_package(GLFW3 3.2 REQUIRED)
find_package(GLM REQUIRED)

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

add_library(${PROJECT_NAME} STATIC ${MK_DEMOFW_SOURCES})

target_include_directories(${PROJECT_NAME}
  PRIVATE ${GLFW3_INCLUDE_DIR}
          ${GLEW_INCLUDE_DIR}
  PUBLIC ${GLM_INCLUDE_DIRS}
         ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(${PROJECT_NAME} 
  PUBLIC ${GLFW3_LIBRARY}
         ${GLEW_LIBRARY})
