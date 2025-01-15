#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "cu_cp")

add_executable(up_resource_manager_test up_resource_manager_test.cpp)
target_link_libraries(up_resource_manager_test srsran_up_resource_manager du_processor_test_helpers srsran_support srslog gtest gtest_main srsran_e2)
gtest_discover_tests(up_resource_manager_test)
