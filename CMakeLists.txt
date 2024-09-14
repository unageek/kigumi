cmake_minimum_required(VERSION 3.18)

option(KIGUMI_BUILD_BENCHES "Build the benchmarks" OFF)
option(KIGUMI_BUILD_CLI "Build the command-line interface" ON)
option(KIGUMI_BUILD_TESTS "Build the unit tests" ON)

if(KIGUMI_BUILD_BENCHES)
    list(APPEND VCPKG_MANIFEST_FEATURES "bench-geogram" "bench-libigl" "bench-manifold" "bench-mcut")
endif()

project(kigumi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost CONFIG REQUIRED COMPONENTS container container_hash endian iterator program_options range unordered)
find_package(CGAL CONFIG REQUIRED)
find_package(FastFloat CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

include(GNUInstallDirs)

set(TARGET kigumi)

add_library(${TARGET} INTERFACE)

target_include_directories(${TARGET} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${TARGET} INTERFACE
    Boost::container
    Boost::container_hash
    Boost::endian
    Boost::iterator
    Boost::range
    Boost::unordered
    CGAL::CGAL
    FastFloat::fast_float
)

if(KIGUMI_BUILD_BENCHES)
    add_subdirectory(benches)
endif()

if(KIGUMI_BUILD_CLI)
    add_subdirectory(cli)
    install(TARGETS kigumi_cli)
endif()

if(KIGUMI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(
    TARGETS ${TARGET}
    EXPORT ${PROJECT_NAME}Targets
)
install(
    FILES cmake/kigumiConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
)

export(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
)
