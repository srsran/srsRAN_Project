#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)

add_executable(downlink_processor_benchmark downlink_processor_benchmark.cpp)
target_link_libraries(downlink_processor_benchmark srsgnb_upper_phy srsvec srslog)
add_test(downlink_processor_benchmark downlink_processor_benchmark -s -R 1)
