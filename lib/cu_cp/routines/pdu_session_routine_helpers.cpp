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
  e1ap_pdcp_cfg.discard_timer   = cu_cp_pdcp_cfg.tx.discard_timer;
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
    rrc_reconfiguration_procedure_request&                             rrc_reconfig_args,
    const slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item>& srbs_to_be_setup_mod_list,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&   pdu_sessions,
    const f1ap_du_to_cu_rrc_info&                                      du_to_cu_rrc_info,
    const std::map<pdu_session_id_t, byte_buffer>&                     nas_pdus,
    const optional<rrc_meas_cfg>                                       rrc_meas_cfg,
    bool                                                               reestablish_srbs,
    bool                                                               reestablish_drbs)
{
  rrc_radio_bearer_config radio_bearer_config;
  // if default DRB is being setup, SRB2 needs to be setup as well
  if (!srbs_to_be_setup_mod_list.empty()) {
    for (const f1ap_srbs_to_be_setup_mod_item& srb_to_add_mod : srbs_to_be_setup_mod_list) {
      rrc_srb_to_add_mod srb = {};
      srb.srb_id             = srb_to_add_mod.srb_id;
      if (reestablish_srbs) {
        srb.reestablish_pdcp_present = true;
      }
      radio_bearer_config.srb_to_add_mod_list.emplace(srb_to_add_mod.srb_id, srb);
    }
  }

  for (const auto& pdu_session_to_add_mod : pdu_sessions) {
    // Add radio bearer config
    for (const auto& drb_to_add : pdu_session_to_add_mod.second.drb_to_add) {
      rrc_drb_to_add_mod drb_to_add_mod;
      drb_to_add_mod.drb_id = drb_to_add.first;
      if (reestablish_drbs) {
        drb_to_add_mod.reestablish_pdcp_present = true;
      } else {
        drb_to_add_mod.pdcp_cfg = drb_to_add.second.pdcp_cfg;

        // Add CN association and SDAP config
        rrc_cn_assoc cn_assoc;
        cn_assoc.sdap_cfg       = drb_to_add.second.sdap_cfg;
        drb_to_add_mod.cn_assoc = cn_assoc;
      }

      radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_add.first, drb_to_add_mod);
    }

    for (const auto& drb_to_remove : pdu_session_to_add_mod.second.drb_to_remove) {
      radio_bearer_config.drb_to_release_list.push_back(drb_to_remove);
    }

    // set masterCellGroupConfig as received by DU
    rrc_recfg_v1530_ies rrc_recfg_v1530_ies;
    rrc_recfg_v1530_ies.master_cell_group = du_to_cu_rrc_info.cell_group_cfg.copy();

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

  rrc_reconfig_args.meas_cfg = rrc_meas_cfg;
}

