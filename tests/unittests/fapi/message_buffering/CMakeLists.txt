#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(buffered_slot_gateway_impl_test buffered_slot_gateway_impl_test.cpp)
target_link_libraries(buffered_slot_gateway_impl_test srsran_support srsran_fapi_message_builder_test_helpers srslog srsran_fapi gtest gtest_main)
gtest_discover_tests(buffered_slot_gateway_impl_test)
