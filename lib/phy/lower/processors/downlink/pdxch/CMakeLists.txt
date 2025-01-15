#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_lower_phy_pdxch_processor STATIC
        pdxch_processor_factories.cpp
        pdxch_processor_impl.cpp)
target_link_libraries(srsran_lower_phy_pdxch_processor
        srsran_instrumentation
        srsran_lower_phy_modulation)