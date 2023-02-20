#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_data)
set(TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_data)

add_executable(pusch_processor_benchmark pusch_processor_benchmark.cpp)
target_link_libraries(pusch_processor_benchmark
        srsran_phy_support
        srsran_pusch_processor
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_sched
        srslog
        srsvec)
add_test(pusch_processor_benchmark pusch_processor_benchmark -s -R 1 -B 1 -T 2 -P pusch_scs15_5MHz_qpsk_min)