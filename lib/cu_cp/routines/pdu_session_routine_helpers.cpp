/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/asn1/rrc_nr/cell_group_config.h"

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

  e1ap_qos_item.qos_flow_level_qos_params.qos_desc = request_item.qos_flow_level_qos_params.qos_desc;

  e1ap_qos_item.qos_flow_level_qos_params.ng_ran_alloc_retention =
      request_item.qos_flow_level_qos_params.alloc_retention_prio;

  e1ap_qos_item.qos_flow_level_qos_params.gbr_qos_flow_info = request_item.qos_flow_level_qos_params.gbr_qos_info;
}

bool srsran::srs_cu_cp::verify_and_log_cell_group_config(const byte_buffer&          packed_cell_group_cfg,
                                                         const srslog::basic_logger& logger)
{
  // Unpack DU to CU container.
  asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
  asn1::cbit_ref                 bref_cell({packed_cell_group_cfg.begin(), packed_cell_group_cfg.end()});
  if (cell_group_cfg.unpack(bref_cell) != asn1::SRSASN_SUCCESS) {
    logger.warning("Failed to unpack cellGroupConfig");
    return false;
  }

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    cell_group_cfg.to_json(js);
    logger.debug("Containerized cellGroupCfg: {}", js.to_string());
  }

  return true;
}

