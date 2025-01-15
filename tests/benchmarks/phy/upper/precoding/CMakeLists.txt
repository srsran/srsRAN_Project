#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(channel_precoder_benchmark channel_precoder_benchmark.cpp)
target_link_libraries(channel_precoder_benchmark srslog srsran_channel_precoder)
add_test(channel_precoder_benchmark channel_precoder_benchmark -s -R 1)
