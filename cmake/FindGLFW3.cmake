find_path(GLFW3_INCLUDE_DIR "GLFW/glfw3.h")
find_library(GLFW3_LIBRARY NAMES glfw3 glfw)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW3 DEFAULT_MSG GLFW3_LIBRARY GLFW3_INCLUDE_DIR)
