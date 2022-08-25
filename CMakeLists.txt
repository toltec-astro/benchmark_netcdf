cmake_minimum_required(VERSION 3.20)
project(benchmark_netcdf
    DESCRIPTION
    "Benchmark the NetCDF library"
    HOMEPAGE_URL
        "https://toltec-astro.github.io/benchmark_netcdf"
    )

# Fetch tula
include(FetchContent)
FetchContent_Declare(
  tula
  GIT_REPOSITORY https://github.com/toltec-astro/tula.git
  GIT_TAG        "main"
)
FetchContent_MakeAvailable(tula)

# Add the tula cmake modules
set(CMAKE_MODULE_PATH "${tula_SOURCE_DIR}/tula_cmake/cmake" ${CMAKE_MODULE_PATH})
include(common_settings)

# Check if this project is embedded as sub project.
if(NOT DEFINED BMN_STANDALONE)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(BMN_STANDALONE ON)
    else()
        set(BMN_STANDALONE OFF)
    endif()
endif()

# Add the needed tula modules
set(dep_libs tula::headers)

include(perflibs)
set(dep_libs ${dep_libs} tula::perflibs)

include(logging)
set(dep_libs ${dep_libs} tula::logging)

include(Clipp)
set(dep_libs ${dep_libs} tula::Clipp)

include(NetCDFCXX4)
set(dep_libs ${dep_libs} tula::NetCDFCXX4)

include(Eigen3)
set(dep_libs ${dep_libs} tula::Eigen3)

include(Grppi)
set(dep_libs ${dep_libs} tula::Grppi)

include(Clipp)
set(dep_libs ${dep_libs} tula::Clipp)

add_executable(bmn_cli)
target_sources(bmn_cli
    PRIVATE
    "main.cpp"
    )
target_link_libraries(bmn_cli PRIVATE ${dep_libs})
set_property(TARGET bmn_cli PROPERTY RULE_LAUNCH_COMPILE "\"${CMAKE_COMMAND}\" -E time")
set_property(TARGET bmn_cli PROPERTY OUTPUT_NAME bmn)

# Create concfig headers
generate_gitversion_header(${PROJECT_NAME})
generate_config_header(${PROJECT_NAME})