bool fill_f1ap_drb_setup_mod_item(f1ap_drbs_to_be_setup_mod_item& drb_setup_mod_item, // Request to setup DRB at DU.
                                  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow>* response_flow_list,
                                  pdu_session_id_t                                             psi,
                                  drb_id_t                                                     drb_id,
                                  up_drb_context& next_drb_config,                 // DRB config (info is written back).
                                  const e1ap_drb_setup_item_ng_ran& e1ap_drb_item, // Response from CU-UP.
                                  const slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item>&
                                                              ngap_qos_flow_setup_items, // Initial request from AMF.
                                  const srslog::basic_logger& logger)
{
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

  // Start filling the DU request.
  drb_setup_mod_item.drb_id = drb_id;

  // QoS config.
  drb_setup_mod_item.qos_info.drb_qos.qos_characteristics      = next_drb_config.qos_params.qos_characteristics;
  drb_setup_mod_item.qos_info.drb_qos.alloc_and_retention_prio = next_drb_config.qos_params.alloc_and_retention_prio;

  // S-NSSAI
  drb_setup_mod_item.qos_info.s_nssai = next_drb_config.s_nssai;

  drb_setup_mod_item.rlc_mod = next_drb_config.rlc_mod;

  // Add up tnl info
  for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
    drb_setup_mod_item.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
    // Store UL tunnel information in DRB context (required for mobility).
    next_drb_config.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
  }

  // QoS flows.
  for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
    // Verify the QoS flow ID is present in original setup message.
    if (ngap_qos_flow_setup_items.contains(e1ap_flow.qos_flow_id) == false) {
      logger.error("PDU Session Resource setup request doesn't include setup for QoS flow {} in PDU session {}",
                   e1ap_flow.qos_flow_id,
                   psi);
      return false;
    }

    // Verify flow is present in next config as well.
    if (next_drb_config.qos_flows.find(e1ap_flow.qos_flow_id) == next_drb_config.qos_flows.end()) {
      logger.error("UP config for {} doesn't include setup for QoS flow {} in PDU session {}",
                   drb_id,
                   e1ap_flow.qos_flow_id,
                   psi);
      return false;
    }

    if (response_flow_list) {
      // Add flow to NGAP response
      cu_cp_associated_qos_flow qos_flow;
      qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
      response_flow_list->emplace(e1ap_flow.qos_flow_id, qos_flow);
    }

    // Retrieve QoS properties from NGAP request
    const auto& ngap_qos_flow = ngap_qos_flow_setup_items[e1ap_flow.qos_flow_id];

    // Add flow to F1AP DRB item
    f1ap_flows_mapped_to_drb_item flow_mapped_to_drb;
    flow_mapped_to_drb.qos_flow_id               = e1ap_flow.qos_flow_id;
    flow_mapped_to_drb.qos_flow_level_qos_params = ngap_qos_flow.qos_flow_level_qos_params;

    drb_setup_mod_item.qos_info.flows_mapped_to_drb_list.emplace(flow_mapped_to_drb.qos_flow_id, flow_mapped_to_drb);

    // Store flow QoS params in UP config.
    auto& next_config_flow_cfg      = next_drb_config.qos_flows.at(e1ap_flow.qos_flow_id);
    next_config_flow_cfg.qos_params = ngap_qos_flow.qos_flow_level_qos_params;
  }

  return true;
}

