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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

# Only implement if hardware-acceleration for the PUSCH is enabled.
if (ENABLE_PUSCH_HWACC)
    add_executable(pusch_decoder_hwacc_benchmark pusch_decoder_hwacc_benchmark.cpp)
    set(PUSCH_DECODER_HWACC_BENCHMARK_LIBRARIES srsran_phy_support 
            srsran_channel_processors
            srslog
            srsvec)
    if (DPDK_FOUND)
        set_source_files_properties(pusch_decoder_hwacc_benchmark.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND")
        list(APPEND PUSCH_DECODER_HWACC_BENCHMARK_LIBRARIES hal_hwacc_pusch)
    endif (DPDK_FOUND)
    target_link_libraries(pusch_decoder_hwacc_benchmark ${PUSCH_DECODER_HWACC_BENCHMARK_LIBRARIES})
endif (ENABLE_PUSCH_HWACC)

add_executable(pusch_processor_benchmark pusch_processor_benchmark.cpp)
set(PUSCH_PROCESSOR_LIBRARIES srsran_phy_support
        srsran_pusch_processor
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_channel_processors
        srslog
        srsvec)

if (ENABLE_PUSCH_HWACC)
    set_source_files_properties(pusch_processor_benchmark.cpp PROPERTIES COMPILE_DEFINITIONS "HWACC_PUSCH_ENABLED")
    list(APPEND PUSCH_PROCESSOR_LIBRARIES hal_hwacc_pusch)
endif (ENABLE_PUSCH_HWACC)

target_link_libraries(pusch_processor_benchmark ${PUSCH_PROCESSOR_LIBRARIES})
add_test(pusch_processor_benchmark pusch_processor_benchmark -m silent -R 1 -B 1 -T 2 -P pusch_scs15_5MHz_qpsk_min)
