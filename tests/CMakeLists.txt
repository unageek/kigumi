set(TARGET kigumi-test)

add_executable(${TARGET}
    face_data.cc
    Local_face_classifier_test.cc
    side_of_mesh.cc
    special.cc
    special_results.cc
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
    Boost::container_hash
    Boost::endian
    Boost::iterator
    Boost::range
    CGAL::CGAL
    GTest::gtest
    GTest::gtest_main
    OpenMP::OpenMP_CXX
    -fsanitize=undefined
)

add_test(NAME ${TARGET} COMMAND $<TARGET_FILE:${TARGET}>)
