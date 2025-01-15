#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(channel_precoder_test channel_precoder_test.cpp)

target_link_libraries(
        channel_precoder_test
        srsran_channel_precoder
        srsran_ran
        srslog
        srsvec
        gtest
        gtest_main)

add_test(channel_precoder_test channel_precoder_test)