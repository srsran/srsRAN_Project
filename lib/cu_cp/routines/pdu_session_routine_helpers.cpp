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

void srsran::srs_cu_cp::fill_rrc_reconfig_args(
    cu_cp_rrc_reconfiguration_procedure_request&                        rrc_reconfig_args,
    const slotted_id_vector<srb_id_t, cu_cp_srbs_to_be_setup_mod_item>& srbs_to_be_setup_mod_list,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&    pdu_sessions,
    const cu_cp_ue_context_modification_response&                       ue_context_modification_response,
    const std::map<pdu_session_id_t, byte_buffer>&                      nas_pdus,
    bool                                                                is_reestablishment)
{
  cu_cp_radio_bearer_config radio_bearer_config;
  // if default DRB is being setup, SRB2 needs to be setup as well
  if (!srbs_to_be_setup_mod_list.empty()) {
    for (const cu_cp_srbs_to_be_setup_mod_item& srb_to_add_mod : srbs_to_be_setup_mod_list) {
      cu_cp_srb_to_add_mod srb = {};
      srb.srb_id               = srb_to_add_mod.srb_id;
      if (is_reestablishment) {
        srb.reestablish_pdcp_present = true;
      }
      radio_bearer_config.srb_to_add_mod_list.emplace(srb_to_add_mod.srb_id, srb);
    }
  }

  for (const auto& pdu_session_to_add_mod : pdu_sessions) {
    // Add radio bearer config
    for (const auto& drb_to_add : pdu_session_to_add_mod.second.drb_to_add) {
      cu_cp_drb_to_add_mod drb_to_add_mod;
      drb_to_add_mod.drb_id = drb_to_add.first;

      if (is_reestablishment) {
        drb_to_add_mod.reestablish_pdcp_present = true;
      } else {
        drb_to_add_mod.pdcp_cfg = drb_to_add.second.pdcp_cfg;

        // Add CN association and SDAP config
        cu_cp_cn_assoc cn_assoc;
        cn_assoc.sdap_cfg       = drb_to_add.second.sdap_cfg;
        drb_to_add_mod.cn_assoc = cn_assoc;
      }

      radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_add.first, drb_to_add_mod);
    }

    for (const auto& drb_to_remove : pdu_session_to_add_mod.second.drb_to_remove) {
      radio_bearer_config.drb_to_release_list.push_back(drb_to_remove);
    }

    // set masterCellGroupConfig as received by DU
    cu_cp_rrc_recfg_v1530_ies rrc_recfg_v1530_ies;
    rrc_recfg_v1530_ies.master_cell_group = ue_context_modification_response.du_to_cu_rrc_info.cell_group_cfg.copy();

    // append NAS PDUs as received by AMF
    if (!nas_pdus.empty()) {
      if (nas_pdus.find(pdu_session_to_add_mod.first) != nas_pdus.end()) {
        if (!nas_pdus.at(pdu_session_to_add_mod.first).empty()) {
          rrc_recfg_v1530_ies.ded_nas_msg_list.push_back(nas_pdus.at(pdu_session_to_add_mod.first).copy());
        }
      }
    }

    rrc_reconfig_args.non_crit_ext = rrc_recfg_v1530_ies;
  }

  if (radio_bearer_config.contains_values()) {
    // Add radio bearer config.
    rrc_reconfig_args.radio_bearer_cfg = radio_bearer_config;
  }
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

      // Make sure the DRB ID returned by E1AP is actually present in next config.
      if (next_config.pdu_sessions_to_setup_list.at(e1ap_item.pdu_session_id).drb_to_add.find(e1ap_drb_item.drb_id) ==
          next_config.pdu_sessions_to_setup_list.at(e1ap_item.pdu_session_id).drb_to_add.end()) {
        logger.error("DRB id {} not part of next configuration", e1ap_drb_item.drb_id);
        return false;
      }

      // Prepare DRB item for DU.
      cu_cp_drbs_to_be_setup_mod_item drb_setup_mod_item;
      drb_setup_mod_item.drb_id = e1ap_drb_item.drb_id;

      // QoS config.
      drb_setup_mod_item.qos_info.drb_qos.qos_characteristics =
          next_config.pdu_sessions_to_setup_list.at(e1ap_item.pdu_session_id)
              .drb_to_add.at(e1ap_drb_item.drb_id)
              .qos_params.qos_characteristics;
      drb_setup_mod_item.qos_info.drb_qos.alloc_and_retention_prio =
          next_config.pdu_sessions_to_setup_list.at(e1ap_item.pdu_session_id)
              .drb_to_add.at(e1ap_drb_item.drb_id)
              .qos_params.alloc_and_retention_prio;

      // Add up tnl info
      for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
        drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
      }

      // QoS flows.
      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Verify the QoS flow ID is present in original setup message.
        if (ngap_setup_list[e1ap_item.pdu_session_id].qos_flow_setup_request_items.contains(e1ap_flow.qos_flow_id) ==
            false) {
          logger.error("PDU Session Resource setup request doesn't include setup for QoS flow {} in PDU session {}",
                       e1ap_flow.qos_flow_id,
                       e1ap_item.pdu_session_id);
          return false;
        }

        // Add flow to NGAP response
        cu_cp_associated_qos_flow qos_flow;
        qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
        transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list.emplace(e1ap_flow.qos_flow_id, qos_flow);

        // Retrieve QoS properties from NGAP request
        const auto& ngap_qos_flow =
            ngap_setup_list[e1ap_item.pdu_session_id].qos_flow_setup_request_items[e1ap_flow.qos_flow_id];

        // Add flow to F1AP DRB item
        cu_cp_flows_mapped_to_drb_item flow_mapped_to_drb;
        flow_mapped_to_drb.qos_flow_id               = e1ap_flow.qos_flow_id;
        flow_mapped_to_drb.qos_flow_level_qos_params = ngap_qos_flow.qos_flow_level_qos_params;
        drb_setup_mod_item.qos_info.flows_mapped_to_drb_list.emplace(flow_mapped_to_drb.qos_flow_id,
                                                                     flow_mapped_to_drb);
      }

      // Add rlc mode
      drb_setup_mod_item.rlc_mod = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

      ue_context_mod_request.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
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

