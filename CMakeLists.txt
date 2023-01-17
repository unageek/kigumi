cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION "3.16")

project(kigumi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(CGAL CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

add_subdirectory(examples)
enable_testing()
add_subdirectory(tests)
