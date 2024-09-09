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
