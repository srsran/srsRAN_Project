/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "inter_cu_handover_target_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

inter_cu_handover_target_routine::inter_cu_handover_target_routine(
    const ngap_handover_request&           request_,
    du_processor_f1ap_ue_context_notifier& f1ap_ue_ctxt_notif_,
    du_processor_e1ap_control_notifier&    e1ap_ctrl_notif_,
    du_processor_ue_manager&               ue_manager_,
    srslog::basic_logger&                  logger_) :
  request(request_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  ue_manager(ue_manager_),
  logger(logger_)
{
}

void inter_cu_handover_target_routine::operator()(
    coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", request.ue_index, name());

  ue = ue_manager.find_ue(request.ue_index);

  // Perform initial sanity checks on incoming message.
  if (!ue->get_up_resource_manager().validate_request(request.pdu_session_res_setup_list_ho_req)) {
    logger.error("ue={}: \"{}\" Invalid PDU Session Resource Setup", request.ue_index, name());
    CORO_EARLY_RETURN(response);
  }

  {
    // Calculate next user-plane configuration based on incoming setup message.
    next_config = ue->get_up_resource_manager().calculate_update(request.pdu_session_res_setup_list_ho_req);
  }

  {
    // prepare F1AP UE Context Setup Command and call F1AP notifier of target DU
    if (!generate_ue_context_setup_request(target_ue_context_setup_request, next_config)) {
      logger.error("ue={}: \"{}\" failed to generate UE context setup request at DU.", request.ue_index, name());
      CORO_EARLY_RETURN(response);
    }

    CORO_AWAIT_VALUE(target_ue_context_setup_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_setup_request(target_ue_context_setup_request));
    if (target_ue_context_setup_response.ue_index == ue_index_t::invalid) {
      logger.error("ue={}: \"{}\" Failed to create UE at the target DU.", request.ue_index, name());
      CORO_EARLY_RETURN(response);
    }
  }

  // Routine only implemented until here.
  response.success = target_ue_context_setup_response.success;

  CORO_EARLY_RETURN(response);

  {
    ue->get_up_resource_manager().get_nof_drbs();
    // If there is an active E1AP context,
    // prepare Bearer Context Release Command and call E1AP notifier
    bearer_context_modification_command.ue_index = request.ue_index;

    CORO_AWAIT(e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_command));
  }

  CORO_RETURN(response);
}

bool inter_cu_handover_target_routine::generate_ue_context_setup_request(f1ap_ue_context_setup_request& setup_request,
                                                                         const up_config_update&        up_config)
{
  setup_request.serv_cell_idx = 0;
  setup_request.sp_cell_id    = request.source_to_target_transparent_container.target_cell_id;

  // Request setup of SRB0, SRB1 and SRB2
  for (unsigned srb_id = 0; srb_id < 3; ++srb_id) {
    f1ap_srbs_to_be_setup_mod_item srb_item;
    srb_item.srb_id = int_to_srb_id(srb_id);
    setup_request.srbs_to_be_setup_list.emplace(srb_item.srb_id, srb_item);
  }

  for (const auto& pdu_session : up_config.pdu_sessions_to_setup_list) {
    for (const auto& drb : pdu_session.second.drb_to_add) {
      const up_drb_context& drb_context = drb.second;

      f1ap_drbs_to_be_setup_mod_item drb_item;
      drb_item.drb_id           = drb.first;
      drb_item.qos_info.drb_qos = drb_context.qos_params;

      // Add each QoS flow including QoS.
      for (auto& flow : drb_context.qos_flows) {
        f1ap_flows_mapped_to_drb_item flow_item;
        flow_item.qos_flow_id               = flow.first;
        flow_item.qos_flow_level_qos_params = flow.second.qos_params;
        drb_item.qos_info.flows_mapped_to_drb_list.emplace(flow_item.qos_flow_id, flow_item);
      }
      drb_item.ul_up_tnl_info_to_be_setup_list = drb_context.ul_up_tnl_info_to_be_setup_list;
      drb_item.rlc_mod                         = drb_context.rlc_mod;

      setup_request.drbs_to_be_setup_list.emplace(drb_item.drb_id, drb_item);
    }
  }

  return true;
}
