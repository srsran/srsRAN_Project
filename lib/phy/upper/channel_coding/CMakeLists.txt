#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding)

add_library(crc_calculator STATIC crc_calculator_impl.cpp)
add_library(polar_code STATIC polar_code_impl.cpp polar_interleaver_impl.cpp)
add_library(polar_encoder STATIC polar_encoder_impl.cpp polar_allocator_impl.cpp polar_rate_matcher_impl.cpp)
add_library(polar_decoder STATIC polar_decoder_impl.cpp polar_deallocator_impl.cpp polar_rate_dematcher_impl.cpp)
