/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_ue_context_update.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a dummy UE Context Release Command.
/// \param[in] ue_index The UE Index to use.
/// \return The dummy UE Context Release Command.
cu_cp_ue_context_release_command generate_ue_context_release_command(ue_index_t ue_index);

/// \brief Generate a dummy PDU Session Resource Setup request.
cu_cp_pdu_session_resource_setup_request generate_pdu_session_resource_setup(ue_index_t ue_index = ue_index_t::min,
                                                                             unsigned   num_pdu_sessions = 1,
                                                                             unsigned   num_qos_flows    = 1);

/// \brief Generate a dummy PDU Session Resource Release Command.
cu_cp_pdu_session_resource_release_command generate_pdu_session_resource_release(ue_index_t ue_index = ue_index_t::min);

/// \brief Generate a dummy PDU Session Resource Modification Request.
cu_cp_pdu_session_resource_modify_request
generate_pdu_session_resource_modification(ue_index_t ue_index = ue_index_t::min, unsigned psi = 1, unsigned qfi = 2);

/// \brief Generate a dummy PDU Session Resource Modification Request with QoS flow to remove item.
cu_cp_pdu_session_resource_modify_request
generate_pdu_session_resource_modification_with_qos_flow_removal(qos_flow_id_t flow_id);

/// \brief Generate a dummy E1AP Bearer Context Setup Response.
e1ap_bearer_context_setup_response generate_e1ap_bearer_context_setup_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                                               gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy E1AP Bearer Context Modification Response.
e1ap_bearer_context_modification_response
generate_e1ap_bearer_context_modification_response(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id,
                                                   gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

/// \brief Generate a dummy CU-CP UE Context Modification Response.
f1ap_ue_context_modification_response generate_f1ap_ue_context_modification_response(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id,
                                                                                     gnb_du_ue_f1ap_id_t du_ue_f1ap_id);

} // namespace srs_cu_cp
} // namespace srsran
