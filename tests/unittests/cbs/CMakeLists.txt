#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

set_directory_properties(PROPERTIES LABELS "cbs")

add_executable(cbs_encoder_test cbs_encoder_test.cpp)
target_link_libraries(cbs_encoder_test srsran_cbs_encoder srsran_support gtest gtest_main)
gtest_discover_tests(cbs_encoder_test)
