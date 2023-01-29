#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


set(SOURCES f1c_configuration_helpers.cpp
        asn1_cell_group_config_helpers.cpp
        asn1_csi_meas_config_helpers.cpp
        rlc_config_helpers.cpp
        mac_config_helpers.cpp
        scheduler_configuration_helpers.cpp)

add_library(du_manager_converters STATIC ${SOURCES})
target_link_libraries(du_manager_converters rrc_nr_asn1 srsgnb_support srsgnb_ran srsgnb_sched f1ap_asn1)