bool srsran::srs_cu_cp::update_setup_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item>& ngap_response_list,
    slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item>&                    srb_setup_mod_list,
    slotted_id_vector<drb_id_t, f1ap_drbs_to_be_setup_mod_item>&                    drb_setup_mod_list,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>&    ngap_setup_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                 pdu_session_resource_setup_list,
    up_config_update&            next_config,
    up_resource_manager&         rrc_ue_up_resource_manager,
    const security_indication_t& default_security_indication,
    const srslog::basic_logger&  logger)
{
  // Set up SRB2 if this is the first DRB to be setup
  if (rrc_ue_up_resource_manager.get_nof_drbs() == 0) {
    f1ap_srbs_to_be_setup_mod_item srb2;
    srb2.srb_id = srb_id_t::srb2;
    srb_setup_mod_list.emplace(srb2.srb_id, srb2);
  }

  for (const auto& e1ap_item : pdu_session_resource_setup_list) {
    const auto& psi = e1ap_item.pdu_session_id;

    // Sanity check - make sure this session ID is present in the original setup message.
    if (ngap_setup_list.contains(psi) == false) {
      logger.error("PDU Session Resource setup request doesn't include setup for PDU session {}", psi);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_setup_list.find(psi) == next_config.pdu_sessions_to_setup_list.end()) {
      logger.error("Didn't expect setup for PDU session {}", psi);
      return false;
    }

    cu_cp_pdu_session_res_setup_response_item item;
    item.pdu_session_id = psi;

    auto& transfer                                    = item.pdu_session_resource_setup_response_transfer;
    transfer.dlqos_flow_per_tnl_info.up_tp_layer_info = e1ap_item.ng_dl_up_tnl_info;

    // Determine security settings for this PDU session and decide whether we have to send the security_result via NGAP
    bool integrity_enabled = false;
    bool ciphering_enabled = false;

    if (ngap_setup_list[psi].security_ind.has_value()) {
      // TS 38.413 Sec. 8.2.1.2:
      // For each PDU session for which the Security Indication IE is included in the PDU Session Resource Setup Request
      // Transfer IE of the PDU SESSION RESOURCE SETUP REQUEST message, and the Integrity Protection Indication IE
      // or Confidentiality Protection Indication IE is set to "preferred", then the NG-RAN node should, if supported,
      // perform user plane integrity protection or ciphering, respectively, for the concerned PDU session and shall
      // notify whether it performed the user plane integrity protection or ciphering by including the Integrity
      // Protection Result IE or Confidentiality Protection Result IE, respectively, in the PDU Session Resource Setup
      // Response Transfer IE of the PDU SESSION RESOURCE SETUP RESPONSE message.
      const auto& ngap_sec_ind = ngap_setup_list[psi].security_ind.value();
      if (security_result_required(ngap_sec_ind)) {
        // Apply security settings according to the decision in the CU-UP.
        if (!e1ap_item.security_result.has_value()) {
          logger.error("Missing security result in E1AP response for PDU session {}", psi);
          return false;
        }
        auto& sec_res     = e1ap_item.security_result.value();
        integrity_enabled = sec_res.integrity_protection_result == integrity_protection_result_t::performed;
        ciphering_enabled = sec_res.confidentiality_protection_result == confidentiality_protection_result_t::performed;
        // Add result to NGAP response
        transfer.security_result = sec_res;
      } else {
        // Apply security settings that were requested via NGAP and do not require an explicit reponse.
        integrity_enabled = ngap_sec_ind.integrity_protection_ind == integrity_protection_indication_t::required;
        ciphering_enabled =
            ngap_sec_ind.confidentiality_protection_ind == confidentiality_protection_indication_t::required;
      }
    } else {
      // Security settings were not signaled via NGAP, we have used the defaults of CU-CP
      const auto sec_ind = default_security_indication;
      if (security_result_required(sec_ind)) {
        // Apply security settings according to the decision in the CU-UP.
        if (!e1ap_item.security_result.has_value()) {
          logger.error("Missing security result in E1AP response for PDU session {}", psi);
          return false;
        }
        const auto& sec_res = e1ap_item.security_result.value();
        integrity_enabled   = sec_res.integrity_protection_result == integrity_protection_result_t::performed;
        ciphering_enabled = sec_res.confidentiality_protection_result == confidentiality_protection_result_t::performed;
        // No result in NGAP response needed here
      } else {
        // Apply default security settings that do not require an explicit response.
        integrity_enabled = sec_ind.integrity_protection_ind == integrity_protection_indication_t::required;
        ciphering_enabled = sec_ind.confidentiality_protection_ind == confidentiality_protection_indication_t::required;
      }
    }

    auto& next_cfg_pdu_session = next_config.pdu_sessions_to_setup_list.at(psi);

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      const auto& drb_id = e1ap_drb_item.drb_id;
      if (next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.find(drb_id) ==
          next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.end()) {
        logger.error("DRB id {} not part of next configuration", drb_id);
        return false;
      }

      // Update security settings of each DRB
      next_cfg_pdu_session.drb_to_add.find(drb_id)->second.pdcp_cfg.integrity_protection_required = integrity_enabled;
      next_cfg_pdu_session.drb_to_add.find(drb_id)->second.pdcp_cfg.ciphering_required            = ciphering_enabled;

      // Prepare DRB item for DU.
      f1ap_drbs_to_be_setup_mod_item drb_setup_mod_item;
      if (!fill_f1ap_drb_setup_mod_item(drb_setup_mod_item,
                                        &transfer.dlqos_flow_per_tnl_info.associated_qos_flow_list,
                                        item.pdu_session_id,
                                        drb_id,
                                        next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.at(drb_id),
                                        e1ap_drb_item,
                                        ngap_setup_list[item.pdu_session_id].qos_flow_setup_request_items,
                                        logger)) {
        logger.error("Couldn't populate DRB setup/mod item {}", e1ap_drb_item.drb_id);
        return false;
      }
      drb_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
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

bool srsran::srs_cu_cp::update_setup_list(
    slotted_id_vector<srb_id_t, f1ap_srbs_to_be_setup_mod_item>&                 srb_setup_mod_list,
    slotted_id_vector<drb_id_t, f1ap_drbs_to_be_setup_mod_item>&                 drb_setup_mod_list,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& ngap_setup_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                pdu_session_resource_setup_list,
    up_config_update&           next_config,
    up_resource_manager&        rrc_ue_up_resource_manager,
    const srslog::basic_logger& logger)
{
  // Set up SRB1 and SRB2 (this is for inter CU handover, so no SRBs are setup yet)
  // TODO: Do we need to setup SRB0 here as well?
  for (unsigned srb_id = 1; srb_id < 3; ++srb_id) {
    f1ap_srbs_to_be_setup_mod_item srb_item;
    srb_item.srb_id = int_to_srb_id(srb_id);
    srb_setup_mod_list.emplace(srb_item.srb_id, srb_item);
  }

  for (const auto& e1ap_item : pdu_session_resource_setup_list) {
    const auto& psi = e1ap_item.pdu_session_id;

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

    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      const auto& drb_id = e1ap_drb_item.drb_id;
      if (next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.find(drb_id) ==
          next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.end()) {
        logger.error("DRB id {} not part of next configuration", drb_id);
        return false;
      }

      // Prepare DRB item for DU.
      f1ap_drbs_to_be_setup_mod_item drb_setup_mod_item;
      if (!fill_f1ap_drb_setup_mod_item(drb_setup_mod_item,
                                        {},
                                        e1ap_item.pdu_session_id,
                                        drb_id,
                                        next_config.pdu_sessions_to_setup_list.at(psi).drb_to_add.at(drb_id),
                                        e1ap_drb_item,
                                        ngap_setup_list[e1ap_item.pdu_session_id].qos_flow_setup_request_items,
                                        logger)) {
        logger.error("Couldn't populate DRB setup/mod item {}", e1ap_drb_item.drb_id);
        return false;
      }
      drb_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
    }

    // Fail on any DRB that fails to be setup
    if (!e1ap_item.drb_failed_list_ng_ran.empty()) {
      logger.error("Non-empty DRB failed list not supported");
      return false;
    }
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
    for (const auto& flow : drb_to_setup.second.qos_flows) {
      srsran_assert(qos_flow_list.contains(flow.first), "Original setup request doesn't contain for {}", flow.first);
      // Lookup the QoS characteristics from the original request.
      const auto&                  qos_flow_params = qos_flow_list[flow.first];
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
    failed_item.pdu_session_id              = e1ap_item.pdu_session_id;
    failed_item.unsuccessful_transfer.cause = e1ap_item.cause;
    ngap_failed_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

bool srsran::srs_cu_cp::update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    f1ap_ue_context_modification_request&                                                 ue_context_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>&
                                e1ap_pdu_session_resource_modify_list,
    up_config_update&           next_config,
    const srslog::basic_logger& logger)
{
  for (const auto& e1ap_item : e1ap_pdu_session_resource_modify_list) {
    const auto& psi = e1ap_item.pdu_session_id;
    // Sanity check - make sure this session ID is present in the original modify message.
    if (ngap_modify_list.contains(psi) == false) {
      logger.error("PDU Session Resource setup request doesn't include setup for PDU session {}", psi);
      return false;
    }
    // Also check if PDU session is included in expected next configuration.
    if (next_config.pdu_sessions_to_modify_list.find(psi) == next_config.pdu_sessions_to_modify_list.end()) {
      logger.error("Didn't expect modification for PDU session {}", psi);
      return false;
    }

    if (ngap_response_list.contains(psi)) {
      // Load existing response item from previous call.
      logger.debug("Amend to existing NGAP response item for PDU session ID {}", psi);
    } else {
      // Add empty new item;
      cu_cp_pdu_session_resource_modify_response_item new_item;
      new_item.pdu_session_id = psi;
      ngap_response_list.emplace(new_item.pdu_session_id, new_item);
      logger.debug("Insert new NGAP response item for PDU session ID {}", psi);
    }

    // Start/continue filling response item.
    cu_cp_pdu_session_resource_modify_response_item& ngap_item = ngap_response_list[psi];
    for (const auto& e1ap_drb_item : e1ap_item.drb_setup_list_ng_ran) {
      const auto& drb_id = e1ap_drb_item.drb_id;
      if (next_config.pdu_sessions_to_modify_list.at(psi).drb_to_add.find(drb_id) ==
          next_config.pdu_sessions_to_modify_list.at(psi).drb_to_add.end()) {
        logger.error("DRB id {} not part of next configuration", drb_id);
        return false;
      }

      const auto& request_transfer = ngap_modify_list[psi].transfer;

      //  Prepare DRB creation at DU.
      f1ap_drbs_to_be_setup_mod_item drb_setup_mod_item;
      if (!fill_f1ap_drb_setup_mod_item(drb_setup_mod_item,
                                        nullptr,
                                        psi,
                                        drb_id,
                                        next_config.pdu_sessions_to_modify_list.at(psi).drb_to_add.at(drb_id),
                                        e1ap_drb_item,
                                        request_transfer.qos_flow_add_or_modify_request_list,
                                        logger)) {
        logger.error("Couldn't populate DRB setup/mod item {}", e1ap_drb_item.drb_id);
        return false;
      }

      // Note: this extra handling for the Modification could be optimized
      for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
        // Fill added flows in NGAP response transfer.
        if (!ngap_item.transfer.qos_flow_add_or_modify_response_list.has_value()) {
          // Add list if it's not present yet.
          ngap_item.transfer.qos_flow_add_or_modify_response_list.emplace();
        }

        qos_flow_add_or_mod_response_item qos_flow;
        qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
        ngap_item.transfer.qos_flow_add_or_modify_response_list.value().emplace(qos_flow.qos_flow_id, qos_flow);
      }

      // Finally add DRB to setup to UE context modification.
      ue_context_mod_request.drbs_to_be_setup_mod_list.emplace(e1ap_drb_item.drb_id, drb_setup_mod_item);
    }

    // Add DRB to be removed to UE context modifcation.
    for (const auto& drb_id : next_config.pdu_sessions_to_modify_list.at(psi).drb_to_remove) {
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

void srsran::srs_cu_cp::fill_e1ap_bearer_context_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& e1ap_list,
    const slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>&              drb_setup_items,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>&          pdu_sessions_update_list)
{
  /// Iterate over all PDU sessions to be updated and match the containing DRBs.
  for (const auto& pdu_session : pdu_sessions_update_list) {
    // The modifications are only for this PDU session.
    e1ap_pdu_session_res_to_modify_item e1ap_mod_item;
    e1ap_mod_item.pdu_session_id = pdu_session.second.id;

    for (const auto& drb_item : drb_setup_items) {
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

void srsran::srs_cu_cp::fill_e1ap_pdu_session_res_to_setup_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_setup_item>&     pdu_session_res_to_setup_list,
    const srslog::basic_logger&                                                  logger,
    const up_config_update&                                                      next_config,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items,
    const ue_configuration&                                                      ue_cfg,
    const security_indication_t&                                                 default_security_indication)
{
  for (const auto& setup_item : next_config.pdu_sessions_to_setup_list) {
    const auto& session = setup_item.second;
    srsran_assert(
        setup_items.contains(session.id), "Setup request doesn't contain config for PDU session id={}", session.id);
    // Obtain PDU session config from original setup request.
    const auto&                        pdu_session_cfg = setup_items[session.id];
    e1ap_pdu_session_res_to_setup_item e1ap_pdu_session_item;

    e1ap_pdu_session_item.pdu_session_id    = pdu_session_cfg.pdu_session_id;
    e1ap_pdu_session_item.pdu_session_type  = pdu_session_cfg.pdu_session_type;
    e1ap_pdu_session_item.snssai            = pdu_session_cfg.s_nssai;
    e1ap_pdu_session_item.ng_ul_up_tnl_info = pdu_session_cfg.ul_ngu_up_tnl_info;

    if (pdu_session_cfg.security_ind.has_value()) {
      // Apply security indication as signaled via NGAP.
      e1ap_pdu_session_item.security_ind = pdu_session_cfg.security_ind.value();
    } else {
      // The security indication was not signaled via NGAP - apply default setting.
      e1ap_pdu_session_item.security_ind = default_security_indication;
    }

    // TODO: set `e1ap_pdu_session_item.pdu_session_inactivity_timer` if configured
    fill_drb_to_setup_list(e1ap_pdu_session_item.drb_to_setup_list_ng_ran,
                           pdu_session_cfg.qos_flow_setup_request_items,
                           session.drb_to_add,
                           logger);

    pdu_session_res_to_setup_list.emplace(e1ap_pdu_session_item.pdu_session_id, e1ap_pdu_session_item);
  }
}

bool srsran::srs_cu_cp::update_modify_list(
    slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item>& ngap_response_list,
    e1ap_bearer_context_modification_request&                                             bearer_ctxt_mod_request,
    const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req>& ngap_modify_list,
    const f1ap_ue_context_modification_response& ue_context_modification_response,
    const up_config_update&                      next_config,
    const srslog::basic_logger&                  logger)
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
                                ue_context_modification_response.drbs_setup_mod_list,
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
    e1ap_bearer_context_modification_request&                    bearer_ctxt_mod_request,
    const slotted_id_vector<drb_id_t, f1ap_drbs_setup_mod_item>& drb_setup_mod_list,
    const up_config_update&                                      next_config,
    const srslog::basic_logger&                                  logger)
{
  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.emplace();

  fill_e1ap_bearer_context_list(e1ap_bearer_context_mod.pdu_session_res_to_modify_list,
                                drb_setup_mod_list,
                                next_config.pdu_sessions_to_setup_list);

  return true;
}
