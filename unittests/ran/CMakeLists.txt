#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ran")

add_executable(slot_point_test slot_point_test.cpp)
target_link_libraries(slot_point_test srsgnb_support)
add_test(slot_point_test slot_point_test)

add_executable(band_helper_test band_helper_test.cpp)
target_link_libraries(band_helper_test srsgnb_ran srslog)
target_include_directories(band_helper_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(band_helper_test band_helper_test)