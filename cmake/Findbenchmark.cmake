find_path(BENCHMARK_INCLUDE_DIR "benchmark.h")
find_library(BENCHMARK_LIBRARY NAMES benchmark)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(benchmark DEFAULT_MSG BENCHMARK_LIBRARY BENCHMARK_INCLUDE_DIR)
