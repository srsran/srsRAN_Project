#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        gnb_cu_configuration_update_procedure.cpp
        f1ap_du_setup_procedure.cpp
        f1ap_du_removal_procedure.cpp
        f1ap_du_ue_context_setup_procedure.cpp
        f1ap_du_ue_context_modification_procedure.cpp
        f1ap_du_ue_context_release_procedure.cpp
        f1ap_du_reset_procedure.cpp
        f1ap_du_gnbdu_config_update_procedure.cpp
        f1ap_du_positioning_procedures.cpp
)

add_library(srsran_f1ap_du_procedures STATIC ${SOURCES})
target_link_libraries(srsran_f1ap_du_procedures srsran_f1ap_common srsran_f1ap_du_helpers f1ap_asn1)
