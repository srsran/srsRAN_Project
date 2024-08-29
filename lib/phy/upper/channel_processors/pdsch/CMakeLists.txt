#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_pdsch_processor STATIC
        factories.cpp
        pdsch_codeblock_processor.cpp
        pdsch_encoder_hw_impl.cpp
        pdsch_encoder_impl.cpp
        pdsch_modulator_impl.cpp
        pdsch_processor_concurrent_impl.cpp
        pdsch_processor_impl.cpp
        pdsch_processor_lite_impl.cpp
        pdsch_processor_validator_impl.cpp)

target_link_libraries(srsran_pdsch_processor srsran_upper_phy_support srsran_ran)
