cmake_minimum_required(VERSION 3.18)

project(kigumi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS container container_hash endian iterator program_options range)
find_package(CGAL CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(examples)
enable_testing()
add_subdirectory(tests)
