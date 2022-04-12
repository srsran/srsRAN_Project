
set_directory_properties(PROPERTIES LABELS "du_high")

add_executable(du_high_test du_high_test.cpp)
target_link_libraries(du_high_test srsgnb_du_high srsgnb_gateway)
target_include_directories(du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(du_high_test du_high_test)
set_tests_properties(du_high_test PROPERTIES LABELS "tsan")
