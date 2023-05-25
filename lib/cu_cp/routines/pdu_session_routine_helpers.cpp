/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_routine_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

void srsran::srs_cu_cp::fill_e1ap_drb_pdcp_config(e1ap_pdcp_config& e1ap_pdcp_cfg, const pdcp_config& cu_cp_pdcp_cfg)
{
  e1ap_pdcp_cfg.pdcp_sn_size_ul = cu_cp_pdcp_cfg.tx.sn_size;
  e1ap_pdcp_cfg.pdcp_sn_size_dl = cu_cp_pdcp_cfg.rx.sn_size;
  e1ap_pdcp_cfg.rlc_mod         = cu_cp_pdcp_cfg.rlc_mode;
  if (cu_cp_pdcp_cfg.tx.discard_timer != pdcp_discard_timer::not_configured) {
    e1ap_pdcp_cfg.discard_timer = cu_cp_pdcp_cfg.tx.discard_timer;
  }
  if (cu_cp_pdcp_cfg.rx.t_reordering != pdcp_t_reordering::infinity) {
    e1ap_pdcp_cfg.t_reordering_timer = cu_cp_pdcp_cfg.rx.t_reordering;
  }
}

void srsran::srs_cu_cp::fill_e1ap_qos_flow_param_item(e1ap_qos_flow_qos_param_item&      e1ap_qos_item,
                                                      const srslog::basic_logger&        logger,
                                                      const qos_flow_setup_request_item& request_item)
{
  e1ap_qos_item.qos_flow_id = request_item.qos_flow_id;

  if (request_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.has_value()) {
    non_dyn_5qi_descriptor_t non_dyn_5qi;
    non_dyn_5qi.five_qi = request_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value().five_qi;

    // TODO: Add optional values

    e1ap_qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
  } else {
    // logger.warning("qos_flow_id={}: dynamic 5QI not fully supported.", e1ap_qos_item.qos_flow_id);
    //  TODO: Add dynamic 5qi
  }

  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap;
  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability =
      request_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability;
}

bool srsran::srs_cu_cp::update_setup_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>& ngap_response_list,
    cu_cp_ue_context_modification_request&                                          ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>&    ngap_setup_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                            pdu_session_resource_setup_list,
    const up_config_update& next_config,
    up_resource_manager&    rrc_ue_up_resource_manager,
    srslog::basic_logger&   logger)
{
  // Set up SRB2 if this is the first DRB to be setup
  if (rrc_ue_up_resource_manager.get_nof_drbs() == 0) {
    cu_cp_srbs_to_be_setup_mod_item srb2;
    srb2.srb_id = srb_id_t::srb2;
    ue_context_mod_request.srbs_to_be_setup_mod_list.emplace(srb2.srb_id, srb2);
  }

  for (const auto& e1ap_item : pdu_session_resource_setup_list) {
    // Sanity check - make sure this session ID is present in the original setup message.
    if (ngap_setup_list.contains(e1ap_item.pdu_session_id) == false) {
      logger.error("PDU Session Resource setup request doesn't include setup for PDU session {}",
                   e1ap_item.pdu_session_id);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_setup_list.find(e1ap_item.pdu_session_id) ==
        next_config.pdu_sessions_to_setup_list.end()) {
      logger.error("Didn't expect setup for PDU session {}", e1ap_item.pdu_session_id);
      return false;
    }

    cu_cp_pdu_session_res_setup_response_item item;
    item.pdu_session_id = e1ap_item.pdu_session_id;

    auto& transfer                                    = item.pdu_session_resource_setup_response_transfer;
    transfer.dlqos_flow_per_tnl_info.up_tp_layer_info = e1ap_item.ng_dl_up_tnl_info;

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
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

      // TODO: add DRB verification

      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Verify the QoS flow ID is present in original setup message.
        if (ngap_setup_list[e1ap_item.pdu_session_id].qos_flow_setup_request_items.contains(e1ap_flow.qos_flow_id) ==
            false) {
          logger.error("PDU Session Resource setup request doesn't include setup for QoS flow {} in PDU session {}",
                       e1ap_flow.qos_flow_id,
                       e1ap_item.pdu_session_id);
          return false;
        }

        cu_cp_associated_qos_flow qos_flow;
        qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
        transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.emplace(e1ap_flow.qos_flow_id, qos_flow);
      }

      // Fill UE context modification for DU
      {
        cu_cp_drbs_to_be_setup_mod_item drb_setup_mod_item;
        drb_setup_mod_item.drb_id = e1ap_drb_item.drb_id;

        // Add qos info
        for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
          drb_setup_mod_item.qos_info.drb_qos.qos_characteristics =
              ngap_setup_list[item.pdu_session_id]
                  .qos_flow_setup_request_items[e1ap_flow.qos_flow_id]
                  .qos_flow_level_qos_params.qos_characteristics;

          non_dyn_5qi_descriptor_t non_dyn_5qi;
          non_dyn_5qi.five_qi = ngap_setup_list[item.pdu_session_id]
                                    .qos_flow_setup_request_items[e1ap_flow.qos_flow_id]
                                    .qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value()
                                    .five_qi;

          drb_setup_mod_item.qos_info.drb_qos.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
        }

        // Add up tnl info
        for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
          drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
        }

        // Add rlc mode
        drb_setup_mod_item.rlc_mod = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

        ue_context_mod_request.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
      }
    }

    // Fail on any DRB that fails to be setup
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.error("Non-empty DRB failed list not supported");
      return false;
    }

    ngap_response_list.emplace(item.pdu_session_id, item);
  }

  return true;
}

