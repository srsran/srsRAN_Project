#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_pusch_processor STATIC
        pusch_codeblock_decoder.cpp
        pusch_decoder_impl.cpp
        pusch_demodulator_impl.cpp
        pusch_processor_impl.cpp
        ulsch_demultiplex_impl.cpp)
target_link_libraries(srsran_pusch_processor srsran_upper_phy_support srsran_ran)
