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
FetchContent_GetProperties(geogram)
if(NOT geogram_POPULATED)
    FetchContent_Populate(geogram)
    add_subdirectory(${geogram_SOURCE_DIR} ${geogram_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(TARGET kigumi_bench_geogram)

add_executable(${TARGET}
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME geogram
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    geogram
    kigumi
)
