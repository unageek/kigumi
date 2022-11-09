set(TARGET boole)

add_executable(${TARGET}
    main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror -fsanitize=undefined)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702 /openmp:llvm)
endif()

target_include_directories(${TARGET} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} PRIVATE
    CGAL::CGAL
    -fsanitize=undefined
)

if(UNIX)
    target_link_libraries(${TARGET} PRIVATE
        OpenMP::OpenMP_CXX
    )
endif()