bool srsran::srs_cu_cp::fill_rrc_reconfig_args(
    rrc_reconfiguration_procedure_request&                           rrc_reconfig_args,
    const std::vector<f1ap_srb_to_setup>&                            srbs_to_be_setup_mod_list,
    const std::map<pdu_session_id_t, up_pdu_session_context_update>& pdu_sessions,
    const std::vector<drb_id_t>&                                     drb_to_remove,
    const f1ap_du_to_cu_rrc_info&                                    du_to_cu_rrc_info,
    const std::vector<byte_buffer>&                                  nas_pdus,
    const std::optional<rrc_meas_cfg>&                               rrc_meas_cfg,
    bool                                                             reestablish_srbs,
    bool                                                             reestablish_drbs,
    std::optional<uint8_t>                                           ncc,
    byte_buffer                                                      sib1,
    std::optional<security::sec_selected_algos>                      selected_algos,
    const srslog::basic_logger&                                      logger)
{
  rrc_radio_bearer_config radio_bearer_config;
  // If default DRB is being setup, SRB2 needs to be setup as well.
  if (!srbs_to_be_setup_mod_list.empty()) {
    for (const f1ap_srb_to_setup& srb_to_add_mod : srbs_to_be_setup_mod_list) {
      rrc_srb_to_add_mod srb = {};
      srb.srb_id             = srb_to_add_mod.srb_id;
      if (reestablish_srbs) {
        srb.reestablish_pdcp_present = true;
      }
      radio_bearer_config.srb_to_add_mod_list.emplace(srb_to_add_mod.srb_id, srb);
    }
  }

  // Set masterCellGroupConfig as received by DU.
  rrc_recfg_v1530_ies rrc_recfg_v1530_ies;

  // Verify DU container content.
  if (!du_to_cu_rrc_info.cell_group_cfg.empty()) {
    if (!verify_and_log_cell_group_config(du_to_cu_rrc_info.cell_group_cfg, logger)) {
      logger.warning("Failed to verify cellGroupConfig");
      return false;
    }
    // Set masterCellGroupConfig as received by DU.
    rrc_recfg_v1530_ies.master_cell_group = du_to_cu_rrc_info.cell_group_cfg.copy();
  }

  for (const auto& pdu_session_to_add_mod : pdu_sessions) {
    // Fill radio bearer config.
    for (const auto& drb_to_add : pdu_session_to_add_mod.second.drb_to_add) {
      rrc_drb_to_add_mod drb_to_add_mod;
      drb_to_add_mod.drb_id = drb_to_add.first;
      if (reestablish_drbs) {
        drb_to_add_mod.reestablish_pdcp_present = true;
      } else {
        drb_to_add_mod.pdcp_cfg = drb_to_add.second.pdcp_cfg;

        // Fill CN association and SDAP config.
        rrc_cn_assoc cn_assoc;
        cn_assoc.sdap_cfg       = drb_to_add.second.sdap_cfg;
        drb_to_add_mod.cn_assoc = cn_assoc;
      }

      radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_add.first, drb_to_add_mod);
    }

    for (const auto& drb_to_modify : pdu_session_to_add_mod.second.drb_to_modify) {
      rrc_drb_to_add_mod drb_to_add_mod;
      drb_to_add_mod.drb_id = drb_to_modify.first;
      if (reestablish_drbs) {
        drb_to_add_mod.reestablish_pdcp_present = true;
      } else {
        drb_to_add_mod.pdcp_cfg = drb_to_modify.second.pdcp_cfg;

        // Fill CN association and SDAP config.
        rrc_cn_assoc cn_assoc;
        cn_assoc.sdap_cfg       = drb_to_modify.second.sdap_cfg;
        drb_to_add_mod.cn_assoc = cn_assoc;
      }

      radio_bearer_config.drb_to_add_mod_list.emplace(drb_to_modify.first, drb_to_add_mod);
    }

    // Remove DRB from a PDU session (PDU session itself still exists with out DRBs).
    for (const auto& drb_id : pdu_session_to_add_mod.second.drb_to_remove) {
      radio_bearer_config.drb_to_release_list.push_back(drb_id);
    }
  }

  // Remove DRB (if not already) that are not associated with any PDU session anymore.
  for (const auto& drb_id : drb_to_remove) {
    if (std::any_of(radio_bearer_config.drb_to_release_list.begin(),
                    radio_bearer_config.drb_to_release_list.end(),
                    [drb_id](const auto& item) { return item == drb_id; })) {
      // The DRB is already set to be removed.
      continue;
    }

    radio_bearer_config.drb_to_release_list.push_back(drb_id);
  }

  // If selected security algos, fill securityConfig
  if (selected_algos) {
    radio_bearer_config.security_cfg.emplace();
    radio_bearer_config.security_cfg->security_algorithm_cfg.emplace();
    radio_bearer_config.security_cfg->security_algorithm_cfg->ciphering_algorithm      = selected_algos->cipher_algo;
    radio_bearer_config.security_cfg->security_algorithm_cfg->integrity_prot_algorithm = selected_algos->integ_algo;
  }

  // Append NAS PDUs as received by AMF.
  for (const auto& nas_pdu : nas_pdus) {
    rrc_recfg_v1530_ies.ded_nas_msg_list.push_back(nas_pdu.copy());
  }

  if (ncc.has_value()) {
    rrc_recfg_v1530_ies.master_key_upd.emplace();
    rrc_recfg_v1530_ies.master_key_upd.value().key_set_change_ind      = false;
    rrc_recfg_v1530_ies.master_key_upd.value().next_hop_chaining_count = ncc.value(); // TODO: remove hard-coded value
  }

  rrc_reconfig_args.non_crit_ext = rrc_recfg_v1530_ies;

  if (radio_bearer_config.contains_values()) {
    // Fill radio bearer config.
    rrc_reconfig_args.radio_bearer_cfg = radio_bearer_config;
  }

  rrc_reconfig_args.meas_cfg = rrc_meas_cfg;

  // Fill meas gap config.
  if (!du_to_cu_rrc_info.meas_gap_cfg.empty()) {
    rrc_reconfig_args.meas_gap_cfg = du_to_cu_rrc_info.meas_gap_cfg.copy();
  }

  if (!sib1.empty()) {
    rrc_reconfig_args.non_crit_ext.value().ded_sib1_delivery = std::move(sib1);
  }

  return true;
}