void srsran::srs_cu_cp::fill_drb_to_setup_list(
    slotted_id_vector<drb_id_t, e1ap_drb_to_setup_item_ng_ran>&          e1ap_drb_to_setup_list,
    const slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item>& qos_flow_list,
    const std::map<drb_id_t, up_drb_context>&                            drb_to_add_list,
    const srslog::basic_logger&                                          logger)
{
  for (const auto& drb_to_setup : drb_to_add_list) {
    e1ap_drb_to_setup_item_ng_ran e1ap_drb_setup_item;
    e1ap_drb_setup_item.drb_id = drb_to_setup.first;
    // TODO: set `e1ap_drb_setup_item.drb_inactivity_timer` if configured
    e1ap_drb_setup_item.sdap_cfg = drb_to_setup.second.sdap_cfg;
    fill_e1ap_drb_pdcp_config(e1ap_drb_setup_item.pdcp_cfg, drb_to_setup.second.pdcp_cfg);

    e1ap_cell_group_info_item e1ap_cell_group_item;
    e1ap_cell_group_item.cell_group_id = 0; // TODO: Remove hardcoded value
    e1ap_drb_setup_item.cell_group_info.push_back(e1ap_cell_group_item);

    // Only iterate over the QoS flows mapped to this particular DRB
    for (const auto& qfi : drb_to_setup.second.qos_flows) {
      srsran_assert(qos_flow_list.contains(qfi), "Original setup request doesn't contain for {}", qfi);
      // Lookup the QoS characteristics from the original request.
      const auto&                  qos_flow_params = qos_flow_list[qfi];
      e1ap_qos_flow_qos_param_item e1ap_qos_item;
      fill_e1ap_qos_flow_param_item(e1ap_qos_item, logger, qos_flow_params);
      e1ap_drb_setup_item.qos_flow_info_to_be_setup.emplace(e1ap_qos_item.qos_flow_id, e1ap_qos_item);
    }

    e1ap_drb_to_setup_list.emplace(e1ap_drb_setup_item.drb_id, e1ap_drb_setup_item);
  }
}

