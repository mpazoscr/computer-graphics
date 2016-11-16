find_path(SOIL_INCLUDE_DIR "SOIL.h")
find_library(SOIL_LIBRARY NAMES SOIL)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SOIL DEFAULT_MSG SOIL_LIBRARY SOIL_INCLUDE_DIR)
