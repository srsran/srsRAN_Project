#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES
        asn1_csi_meas_config_helpers.cpp
        asn1_rrc_config_helpers.cpp
        asn1_sys_info_packer.cpp
        f1ap_configuration_helpers.cpp
        rlc_config_helpers.cpp
        scheduler_configuration_helpers.cpp)

add_library(du_manager_converters STATIC ${SOURCES})
target_link_libraries(du_manager_converters rrc_nr_asn1 srsran_support srsran_ran srsran_sched srsran_cbs_encoder)
