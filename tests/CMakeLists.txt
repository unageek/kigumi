set(TARGET kigumi-test)

add_executable(${TARGET}
    Classify_faces_locally_test.cc
    face_data.cc
    Face_face_intersection_tests.cc
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
    Boost::container
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
