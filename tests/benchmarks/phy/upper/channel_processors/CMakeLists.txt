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

add_executable(pdsch_processor_benchmark pdsch_processor_benchmark.cpp)
target_link_libraries(pdsch_processor_benchmark
        srsran_phy_support
        srsran_pdsch_processor
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_channel_processors
        srsran_sched
        srslog
        srsvec)
add_test(pdsch_processor_benchmark pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pdsch_scs15_5MHz_qpsk_min -t generic)
add_test(pdsch_processor_benchmark pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P 2port_2layer_scs30_100MHz_256qam -t generic)
add_test(pdsch_processor_benchmark pdsch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pdsch_scs15_5MHz_qpsk_min -t concurrent)

add_executable(prach_detector_benchmark prach_detector_benchmark.cpp)
target_link_libraries(prach_detector_benchmark
        srsran_phy_support
        srsran_channel_processors
        srslog
        srsvec)
add_test(prach_detector_benchmark prach_detector_benchmark -R 1)

add_executable(pusch_processor_benchmark pusch_processor_benchmark.cpp)
target_link_libraries(pusch_processor_benchmark
        srsran_phy_support
        srsran_pusch_processor
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_channel_processors
        srsran_sched
        srslog
        srsvec)
add_test(pusch_processor_benchmark pusch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pusch_scs15_5MHz_qpsk_min)