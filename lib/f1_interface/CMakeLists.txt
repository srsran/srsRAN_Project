#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cu/f1ap_cu_factory.cpp
        du/f1ap_du_factory.cpp
        du/f1ap_du_impl.cpp
        cu/f1ap_cu_impl.cpp
        cu/procedures/f1ap_ue_context_setup_procedure.cpp
        cu/procedures/f1ap_ue_context_release_procedure.cpp
        cu/procedures/f1ap_ue_context_modification_procedure.cpp
        common/f1ap_asn1_packer.cpp
        common/asn1_helpers.cpp
        du/handlers/f1c_du_packet_handler.cpp
        du/procedures/f1ap_du_setup_procedure.cpp
        du/f1c_du_bearer_impl.cpp
        du/f1ap_du_ue.cpp
    )

add_library(srsgnb_f1 STATIC ${SOURCES})
target_link_libraries(srsgnb_f1 f1ap_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
