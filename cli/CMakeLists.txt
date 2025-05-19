set(TARGET kigumi_cli)

add_executable(${TARGET}
    boolean_command.cc
    convert_command.cc
    defects_command.cc
    main.cc
)

set_target_properties(${TARGET} PROPERTIES
    OUTPUT_NAME kigumi
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${TARGET} PRIVATE
            -Wno-overriding-option -Wno-unused-command-line-argument
        )
    endif()
endif()

target_link_libraries(${TARGET} PRIVATE
    Boost::program_options
    kigumi
)
