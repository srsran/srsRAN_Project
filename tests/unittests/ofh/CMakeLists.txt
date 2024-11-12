#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ofh")

add_subdirectory(compression)
add_subdirectory(ecpri)
add_subdirectory(ethernet)
add_subdirectory(receiver)
add_subdirectory(serdes)
add_subdirectory(support)
add_subdirectory(transmitter)

add_executable(slot_symbol_point_test slot_symbol_point_test.cpp)
target_link_libraries(slot_symbol_point_test srslog gtest gtest_main)
gtest_discover_tests(slot_symbol_point_test)
