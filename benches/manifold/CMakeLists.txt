find_package(PkgConfig REQUIRED)
pkg_check_modules(Clipper2 REQUIRED IMPORTED_TARGET Clipper2)

include(FetchContent)
FetchContent_Declare(
    manifold
    GIT_REPOSITORY https://github.com/elalish/manifold.git
    GIT_TAG master
    GIT_SHALLOW ON
)
set(MANIFOLD_CROSS_SECTION OFF CACHE BOOL "")
set(MANIFOLD_PAR ON CACHE STRING "")
set(MANIFOLD_TEST OFF CACHE BOOL "")
FetchContent_MakeAvailable(manifold)

set(TARGET kigumi_bench_manifold)

add_executable(${TARGET}
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME manifold
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    kigumi
    manifold
)
