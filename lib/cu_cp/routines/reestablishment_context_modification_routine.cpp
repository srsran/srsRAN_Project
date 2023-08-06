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

#include "reestablishment_context_modification_routine.h"
#include "pdu_session_routine_helpers.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

reestablishment_context_modification_routine::reestablishment_context_modification_routine(
    ue_index_t                                    ue_index_,
    du_processor_e1ap_control_notifier&           e1ap_ctrl_notif_,
    du_processor_f1ap_ue_context_notifier&        f1ap_ue_ctxt_notif_,
    du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_,
    up_resource_manager&                          rrc_ue_up_resource_manager_,
    srslog::basic_logger&                         logger_) :
  ue_index(ue_index_),
  e1ap_ctrl_notifier(e1ap_ctrl_notif_),
  f1ap_ue_ctxt_notifier(f1ap_ue_ctxt_notif_),
  rrc_ue_notifier(rrc_ue_notifier_),
  rrc_ue_up_resource_manager(rrc_ue_up_resource_manager_),
  logger(logger_)
{
}

void reestablishment_context_modification_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", ue_index, name());

  // prepare ue context release request in case of failure
  ue_context_release_request.ue_index = ue_index;
  ue_context_release_request.cause    = cause_t::radio_network;

  {
    // prepare first BearerContextModificationRequest
    generate_bearer_context_modification_request_for_new_ul_tnl();

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse and fill subsequent UE context modification
    if (!generate_ue_context_modification_request(
            ue_context_mod_request, bearer_context_modification_response.pdu_session_resource_modified_list)) {
      logger.error("ue={}: \"{}\" failed to modify bearer at CU-UP.", ue_index, name());
      CORO_EARLY_RETURN(false);
    }

    bearer_context_modification_request = {};
  }

  {
    // prepare UE Context Modification Request and call F1 notifier
    ue_context_mod_request.ue_index = ue_index;

    CORO_AWAIT_VALUE(ue_context_modification_response,
                     f1ap_ue_ctxt_notifier.on_ue_context_modification_request(ue_context_mod_request));

    // Handle UE Context Modification Response
    if (!generate_bearer_context_modification(bearer_context_modification_request,
                                              bearer_context_modification_response,
                                              ue_context_modification_response)) {
      logger.error("ue={}: \"{}\" failed to modify UE context at DU.", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
  }

  // Inform CU-UP about the new TEID for UL F1u traffic
  {
    // add remaining fields to BearerContextModificationRequest
    bearer_context_modification_request.ue_index = ue_index;

    // call E1AP procedure and wait for BearerContextModificationResponse
    CORO_AWAIT_VALUE(bearer_context_modification_response,
                     e1ap_ctrl_notifier.on_bearer_context_modification_request(bearer_context_modification_request));

    // Handle BearerContextModificationResponse
    if (!generate_ue_context_modification_request(
            ue_context_mod_request, bearer_context_modification_response.pdu_session_resource_modified_list)) {
      logger.error("ue={}: \"{}\" failed to modify bearer at CU-UP.", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
  }

  {
    // prepare RRC Reconfiguration and call RRC UE notifier
    {
      // add SRB2 again
      slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item> srbs_to_setup_list;
      f1ap_srbs_to_be_setup_mod_item                              srb_to_setup = {};

      srb_to_setup.srb_id = srb_id_t::srb2;
      srbs_to_setup_list.insert(srb_id_t::srb2, srb_to_setup);

      // convert pdu session context
      std::map<pdu_session_id_t, up_pdu_session_context_update> pdu_sessions_to_setup_list;
      for (const auto& pdu_session_id : rrc_ue_up_resource_manager.get_pdu_sessions()) {
        up_pdu_session_context_update context_update{pdu_session_id};
        context_update.drb_to_add = rrc_ue_up_resource_manager.get_pdu_session_context(pdu_session_id).drbs;

        pdu_sessions_to_setup_list.emplace(pdu_session_id, context_update);
      }

      fill_rrc_reconfig_args(rrc_reconfig_args,
                             srbs_to_setup_list,
                             pdu_sessions_to_setup_list,
                             ue_context_modification_response.du_to_cu_rrc_info,
                             {},
                             rrc_ue_notifier.get_rrc_ue_meas_config(),
                             true);
    }

    CORO_AWAIT_VALUE(rrc_reconfig_result, rrc_ue_notifier.on_rrc_reconfiguration_request(rrc_reconfig_args));

    // Handle RRC Reconfiguration result.
    if (not rrc_reconfig_result) {
      logger.error("ue={}: \"{}\" RRC Reconfiguration failed.", ue_index, name());
      CORO_EARLY_RETURN(false);
    }
  }

  // we are done
  CORO_RETURN(true);
}

bool reestablishment_context_modification_routine::generate_bearer_context_modification_request_for_new_ul_tnl()
{
  bearer_context_modification_request.ue_index                 = ue_index;
  bearer_context_modification_request.new_ul_tnl_info_required = true;

  // Request new UL TNL info for all DRBs
  std::vector<pdu_session_id_t>          pdu_session_ids              = rrc_ue_up_resource_manager.get_pdu_sessions();
  e1ap_ng_ran_bearer_context_mod_request ngran_bearer_context_mod_req = {};
  for (const pdu_session_id_t& psi : pdu_session_ids) {
    e1ap_pdu_session_res_to_modify_item pdu_sess_mod_item;
    pdu_sess_mod_item.pdu_session_id = psi;

    const std::map<drb_id_t, up_drb_context>& drbs = rrc_ue_up_resource_manager.get_pdu_session_context(psi).drbs;
    for (const std::pair<const drb_id_t, up_drb_context>& drb : drbs) {
      logger.debug("Requesting new UL TNL for DRB. psi={}, drb={}", psi, drb.first);
      e1ap_drb_to_modify_item_ng_ran drb_to_mod = {};
      drb_to_mod.drb_id                         = drb.first;
      pdu_sess_mod_item.drb_to_modify_list_ng_ran.emplace(drb_to_mod.drb_id, drb_to_mod);
    }

    ngran_bearer_context_mod_req.pdu_session_res_to_modify_list.emplace(psi, pdu_sess_mod_item);
  }

  bearer_context_modification_request.ng_ran_bearer_context_mod_request = ngran_bearer_context_mod_req;
  return true;
}

bool reestablishment_context_modification_routine::generate_ue_context_modification_request(
    f1ap_ue_context_modification_request& ue_context_mod_req,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
        e1ap_pdu_session_resource_modify_list)
{
  // Set up SRB2 in DU
  f1ap_srbs_to_be_setup_mod_item srb2;
  srb2.srb_id = srb_id_t::srb2;
  ue_context_mod_req.srbs_to_be_setup_mod_list.emplace(srb2.srb_id, srb2);

  for (const auto& e1ap_item : e1ap_pdu_session_resource_modify_list) {
    cu_cp_pdu_session_resource_modify_response_item item;
    item.pdu_session_id = e1ap_item.pdu_session_id;

    for (const auto& e1ap_drb_item : e1ap_item.drb_modified_list_ng_ran) {
      // Catch implementation limitations.
      if (!e1ap_drb_item.flow_failed_list.empty()) {
        logger.warning("Non-empty QoS flow failed list not supported");
        return false;
      }

      // verify only a single UL transport info item is present.
      if (e1ap_drb_item.ul_up_transport_params.size() != 1) {
        logger.error("Multiple UL UP transport items not supported");
        return false;
      }

      item.transfer.qos_flow_add_or_modify_response_list.emplace();

      // re-establish old flows
      const up_drb_context& drb_up_context = rrc_ue_up_resource_manager.get_drb_context(e1ap_drb_item.drb_id);

      for (const auto& flow : drb_up_context.qos_flows) {
        qos_flow_add_or_mod_response_item qos_flow;
        qos_flow.qos_flow_id = flow.first;
        item.transfer.qos_flow_add_or_modify_response_list.value().emplace(qos_flow.qos_flow_id, qos_flow);
      }

      // Fill UE context modification for DU
      {
        f1ap_drbs_to_be_setup_mod_item drb_setup_mod_item;
        drb_setup_mod_item.drb_id = e1ap_drb_item.drb_id;

        // Add up tnl info
        for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
          drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
        }

        // Add rlc mode
        drb_setup_mod_item.rlc_mod = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

        // fill QoS info
        drb_setup_mod_item.qos_info.drb_qos    = drb_up_context.qos_params;
        drb_setup_mod_item.qos_info.s_nssai    = drb_up_context.s_nssai;
        drb_setup_mod_item.qos_info.notif_ctrl = f1ap_notif_ctrl::active;
        // Fill QoS flows for UE context modification.
        for (const auto& flow : drb_up_context.qos_flows) {
          // Add mapped flows and extract required QoS info from original NGAP request
          f1ap_flows_mapped_to_drb_item mapped_flow_item;
          mapped_flow_item.qos_flow_id               = flow.first;
          mapped_flow_item.qos_flow_level_qos_params = drb_up_context.qos_params;
          drb_setup_mod_item.qos_info.flows_mapped_to_drb_list.emplace(mapped_flow_item.qos_flow_id, mapped_flow_item);
        }
        ue_context_mod_req.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
      }
    }

    // Fail on any DRB that fails to be setup
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.error("Non-empty DRB failed list not supported");
      return false;
    }
  }

  return true;
}

bool reestablishment_context_modification_routine::generate_bearer_context_modification(
    e1ap_bearer_context_modification_request&        bearer_ctxt_mod_req,
    const e1ap_bearer_context_modification_response& bearer_ctxt_mod_resp,
    const f1ap_ue_context_modification_response&     ue_context_modification_resp)
{
  // Fail procedure if (single) DRB couldn't be setup
  if (!ue_context_modification_resp.drbs_failed_to_be_setup_mod_list.empty()) {
    logger.error("Couldn't setup {} DRBs at DU.", ue_context_modification_resp.drbs_failed_to_be_setup_mod_list.size());
    return false;
  }

  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_req.ng_ran_bearer_context_mod_request.emplace();

  /// Iterate over all PDU sessions to be updated and match the containing DRBs.
  for (const auto& pdu_session : bearer_ctxt_mod_resp.pdu_session_resource_modified_list) {
    // The modifications are only for this PDU session.
    e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
    e1ap_mod_item.pdu_session_id = pdu_session.pdu_session_id;

    for (const auto& drb_item : ue_context_modification_resp.drbs_setup_mod_list) {
      // Only include the DRB if it belongs to the this session.
      if (pdu_session.drb_modified_list_ng_ran.contains(drb_item.drb_id)) {
        // DRB belongs to this PDU session
        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.drb_id;

        for (const auto& dl_up_param : drb_item.dl_up_tnl_info_to_be_setup_list) {
          e1ap_up_params_item e1ap_dl_up_param;
          e1ap_dl_up_param.up_tnl_info   = dl_up_param.dl_up_tnl_info;
          e1ap_dl_up_param.cell_group_id = 0; // TODO: Remove hardcoded value

          e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
        }

        // set pdcp reestablishment
        e1ap_drb_item.pdcp_cfg.emplace(e1ap_pdcp_config{});
        e1ap_drb_item.pdcp_cfg->pdcp_reest = true;

        e1ap_mod_item.drb_to_modify_list_ng_ran.emplace(drb_item.drb_id, e1ap_drb_item);
      }
    }
    e1ap_bearer_context_mod.pdu_session_res_to_modify_list.emplace(e1ap_mod_item.pdu_session_id, e1ap_mod_item);
  }

  return true;
}
