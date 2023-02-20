#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_ran
        csi_rs/csi_rs_pattern.cpp
        pdcch/cce_to_prb_mapping.cpp
        pdcch/dci_packing.cpp
        pdcch/pdcch_candidates.cpp
        pdcch/pdcch_type0_css_coreset_config.cpp
        pdcch/pdcch_type0_css_occasions.cpp
        pdsch/dlsch_info.cpp
        pdsch/pdsch_mcs.cpp
        prach/prach_configuration.cpp
        prach/prach_cyclic_shifts.cpp
        prach/prach_frequency_mapping.cpp
        prach/prach_preamble_information.cpp
        pusch/pusch_mcs.cpp
        pusch/pusch_uci_beta_offset.cpp
        pusch/ulsch_info.cpp
        band_helper.cpp
        resource_allocation/resource_allocation_frequency.cpp
        sch/sch_segmentation.cpp
        ssb_freq_position_generator.cpp
        ssb_mapping.cpp
        tdd_ul_dl_config.cpp)
target_link_libraries(srsran_ran)