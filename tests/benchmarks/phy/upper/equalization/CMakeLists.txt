#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(channel_equalizer_benchmark channel_equalizer_benchmark.cpp)
target_link_libraries(channel_equalizer_benchmark srsvec srslog srsran_channel_equalizer)
add_test(channel_equalizer_benchmark channel_equalizer_benchmark -s -R 1)
