include(FetchContent)
FetchContent_Declare(
    mcut
    GIT_REPOSITORY https://github.com/cutdigital/mcut.git
    GIT_TAG master
    GIT_SHALLOW ON
)
FetchContent_GetProperties(mcut)
if(NOT mcut_POPULATED)
    FetchContent_Populate(mcut)
    add_subdirectory(${mcut_SOURCE_DIR} ${mcut_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(TARGET kigumi_bench_mcut)

add_executable(${TARGET}
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME mcut
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_include_directories(${TARGET} PRIVATE
    ${MCUT_INCLUDE_DIR}
)

target_link_libraries(${TARGET} PRIVATE
    kigumi
    mcut
)
