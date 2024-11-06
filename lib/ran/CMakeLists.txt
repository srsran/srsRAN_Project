#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_ran
        cause/ngap_cause_converters.cpp
        cause/f1ap_cause_converters.cpp
        cause/e1ap_cause_converters.cpp
        csi_report/csi_report_config_helpers.cpp
        csi_report/csi_report_on_pucch_helpers.cpp
        csi_report/csi_report_on_pusch_helpers.cpp
        csi_report/csi_report_on_puxch_helpers.cpp
        csi_rs/csi_rs_pattern.cpp
        csi_rs/csi_rs_config_helpers.cpp
        csi_rs/frequency_allocation_type.cpp
        pdcch/cce_to_prb_mapping.cpp
        pdcch/dci_packing.cpp
        pdcch/pdcch_candidates.cpp
        pdcch/pdcch_type0_css_coreset_config.cpp
        pdcch/pdcch_type0_css_occasions.cpp
        pdcch/search_space.cpp
        pdsch/dlsch_info.cpp
        pdsch/pdsch_mcs.cpp
        pdsch/pdsch_antenna_port_mapping.cpp
        prach/prach_configuration.cpp
        prach/prach_cyclic_shifts.cpp
        prach/prach_frequency_mapping.cpp
        prach/prach_preamble_information.cpp
        prach/prach_helper.cpp
        precoding/precoding_codebooks.cpp
        ptrs/ptrs_pattern.cpp
        pusch/pusch_antenna_port_mapping.cpp
        pusch/pusch_mcs.cpp
        pusch/pusch_tpmi_select.cpp
        pusch/pusch_uci_beta_offset.cpp
        pusch/ulsch_info.cpp
        qos/five_qi_qos_mapping.cpp
        uci/uci_part2_size_calculator.cpp
        band_helper.cpp
        sch/tbs_calculator.cpp
        resource_allocation/resource_allocation_frequency.cpp
        sch/sch_segmentation.cpp
        srs/srs_bandwidth_configuration.cpp
        srs/srs_information.cpp
        ssb_freq_position_generator.cpp
        ssb_gscn.cpp
        ssb_mapping.cpp
        tdd_ul_dl_config.cpp
        drx_config.cpp)
target_link_libraries(srsran_ran srslog)

add_to_exported_libs(srsran_ran)
