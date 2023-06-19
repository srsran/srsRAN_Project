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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

if (USE_PHY_TESTVECTORS)
  add_executable(ofh_compression_test ofh_compression_test.cpp)
  target_link_libraries(ofh_compression_test srsran_ofh_compression srslog gtest gtest_main)
  add_test_vector(ofh_compression_test ofh_compression_test_data.tar.gz "")
  gtest_discover_tests(ofh_compression_test)
endif (USE_PHY_TESTVECTORS)
