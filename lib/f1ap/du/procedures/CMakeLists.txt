#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        gnb_cu_configuration_update_procedure.cpp
        f1ap_du_setup_procedure.cpp
        f1ap_du_ue_context_setup_procedure.cpp
        f1ap_du_ue_context_modification_procedure.cpp
        )

add_library(srsran_f1ap_du_procedures STATIC ${SOURCES})
target_link_libraries(srsran_f1ap_du_procedures f1ap_asn1)
