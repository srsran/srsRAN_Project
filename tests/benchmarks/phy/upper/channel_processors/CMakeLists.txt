#
# Copyright 2021-2024 Software Radio Systems Limited
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

add_subdirectory(pusch)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# Only implement if hardware-acceleration for the PDSCH is enabled.
if (ENABLE_PDSCH_HWACC)
    add_executable(pdsch_encoder_hwacc_benchmark pdsch_encoder_hwacc_benchmark.cpp)
    set(PDSCH_ENCODER_HWACC_BENCHMARK_LIBRARIES srsran_channel_processors srsran_sched srslog)
    if (DPDK_FOUND)
        set_source_files_properties(pdsch_encoder_hwacc_benchmark.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND")
        list(APPEND PDSCH_ENCODER_HWACC_BENCHMARK_LIBRARIES hal_hwacc_pdsch)
    endif (DPDK_FOUND)
    target_link_libraries(pdsch_encoder_hwacc_benchmark ${PDSCH_ENCODER_HWACC_BENCHMARK_LIBRARIES})
endif (ENABLE_PDSCH_HWACC)

add_executable(pdsch_processor_benchmark pdsch_processor_benchmark.cpp)
set(PDSCH_PROCESSOR_LIBRARIES srsran_phy_support
        srsran_pdsch_processor
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_channel_processors
        srsran_ran
        srslog
        srsvec)

if (ENABLE_PDSCH_HWACC)
    set_source_files_properties(pdsch_processor_benchmark.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PDSCH_ENABLED")
    list(APPEND PDSCH_PROCESSOR_LIBRARIES hal_hwacc_pdsch)
endif (ENABLE_PDSCH_HWACC)

target_link_libraries(pdsch_processor_benchmark ${PDSCH_PROCESSOR_LIBRARIES})
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

