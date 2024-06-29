set(TARGET mix)

add_executable(${TARGET}
    mix/main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    Boost::container_hash
    Boost::endian
    Boost::iterator
    Boost::program_options
    Boost::range
    CGAL::CGAL
    OpenMP::OpenMP_CXX
)



set(TARGET extract)

add_executable(${TARGET}
    extract/main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    Boost::container_hash
    Boost::endian
    Boost::iterator
    Boost::program_options
    Boost::range
    CGAL::CGAL
    OpenMP::OpenMP_CXX
)
