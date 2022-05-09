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

add_library(srsgnb_upper_phy_support re_pattern.cpp rb_allocation.cpp rx_softbuffer_pool_impl.cpp)