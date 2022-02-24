

add_executable(du_high_test du_high_test.cc)
target_link_libraries(du_high_test srsgnb_du)
target_include_directories(du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(du_high_test du_test)