#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "support")

add_executable(synchronization_test stop_event_test.cpp sync_event_test.cpp)
target_link_libraries(synchronization_test srsran_support srslog gtest gtest_main)
add_test(synchronization_test synchronization_test)
