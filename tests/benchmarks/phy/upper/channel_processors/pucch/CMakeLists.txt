#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(pucch_processor_benchmark pucch_processor_benchmark.cpp)
set(pucch_PROCESSOR_LIBRARIES srsran_phy_support
        srsran_pucch_processor
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_transform_precoding
        srslog
        srsvec)

target_link_libraries(pucch_processor_benchmark
        srsran_channel_equalizer
        srsran_channel_processors
        srsran_pucch_processor
        srsran_transform_precoding
        srslog
        srsvec)
add_test(pucch_processor_benchmark pucch_processor_benchmark -m silent -R 1 -B 1 -T 2)
