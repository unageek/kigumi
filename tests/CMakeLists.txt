set(TARGET kigumi-test)

add_executable(${TARGET}
    bounded_side_test.cc
    classify_faces_locally_test.cc
    face_data_test.cc
    face_face_intersection_test.cc
    io_test.cc
    special_mesh_test.cc
    special_result_test.cc
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
    -fsanitize=undefined
)

add_test(NAME ${TARGET} COMMAND $<TARGET_FILE:${TARGET}>)
