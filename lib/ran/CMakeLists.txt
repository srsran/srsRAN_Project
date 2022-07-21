#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_ran band_helper.cpp
        ssb_mapping.cpp
        pdcch/cce_to_prb_mapping.cpp
        pdcch/dci_packing.cpp
        pdcch/pdcch_candidates.cpp
        pdcch/pdcch_type0_css_coreset_config.cpp
        pdsch/pdsch_mcs.cpp
        prach/prach_configuration.cpp
        prach/prach_cyclic_shifts.cpp
        prach/prach_preamble_information.cpp
        pusch/pusch_mcs.cpp
        resource_allocation/resource_allocation_frequency.cpp)
target_link_libraries(srsgnb_ran)

install(TARGETS srsgnb_ran EXPORT srsgnb_export)
