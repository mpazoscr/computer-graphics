#
# Find OpenGL Mathematics library (GLM) 
#

include(FindPackageHandleStandardArgs)

if (WIN32)
	find_path(GLM_INCLUDE_DIR
		NAMES glm/glm.hpp
		PATHS
		${GLM_ROOT_DIR}/include)
else()
	find_path(GLM_INCLUDE_DIR
		NAMES glm/glm.hpp
		PATHS
		/usr/include
		/usr/local/include
		${GLM_ROOT_DIR}/include)
endif()

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

if (GLM_FOUND)
	set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})
endif()
