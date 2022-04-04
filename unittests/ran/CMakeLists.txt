
set_directory_properties(PROPERTIES LABELS "ran")

add_executable(slot_point_test slot_point_test.cpp)
target_link_libraries(slot_point_test srsgnb_support)
add_test(slot_point_test slot_point_test)
