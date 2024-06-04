#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ngap_asn1_packer.cpp
        ngap_asn1_utils.cpp
        log_helpers.cpp
        ngap_factory.cpp
        ngap_impl.cpp
        ngap_validators/ngap_validators.cpp
        gateways/n2_connection_client_factory.cpp
        procedures/ng_setup_procedure.cpp
        procedures/ng_reset_procedure.cpp
        procedures/ngap_initial_context_setup_procedure.cpp
        procedures/ngap_dl_nas_message_transfer_procedure.cpp
        procedures/ngap_pdu_session_resource_setup_procedure.cpp
        procedures/ngap_pdu_session_resource_modify_procedure.cpp
        procedures/ngap_pdu_session_resource_release_procedure.cpp
        procedures/ngap_handover_preparation_procedure.cpp
        procedures/ngap_handover_resource_allocation_procedure.cpp
        procedures/ngap_ue_context_release_procedure.cpp
    )

add_library(srsran_ngap STATIC ${SOURCES})
target_link_libraries(srsran_ngap ngap_asn1 srsran_cu_cp srsran_support)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
