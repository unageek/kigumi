set(TARGET kigumi_tests)

add_executable(${TARGET}
    bounded_side_test.cc
    classify_faces_locally_test.cc
    face_data_test.cc
    face_face_intersection_test.cc
    special_mesh_test.cc
    special_result_test.cc
)

if(UNIX)
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Werror)
elseif(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /wd4702)
endif()

target_link_libraries(${TARGET} PRIVATE
    GTest::gtest
    GTest::gtest_main
    kigumi
)

add_test(NAME ${TARGET} COMMAND $<TARGET_FILE:${TARGET}>)
