cmake_minimum_required(VERSION 3.18)

option(KIGUMI_BUILD_BENCHES "Build the benchmarks" OFF)
option(KIGUMI_BUILD_CLI "Build the command-line interface" ON)
option(KIGUMI_BUILD_TESTS "Build the unit tests" ON)

if(KIGUMI_BUILD_BENCHES)
    list(APPEND VCPKG_MANIFEST_FEATURES "bench-geogram" "bench-manifold")
endif()

project(kigumi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(boost_container CONFIG REQUIRED)
find_package(boost_container_hash CONFIG REQUIRED)
find_package(boost_endian CONFIG REQUIRED)
find_package(boost_iterator CONFIG REQUIRED)
find_package(boost_program_options CONFIG REQUIRED)
find_package(boost_program_options CONFIG REQUIRED)
find_package(boost_range CONFIG REQUIRED)
find_package(CGAL CONFIG REQUIRED)
find_package(FastFloat CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

set(TARGET kigumi)

add_library(${TARGET} INTERFACE)

target_include_directories(${TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} INTERFACE
    Boost::container
    Boost::container_hash
    Boost::endian
    Boost::iterator
    Boost::range
    CGAL::CGAL
    FastFloat::fast_float
)

if(KIGUMI_BUILD_BENCHES)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Clipper2 REQUIRED IMPORTED_TARGET Clipper2)

    include(FetchContent)

    FetchContent_Declare(
        geogram
        GIT_REPOSITORY https://github.com/BrunoLevy/geogram.git
        GIT_TAG main
        GIT_SHALLOW ON
    )
    set(GEOGRAM_LIB_ONLY ON CACHE BOOL "")
    set(GEOGRAM_WITH_GRAPHICS OFF CACHE BOOL "")
    set(GEOGRAM_WITH_HLBFGS OFF CACHE BOOL "")
    set(GEOGRAM_WITH_LEGACY_NUMERICS OFF CACHE BOOL "")
    set(GEOGRAM_WITH_LUA OFF CACHE BOOL "")
    set(GEOGRAM_WITH_TETGEN OFF CACHE BOOL "")
    set(GEOGRAM_WITH_TRIANGLE OFF CACHE BOOL "")
    FetchContent_MakeAvailable(geogram)

    FetchContent_Declare(
        manifold
        GIT_REPOSITORY https://github.com/elalish/manifold.git
        GIT_TAG master
        GIT_SHALLOW ON
    )
    set(MANIFOLD_CROSS_SECTION OFF CACHE BOOL "")
    set(MANIFOLD_TEST OFF CACHE BOOL "")
    FetchContent_MakeAvailable(manifold)

    add_subdirectory(benches)
endif()

if(KIGUMI_BUILD_CLI)
    add_subdirectory(cli)
endif()

if(KIGUMI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
