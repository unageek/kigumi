set(TARGET corefinement)

add_executable(${TARGET}
    main.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    CGAL::CGAL
)
