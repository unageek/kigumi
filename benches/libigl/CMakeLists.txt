set(TARGET kigumi_bench_libigl)

add_executable(${TARGET}
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME libigl
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    CGAL::CGAL
    Eigen3::Eigen
    igl::igl_core
    igl_copyleft::igl_copyleft_cgal
    igl_copyleft::igl_copyleft_core
    kigumi
)
