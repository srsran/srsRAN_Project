#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "receiver")

add_executable(ofh_message_receiver_test ofh_message_receiver_test.cpp)
target_link_libraries(ofh_message_receiver_test srsran_ofh srsran_support gtest gtest_main)
gtest_discover_tests(ofh_message_receiver_test)