bool srsran::srs_cu_cp::fill_f1ap_drb_setup_mod_item(
    f1ap_drb_to_setup&                                           drb_setup_mod_item, // Request to setup DRB at DU.
    slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow>* response_flow_list,
    pdu_session_id_t                                             psi,
    drb_id_t                                                     drb_id,
    up_drb_context&                                              next_drb_config, // DRB config (info is written back).
    const e1ap_drb_setup_item_ng_ran&                            e1ap_drb_item,   // Response from CU-UP.
    const slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item>&
                                ngap_qos_flow_setup_items, // Initial request from AMF.
    const srslog::basic_logger& logger)
{
  // Catch implementation limitations.
  if (!e1ap_drb_item.flow_failed_list.empty()) {
    logger.warning("Non-empty QoS flow failed list not supported");
    return false;
  }

  // Verify only a single UL transport info item is present.
  if (e1ap_drb_item.ul_up_transport_params.size() != 1) {
    logger.warning("Multiple UL UP transport items not supported");
    return false;
  }

  // Start filling the DU request.
  drb_setup_mod_item.drb_id = drb_id;

  // Fill DRB QoS.
  drb_setup_mod_item.qos_info.drb_qos = next_drb_config.qos_params;

  // Fill S-NSSAI.
  drb_setup_mod_item.qos_info.s_nssai = next_drb_config.s_nssai;

  drb_setup_mod_item.mode        = next_drb_config.rlc_mod;
  drb_setup_mod_item.pdcp_sn_len = next_drb_config.pdcp_cfg.tx.sn_size;

  // Fill UP TNL info.
  for (const auto& ul_up_transport_param : e1ap_drb_item.ul_up_transport_params) {
    drb_setup_mod_item.uluptnl_info_list.push_back(ul_up_transport_param.up_tnl_info);
    // Store UL tunnel information in DRB context (required for mobility).
    next_drb_config.ul_up_tnl_info_to_be_setup_list.push_back(ul_up_transport_param.up_tnl_info);
  }

  // Fill QoS flows.
  for (const auto& e1ap_flow : e1ap_drb_item.flow_setup_list) {
    // Verify the QoS flow ID is present in original setup message.
    if (!ngap_qos_flow_setup_items.contains(e1ap_flow.qos_flow_id)) {
      logger.warning("PduSessionResourceSetupRequest doesn't include setup for {} in {}", e1ap_flow.qos_flow_id, psi);
      return false;
    }

    // Verify flow is present in next config as well.
    if (next_drb_config.qos_flows.find(e1ap_flow.qos_flow_id) == next_drb_config.qos_flows.end()) {
      logger.warning("UP config for {} doesn't include setup for {} in {}", drb_id, e1ap_flow.qos_flow_id, psi);
      return false;
    }

    if (response_flow_list) {
      // Fill flow to NGAP response.
      cu_cp_associated_qos_flow qos_flow;
      qos_flow.qos_flow_id = e1ap_flow.qos_flow_id;
      response_flow_list->emplace(e1ap_flow.qos_flow_id, qos_flow);
    }

    // Retrieve QoS properties from NGAP request.
    const auto& ngap_qos_flow = ngap_qos_flow_setup_items[e1ap_flow.qos_flow_id];

    // Fill flow to F1AP DRB item.
    flow_mapped_to_drb flow_map_item;
    flow_map_item.qos_flow_id               = e1ap_flow.qos_flow_id;
    flow_map_item.qos_flow_level_qos_params = ngap_qos_flow.qos_flow_level_qos_params;
    drb_setup_mod_item.qos_info.flows_mapped_to_drb_list.push_back(flow_map_item);

    // Store flow QoS params in UP config.
    auto& next_config_flow_cfg      = next_drb_config.qos_flows.at(e1ap_flow.qos_flow_id);
    next_config_flow_cfg.qos_params = ngap_qos_flow.qos_flow_level_qos_params;
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
    // TODO: set `e1ap_drb_setup_item.drb_inactivity_timer` if configured.
    e1ap_drb_setup_item.sdap_cfg = drb_to_setup.second.sdap_cfg;
    fill_e1ap_drb_pdcp_config(e1ap_drb_setup_item.pdcp_cfg, drb_to_setup.second.pdcp_cfg);

    e1ap_cell_group_info_item e1ap_cell_group_item;
    e1ap_cell_group_item.cell_group_id = 0; // TODO: Remove hardcoded value
    e1ap_drb_setup_item.cell_group_info.push_back(e1ap_cell_group_item);

    // Only iterate over the QoS flows mapped to this particular DRB.
    for (const auto& flow : drb_to_setup.second.qos_flows) {
      srsran_assert(qos_flow_list.contains(flow.first), "Original setup request doesn't contain {}", flow.first);
      // Lookup the QoS characteristics from the original request.
      const auto&                  qos_flow_params = qos_flow_list[flow.first];
      e1ap_qos_flow_qos_param_item e1ap_qos_item;
      fill_e1ap_qos_flow_param_item(e1ap_qos_item, logger, qos_flow_params);
      e1ap_drb_setup_item.qos_flow_info_to_be_setup.emplace(e1ap_qos_item.qos_flow_id, e1ap_qos_item);
    }

    e1ap_drb_to_setup_list.emplace(e1ap_drb_setup_item.drb_id, e1ap_drb_setup_item);
  }
}

