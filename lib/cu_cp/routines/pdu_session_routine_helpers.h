/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/pdcp/pdcp_config.h"

using namespace srsran;

namespace srsran {
namespace srs_cu_cp {

void fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg);
void fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                   srslog::basic_logger&              logger,
                                   const qos_flow_setup_request_item& request_item);

bool update_setup_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>& ngap_response_list,
    cu_cp_ue_context_modification_request&                                          ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>&    ngap_setup_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                            pdu_session_resource_setup_list,
    const up_config_update& next_config,
    up_resource_manager&    rrc_ue_up_resource_manager,
    srslog::basic_logger&   logger);

void update_failed_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>&     ngap_failed_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& pdu_session_resource_failed_list);

bool update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>&       ngap_response_list,
    cu_cp_ue_context_modification_request&                                                ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
                            e1ap_pdu_session_resource_modify_list,
    const up_config_update& next_config,
    srslog::basic_logger&   logger);

} // namespace srs_cu_cp
} // namespace srsran
