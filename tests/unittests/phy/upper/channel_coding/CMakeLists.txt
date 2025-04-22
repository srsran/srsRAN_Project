#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(crc_calculator_test crc_calculator_test.cpp)
target_link_libraries(crc_calculator_test srsran_channel_coding)
add_test(crc_calculator_test crc_calculator_test -F generic)
add_test(crc_calculator_test crc_calculator_test -F lut)

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)