void srsran::srs_cu_cp::fill_drb_to_modify_list(
    slotted_id_vector<drb_id_t, e1ap_drb_to_modify_item_ng_ran>&            e1ap_drb_to_modify_list,
    const slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_add_or_mod_item>& qos_flow_list,
    const std::map<drb_id_t, up_drb_context>&                               drb_to_modify_list,
    const srslog::basic_logger&                                             logger)
{
  for (const auto& drb_to_modify : drb_to_modify_list) {
    e1ap_drb_to_modify_item_ng_ran e1ap_drb_to_modify_item;
    e1ap_drb_to_modify_item.drb_id = drb_to_modify.first;
    // TODO: set `e1ap_drb_to_modify_item.drb_inactivity_timer` if configured.
    e1ap_drb_to_modify_item.sdap_cfg = drb_to_modify.second.sdap_cfg;
    e1ap_drb_to_modify_item.pdcp_cfg.emplace();
    fill_e1ap_drb_pdcp_config(e1ap_drb_to_modify_item.pdcp_cfg.value(), drb_to_modify.second.pdcp_cfg);

    // Only iterate over the QoS flows mapped to this particular DRB.
    for (const auto& flow : drb_to_modify.second.qos_flows) {
      srsran_assert(qos_flow_list.contains(flow.first), "Original setup request doesn't contain {}", flow.first);
      // Lookup the QoS characteristics from the original request.
      const auto&                  qos_flow_params = qos_flow_list[flow.first];
      e1ap_qos_flow_qos_param_item e1ap_qos_item;
      fill_e1ap_qos_flow_param_item(e1ap_qos_item, logger, qos_flow_params);
      e1ap_drb_to_modify_item.flow_map_info.emplace(e1ap_qos_item.qos_flow_id, e1ap_qos_item);
    }

    e1ap_drb_to_modify_list.emplace(e1ap_drb_to_modify_item.drb_id, e1ap_drb_to_modify_item);
  }
}

void srsran::srs_cu_cp::fill_drb_to_remove_list(std::vector<drb_id_t>&       e1ap_drb_to_remove_list,
                                                const std::vector<drb_id_t>& drb_to_remove_list)
{
  for (const auto& drb_to_remove : drb_to_remove_list) {
    e1ap_drb_to_remove_list.push_back(drb_to_remove);
  }
}

void srsran::srs_cu_cp::fill_e1ap_bearer_context_list(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_res_to_modify_item>& e1ap_list,
    const std::vector<f1ap_drb_setupmod>&                                     drb_setup_items,
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
        // DRB belongs to this PDU session.
        e1ap_drb_to_modify_item_ng_ran e1ap_drb_item;
        e1ap_drb_item.drb_id = drb_item.drb_id;

        for (const auto& dl_up_tnl_info : drb_item.dluptnl_info_list) {
          e1ap_up_params_item e1ap_dl_up_param;
          e1ap_dl_up_param.up_tnl_info   = dl_up_tnl_info;
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
    srsran_assert(setup_items.contains(session.id), "Setup request doesn't contain config for {}", session.id);
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

    // TODO: set `e1ap_pdu_session_item.pdu_session_inactivity_timer` if configured.
    fill_drb_to_setup_list(e1ap_pdu_session_item.drb_to_setup_list_ng_ran,
                           pdu_session_cfg.qos_flow_setup_request_items,
                           session.drb_to_add,
                           logger);

    pdu_session_res_to_setup_list.emplace(e1ap_pdu_session_item.pdu_session_id, e1ap_pdu_session_item);
  }
}

bool srsran::srs_cu_cp::update_setup_list_with_ue_ctxt_setup_response(
    e1ap_bearer_context_modification_request& bearer_ctxt_mod_request,
    const std::vector<f1ap_drb_setupmod>&     drb_setup_mod_list,
    const up_config_update&                   next_config,
    const srslog::basic_logger&               logger)
{
  // Start with empty message.
  e1ap_ng_ran_bearer_context_mod_request& e1ap_bearer_context_mod =
      bearer_ctxt_mod_request.ng_ran_bearer_context_mod_request.emplace();

  fill_e1ap_bearer_context_list(e1ap_bearer_context_mod.pdu_session_res_to_modify_list,
                                drb_setup_mod_list,
                                next_config.pdu_sessions_to_setup_list);

  return true;
}
