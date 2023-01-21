set(TARGET mix)

add_executable(${TARGET}
    mix/main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror -fsanitize=undefined)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    Boost::program_options
    CGAL::CGAL
    OpenMP::OpenMP_CXX
    -fsanitize=undefined
)



set(TARGET extract)

add_executable(${TARGET}
    extract/main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror -fsanitize=undefined)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    Boost::program_options
    CGAL::CGAL
    OpenMP::OpenMP_CXX
    -fsanitize=undefined
)
