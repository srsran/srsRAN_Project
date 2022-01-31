

add_executable(du_test du_test.cc)
target_link_libraries(du_test srsgnb_du)
target_include_directories(du_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(du_test du_test)