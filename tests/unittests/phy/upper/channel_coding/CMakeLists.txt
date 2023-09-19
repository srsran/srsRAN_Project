#
# Copyright 2021-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#


set_directory_properties(PROPERTIES LABELS "phy")

add_executable(crc_calculator_test crc_calculator_test.cpp)
target_link_libraries(crc_calculator_test srsran_channel_coding)
add_test(crc_calculator_test crc_calculator_test -F generic)
add_test(crc_calculator_test crc_calculator_test -F lut)

if (HAVE_AVX2)
    add_test(crc_calculator_test crc_calculator_test -F clmul)
endif(HAVE_AVX2)

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)
