#
# Copyright 2021-2025 Software Radio Systems Limited
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

add_subdirectory(pdcch)
add_subdirectory(pdsch)
add_subdirectory(pucch)
add_subdirectory(pusch)
add_subdirectory(ssb)
add_subdirectory(uci)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

set_directory_properties(PROPERTIES LABELS "phy")

if (USE_PHY_TESTVECTORS)
    add_executable(prach_detector_vectortest prach_detector_vectortest.cpp)
    target_link_libraries(prach_detector_vectortest srsran_channel_processors srsran_dft srsran_phy_support srslog gtest gtest_main)
    add_test_vector(prach_detector_vectortest prach_detector_test_data.tar.gz "")

    add_executable(prach_generator_vectortest prach_generator_vectortest.cpp)
    target_link_libraries(prach_generator_vectortest srsran_channel_processors srsran_dft srslog gtest gtest_main)
    add_test_vector(prach_generator_vectortest prach_generator_test_data.tar.gz "")
endif (USE_PHY_TESTVECTORS)
