find_package(Eigen3 CONFIG REQUIRED)
find_package(libigl CONFIG REQUIRED)

set(TARGET kigumi_bench_libigl)

add_executable(${TARGET}
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME libigl
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    CGAL::CGAL
    Eigen3::Eigen
    igl::igl_core
    igl_copyleft::igl_copyleft_cgal
    igl_copyleft::igl_copyleft_core
    kigumi
)
