#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark.cpp)
target_link_libraries(dmrs_pdsch_processor_benchmark srsvec srslog srsran_signal_processors srsran_sequence_generators)
add_test(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark -s -R 1)
