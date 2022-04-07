
set_directory_properties(PROPERTIES LABELS "helpers")

add_executable(band_helper_test band_helper_test.cpp)
target_link_libraries(band_helper_test srsgnb_helpers srslog)
target_include_directories(band_helper_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(band_helper_test band_helper_test)
