#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(crc_calculator_test crc_calculator_test.cpp)
target_link_libraries(crc_calculator_test srsgnb_channel_coding)
add_test(crc_calculator_test crc_calculator_test)

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)