void srsran::srs_cu_cp::update_failed_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>&     ngap_failed_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& pdu_session_resource_failed_list)
{
  for (const auto& e1ap_item : pdu_session_resource_failed_list) {
    // Add to list taking cause received from CU-UP.
    cu_cp_pdu_session_res_setup_failed_item failed_item;
    failed_item.pdu_session_id                                         = e1ap_item.pdu_session_id;
    failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = e1ap_item.cause;
    ngap_failed_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

bool srsran::srs_cu_cp::update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>&       ngap_response_list,
    cu_cp_ue_context_modification_request&                                                ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
                                e1ap_pdu_session_resource_modify_list,
    const up_config_update&     next_config,
    const srslog::basic_logger& logger)
{
  for (const auto& e1ap_item : e1ap_pdu_session_resource_modify_list) {
    // Sanity check - make sure this session ID is present in the original modify message.
    if (ngap_modify_list.contains(e1ap_item.pdu_session_id) == false) {
      logger.error("PDU Session Resource setup request doesn't include setup for PDU session {}",
                   e1ap_item.pdu_session_id);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_setup_list.find(e1ap_item.pdu_session_id) ==
        next_config.pdu_sessions_to_setup_list.end()) {
      logger.error("Didn't expect setup for PDU session {}", e1ap_item.pdu_session_id);
      return false;
    }

    cu_cp_pdu_session_res_setup_response_item item;
    item.pdu_session_id = e1ap_item.pdu_session_id;

    auto& transfer = item.pdu_session_resource_setup_response_transfer;

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
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

      // TODO: add DRB verification

      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Verify the QoS flow ID is present in original setup message.
        if (ngap_modify_list[e1ap_item.pdu_session_id].transfer.qos_flow_add_or_modify_request_list.contains(
                e1ap_flow.qos_flow_id) == false) {
          logger.error("PDU Session Resource setup request doesn't include setup for QoS flow {} in PDU session {}",
                       e1ap_flow.qos_flow_id,
                       e1ap_item.pdu_session_id);
          return false;
        }

        cu_cp_associated_qos_flow qos_flow;
        qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
        transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.emplace(e1ap_flow.qos_flow_id, qos_flow);
      }

      // Fill UE context modification for DU
      {
        cu_cp_drbs_to_be_setup_mod_item drb_setup_mod_item;
        drb_setup_mod_item.drb_id = e1ap_drb_item.drb_id;

        // Add qos info
        for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
          drb_setup_mod_item.qos_info.drb_qos.qos_characteristics =
              ngap_modify_list[e1ap_item.pdu_session_id]
                  .transfer.qos_flow_add_or_modify_request_list[e1ap_flow.qos_flow_id]
                  .qos_flow_level_qos_params.qos_characteristics;

          non_dyn_5qi_descriptor_t non_dyn_5qi;
          non_dyn_5qi.five_qi = ngap_modify_list[e1ap_item.pdu_session_id]
                                    .transfer.qos_flow_add_or_modify_request_list[e1ap_flow.qos_flow_id]
                                    .qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi.value()
                                    .five_qi;

          drb_setup_mod_item.qos_info.drb_qos.qos_characteristics.non_dyn_5qi = non_dyn_5qi;
        }

        // Add up tnl info
        for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
          drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
        }

        // Add rlc mode
        drb_setup_mod_item.rlc_mod = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

        ue_context_mod_request.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
      }
    }

    // Fail on any DRB that fails to be setup
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.error("Non-empty DRB failed list not supported");
      return false;
    }

    ngap_response_list.emplace(item.pdu_session_id, item);
  }

  return true;
}