set(TARGET kigumi_bench_corefinement)

add_executable(${TARGET}
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME corefinement
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    CGAL::CGAL
    kigumi
)
