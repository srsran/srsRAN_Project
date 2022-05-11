#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(channel_coding)
add_subdirectory(channel_modulation)
add_subdirectory(channel_processors)
add_subdirectory(sequence_generators)
add_subdirectory(signal_processors)

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(re_pattern_test re_pattern_test.cpp)
target_link_libraries(re_pattern_test upper_phy srslog)
add_test(re_pattern_test re_pattern_test)

add_executable(rx_softbuffer_pool_test rx_softbuffer_pool_test.cpp)
target_link_libraries(rx_softbuffer_pool_test upper_phy srslog)
add_test(rx_softbuffer_pool_test rx_softbuffer_pool_test)