void srsran::srs_cu_cp::fill_drb_to_remove_list(std::vector<drb_id_t>&       e1ap_drb_to_remove_list,
                                                const std::vector<drb_id_t>& drb_to_remove_list)
{
  for (const auto& drb_to_remove : drb_to_remove_list) {
    e1ap_drb_to_remove_list.push_back(drb_to_remove);
  }
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
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
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
    if (next_config.pdu_sessions_to_modify_list.find(e1ap_item.pdu_session_id) ==
        next_config.pdu_sessions_to_modify_list.end()) {
      logger.error("Didn't expect modification for PDU session {}", e1ap_item.pdu_session_id);
      return false;
    }

    if (ngap_response_list.contains(e1ap_item.pdu_session_id)) {
      // Load existing response item from previous call.
      logger.debug("Amend to existing NGAP response item for PDU session ID {}", e1ap_item.pdu_session_id);
    } else {
      // Add empty new item;
      cu_cp_pdu_session_resource_modify_response_item new_item;
      new_item.pdu_session_id = e1ap_item.pdu_session_id;
      ngap_response_list.emplace(new_item.pdu_session_id, new_item);
      logger.debug("Insert new NGAP response item for PDU session ID {}", e1ap_item.pdu_session_id);
    }

    // Start/continue filling response item.
    cu_cp_pdu_session_resource_modify_response_item& ngap_item = ngap_response_list[e1ap_item.pdu_session_id];
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

      // Verify DRB is present in calculated config.
      if (next_config.pdu_sessions_to_modify_list.at(e1ap_item.pdu_session_id).drb_to_add.find(e1ap_drb_item.drb_id) ==
          next_config.pdu_sessions_to_modify_list.at(e1ap_item.pdu_session_id).drb_to_add.end()) {
        logger.error("Couldn't find configuration for {}", e1ap_drb_item.drb_id);
        return false;
      }

      auto& next_config_drb_cfg =
          next_config.pdu_sessions_to_modify_list.at(e1ap_item.pdu_session_id).drb_to_add.at(e1ap_drb_item.drb_id);

      // Prepare DRB creation at DU.
      cu_cp_drbs_to_be_setup_mod_item drb_setup_mod_item;
      drb_setup_mod_item.drb_id           = e1ap_drb_item.drb_id;
      drb_setup_mod_item.qos_info.drb_qos = next_config_drb_cfg.qos_params;
      // Add up tnl info
      for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
        drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
      }
      drb_setup_mod_item.rlc_mod = rlc_mode::am; // TODO: is this coming from FiveQI mapping?

      // Note: don't add the final DRB item yet.

      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Verify the QoS flow ID is present in original setup message.
        if (ngap_modify_list[e1ap_item.pdu_session_id].transfer.qos_flow_add_or_modify_request_list.contains(
                e1ap_flow.qos_flow_id) == false) {
          logger.error("PDU Session Resource modify request doesn't include addition for QoS flow {} in PDU session {}",
                       e1ap_flow.qos_flow_id,
                       e1ap_item.pdu_session_id);
          return false;
        }

        // Fill added flows in NGAP response.
        {
          if (!ngap_item.transfer.qos_flow_add_or_modify_response_list.has_value()) {
            // Add list if it's not present yet.
            ngap_item.transfer.qos_flow_add_or_modify_response_list.emplace();
          }

          qos_flow_add_or_mod_response_item qos_flow;
          qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
          ngap_item.transfer.qos_flow_add_or_modify_response_list.value().emplace(qos_flow.qos_flow_id, qos_flow);
        }

        // Fill QoS flows for UE context modification.
        {
          // Add mapped flows and extract required QoS info from original NGAP request
          cu_cp_flows_mapped_to_drb_item mapped_flow_item;
          mapped_flow_item.qos_flow_id = e1ap_flow.qos_flow_id;
          mapped_flow_item.qos_flow_level_qos_params =
              ngap_modify_list[e1ap_item.pdu_session_id]
                  .transfer.qos_flow_add_or_modify_request_list[e1ap_flow.qos_flow_id]
                  .qos_flow_level_qos_params;
          drb_setup_mod_item.qos_info.flows_mapped_to_drb_list.emplace(mapped_flow_item.qos_flow_id, mapped_flow_item);
        }
      }

      // Finally add DRB to setup to UE context modifaction.
      ue_context_mod_request.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
    }

    // Add DRB to be removed to UE context modifcation.
    for (const auto& drb_id : next_config.pdu_sessions_to_modify_list.at(e1ap_item.pdu_session_id).drb_to_remove) {
      ue_context_mod_request.drbs_to_be_released_list.push_back(drb_id);
    }

    // Fail on any DRB that fails to be setup
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.error("Non-empty DRB failed list not supported");
      return false;
    }
  }

  return true;
}

