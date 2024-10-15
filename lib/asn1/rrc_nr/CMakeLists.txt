#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# RRC NR ASN1
add_library(rrc_nr_asn1 STATIC
        common.cpp
        serving_cell.cpp
        ue_cap.cpp
        bwp_cfg.cpp
        sys_info.cpp
        radio_bearer_cfg.cpp
        dl_ccch_msg_ies.cpp
        dl_dcch_msg_ies.cpp
        ul_ccch_msg_ies.cpp
        ul_dcch_msg_ies.cpp
        pcch_msg_ies.cpp
        dl_dcch_msg.cpp
        ul_dcch_msg.cpp
        nr_ue_variables.cpp
        pc5_rrc_definitions.cpp
        nr_sidelink_preconf.cpp
        cell_group_config.cpp
        bcch_bch_msg.cpp
        bcch_dl_sch_msg.cpp
        pcch_msg.cpp
        ul_ccch_msg.cpp
        ul_ccch1_msg.cpp
        dl_ccch_msg.cpp
        ho_prep_info.cpp
        meas_cfg.cpp
        meas_timing_cfg.cpp
        rrc_nr.cpp)
target_compile_options(rrc_nr_asn1 PRIVATE -Os -fno-exceptions)
target_link_libraries(rrc_nr_asn1 asn1_utils)
