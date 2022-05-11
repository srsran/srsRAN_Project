#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding)

add_library(crc_calculator STATIC crc_calculator_impl.cpp)

add_subdirectory(polar)
add_subdirectory(ldpc)