void fill_e1ap_bearer_context_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& e1ap_list,
    const cu_cp_ue_context_modification_response&                             ue_context_modification_response,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&          pdu_sessions_update_list)
{
  /// Iterate over all PDU sessions to be updated and match the containing DRBs.
  for (const auto& pdu_session : pdu_sessions_update_list) {
    // The modifications are only for this PDU session.
    e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
    e1ap_mod_item.pdu_session_id = pdu_session.second.id;

    for (const auto& drb_item : ue_context_modification_response.drbs_setup_mod_list) {
      // Only include the DRB if it belongs to the this session.
      if (pdu_session.second.drb_to_add.find(drb_item.drb_id) != pdu_session.second.drb_to_add.end()) {
        // DRB belongs to this PDU session
        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.drb_id;

        for (const auto& dl_up_param : drb_item.dl_up_tnl_info_to_be_setup_list) {
          e1ap_up_params_item e1ap_dl_up_param;
          e1ap_dl_up_param.up_tnl_info   = dl_up_param.dl_up_tnl_info;
          e1ap_dl_up_param.cell_group_id = 0; // TODO: Remove hardcoded value

          e1ap_drb_item.dl_up_params.push_back(e1ap_dl_up_param);
        }
        e1ap_mod_item.drb_to_modify_list_ng_ran.emplace(drb_item.drb_id, e1ap_drb_item);
      }
    }
    e1ap_list.emplace(e1ap_mod_item.pdu_session_id, e1ap_mod_item);
  }
}

bool srsran::srs_cu_cp::update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    e1ap_bearer_context_modification_request&                                             bearer_ctxt_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const cu_cp_ue_context_modification_response& ue_context_modification_response,
    const up_config_update&                       next_config,
    const srslog::basic_logger&                   logger)
{
  // Fail procedure if (single) DRB couldn't be setup
  if (!ue_context_modification_response.drbs_failed_to_be_setup_mod_list.empty()) {
    logger.error("Couldn't setup {} DRBs at DU.",
                 ue_context_modification_response.drbs_failed_to_be_setup_mod_list.size());
    return false;
  }

  // Only prepare bearer context modifcation request if needed
  if (ue_context_modification_response.drbs_setup_mod_list.empty() and
      ue_context_modification_response.drbs_modified_list.empty()) {
    // No DRB added or updated.
    logger.debug("Skipping preparation of bearer context modification request.");
    bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.reset();
    return ue_context_modification_response.success;
  }

  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.emplace();

  fill_e1ap_bearer_context_list(e1ap_bearer_context_mod.pdu_session_res_to_modify_list,
                                ue_context_modification_response,
                                next_config.pdu_sessions_to_modify_list);

#if 0
  // Let all PDU sessions fail if response is negative.
  if (ue_context_modification_response.success == false) {
    fill_response_failed_list(ngap_response_list, ngap_modify_list);
  }
#endif

  // TODO: traverse other fields

  return ue_context_modification_response.success;
}

bool srsran::srs_cu_cp::update_setup_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>& ngap_response_list,
    e1ap_bearer_context_modification_request&                                       bearer_ctxt_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>&    ngap_setup_list,
    const cu_cp_ue_context_modification_response&                                   ue_context_modification_response,
    const up_config_update&                                                         next_config,
    const srslog::basic_logger&                                                     logger)
{
  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.emplace();

  fill_e1ap_bearer_context_list(e1ap_bearer_context_mod.pdu_session_res_to_modify_list,
                                ue_context_modification_response,
                                next_config.pdu_sessions_to_setup_list);

  return ue_context_modification_response.success;
}
