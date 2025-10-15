#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark.cpp)
target_link_libraries(dmrs_pdsch_processor_benchmark
        srsvec
        srslog
        srsran_dmrs_pdsch
        srsran_phy_support
        srsran_sequence_generators
        srsran_channel_precoder)
add_test(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark -D -s -R 1)


add_executable(srs_estimator_benchmark srs_estimator_benchmark.cpp)
target_link_libraries(srs_estimator_benchmark
        srsvec
        srslog
        srsran_channel_precoder
        srsran_srs_estimator
        srsran_phy_support
        srsran_dft
        srsran_sequence_generators)
add_test(srs_estimator_benchmark srs_estimator_benchmark -D -s -R 1)
