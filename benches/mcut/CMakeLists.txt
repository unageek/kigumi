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
