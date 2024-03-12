/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pdu_session_manager_impl.h"

#include <utility>

#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/e1ap/cu_up/e1ap_config_converters.h"
#include "srsran/gtpu/gtpu_tunnel_ngu_factory.h"
#include "srsran/pdcp/pdcp_factory.h"
#include "srsran/sdap/sdap_factory.h"

using namespace srsran;
using namespace srs_cu_up;

pdu_session_manager_impl::pdu_session_manager_impl(ue_index_t                                       ue_index_,
                                                   std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos_cfg_,
                                                   const security::sec_as_config&                   security_info_,
                                                   network_interface_config&                        net_config_,
                                                   n3_interface_config&                             n3_config_,
                                                   cu_up_ue_logger&                                 logger_,
                                                   unique_timer&                        ue_inactivity_timer_,
                                                   timer_factory                        ue_dl_timer_factory_,
                                                   timer_factory                        ue_ul_timer_factory_,
                                                   timer_factory                        ue_ctrl_timer_factory_,
                                                   f1u_cu_up_gateway&                   f1u_gw_,
                                                   gtpu_teid_pool&                      f1u_teid_allocator_,
                                                   gtpu_tunnel_tx_upper_layer_notifier& gtpu_tx_notifier_,
                                                   gtpu_demux_ctrl&                     gtpu_rx_demux_,
                                                   task_executor&                       ue_dl_exec_,
                                                   task_executor&                       ue_ul_exec_,
                                                   task_executor&                       ue_ctrl_exec_,
                                                   dlt_pcap&                            gtpu_pcap_) :
  ue_index(ue_index_),
  qos_cfg(std::move(qos_cfg_)),
  security_info(security_info_),
  net_config(net_config_),
  n3_config(n3_config_),
  logger(logger_),
  ue_inactivity_timer(ue_inactivity_timer_),
  ue_dl_timer_factory(ue_dl_timer_factory_),
  ue_ul_timer_factory(ue_ul_timer_factory_),
  ue_ctrl_timer_factory(ue_ctrl_timer_factory_),
  gtpu_tx_notifier(gtpu_tx_notifier_),
  f1u_teid_allocator(f1u_teid_allocator_),
  gtpu_rx_demux(gtpu_rx_demux_),
  ue_dl_exec(ue_dl_exec_),
  ue_ul_exec(ue_ul_exec_),
  ue_ctrl_exec(ue_ctrl_exec_),
  gtpu_pcap(gtpu_pcap_),
  f1u_gw(f1u_gw_)
{
  (void)ue_ctrl_exec;
}

drb_setup_result pdu_session_manager_impl::handle_drb_to_setup_item(pdu_session&                         new_session,
                                                                    const e1ap_drb_to_setup_item_ng_ran& drb_to_setup)
{
  // prepare DRB creation result
  drb_setup_result drb_result = {};
  drb_result.success          = false;
  drb_result.cause            = e1ap_cause_radio_network_t::unspecified;
  drb_result.drb_id           = drb_to_setup.drb_id;

  // get DRB from list and create context
  new_session.drbs.emplace(drb_to_setup.drb_id, std::make_unique<drb_context>(drb_to_setup.drb_id));
  auto& new_drb = new_session.drbs.at(drb_to_setup.drb_id);

  // Create QoS flows
  if (drb_to_setup.qos_flow_info_to_be_setup.empty()) {
    return drb_result;
  }
  five_qi_t five_qi =
      drb_to_setup.qos_flow_info_to_be_setup.begin()->qos_flow_level_qos_params.qos_characteristics.get_five_qi();
  if (qos_cfg.find(five_qi) == qos_cfg.end()) {
    drb_result.cause = e1ap_cause_radio_network_t::not_supported_5qi_value;
    return drb_result;
  }

  uint32_t nof_flow_success = 0;
  for (const auto& qos_flow_info : drb_to_setup.qos_flow_info_to_be_setup) {
    // prepare QoS flow creation result
    qos_flow_setup_result flow_result = {};
    flow_result.success               = false;
    flow_result.cause                 = e1ap_cause_radio_network_t::unspecified;
    flow_result.qos_flow_id           = qos_flow_info.qos_flow_id;

    if (!new_session.sdap->is_mapped(qos_flow_info.qos_flow_id) &&
        qos_flow_info.qos_flow_level_qos_params.qos_characteristics.get_five_qi() == five_qi) {
      // create QoS flow context
      const auto& qos_flow                     = qos_flow_info;
      new_drb->qos_flows[qos_flow.qos_flow_id] = std::make_unique<qos_flow_context>(qos_flow);
      auto& new_qos_flow                       = new_drb->qos_flows[qos_flow.qos_flow_id];
      logger.log_debug("Created QoS flow with {} and {}", new_qos_flow->qos_flow_id, new_qos_flow->five_qi);
      sdap_config sdap_cfg = make_sdap_drb_config(drb_to_setup.sdap_cfg);
      new_session.sdap->add_mapping(
          qos_flow.qos_flow_id, drb_to_setup.drb_id, sdap_cfg, new_qos_flow->sdap_to_pdcp_adapter);
      flow_result.success = true;
      nof_flow_success++;
    } else {
      // fail if mapping already exists
      flow_result.success = false;
      flow_result.cause   = new_session.sdap->is_mapped(qos_flow_info.qos_flow_id)
                                ? e1ap_cause_radio_network_t::multiple_qos_flow_id_instances
                                : e1ap_cause_radio_network_t::not_supported_5qi_value;
      logger.log_error("Cannot overwrite existing mapping for {}", qos_flow_info.qos_flow_id);
    }

    // Add QoS flow creation result
    drb_result.qos_flow_results.push_back(flow_result);
  }

  // If no flow could be created, we remove the rest of the dangling DRB again
  if (nof_flow_success == 0) {
    logger.log_error(
        "Failed to create {} for {}: Could not map any QoS flow", drb_to_setup.drb_id, new_session.pdu_session_id);
    new_session.drbs.erase(drb_to_setup.drb_id);
    drb_result.cause   = e1ap_cause_radio_network_t::unspecified;
    drb_result.success = false;
    return drb_result;
  }

  // If 5QI is not configured in CU-UP, we remove the rest of the dangling DRB again
  if (qos_cfg.find(five_qi) == qos_cfg.end()) {
    logger.log_error(
        "Failed to create {} for {}: Could not find 5QI. {}", drb_to_setup.drb_id, new_session.pdu_session_id, five_qi);
    new_session.drbs.erase(drb_to_setup.drb_id);
    drb_result.cause   = e1ap_cause_radio_network_t::not_supported_5qi_value;
    drb_result.success = false;
    return drb_result;
  }

  // Create PDCP entity
  srsran::pdcp_entity_creation_message pdcp_msg = {};
  pdcp_msg.ue_index                             = ue_index;
  pdcp_msg.rb_id                                = drb_to_setup.drb_id;
  pdcp_msg.config                               = make_pdcp_drb_config(drb_to_setup.pdcp_cfg, new_session.security_ind);
  pdcp_msg.config.custom                        = qos_cfg.at(five_qi).pdcp_custom;
  pdcp_msg.tx_lower                             = &new_drb->pdcp_to_f1u_adapter;
  pdcp_msg.tx_upper_cn                          = &new_drb->pdcp_tx_to_e1ap_adapter;
  pdcp_msg.rx_upper_dn                          = &new_drb->pdcp_to_sdap_adapter;
  pdcp_msg.rx_upper_cn                          = &new_drb->pdcp_rx_to_e1ap_adapter;
  pdcp_msg.ue_dl_timer_factory                  = ue_dl_timer_factory;
  pdcp_msg.ue_ul_timer_factory                  = ue_ul_timer_factory;
  pdcp_msg.ue_ctrl_timer_factory                = ue_ctrl_timer_factory;
  new_drb->pdcp                                 = srsran::create_pdcp_entity(pdcp_msg);

  security::sec_128_as_config sec_128 = security::truncate_config(security_info);
  // configure tx security
  auto& pdcp_tx_ctrl = new_drb->pdcp->get_tx_upper_control_interface();
  pdcp_tx_ctrl.configure_security(sec_128);
  pdcp_tx_ctrl.set_integrity_protection(new_session.security_ind.integrity_protection_ind ==
                                                integrity_protection_indication_t::not_needed
                                            ? security::integrity_enabled::off
                                            : security::integrity_enabled::on);
  pdcp_tx_ctrl.set_ciphering(new_session.security_ind.confidentiality_protection_ind ==
                                     confidentiality_protection_indication_t::not_needed
                                 ? security::ciphering_enabled::off
                                 : security::ciphering_enabled::on);
  // configure rx security
  auto& pdcp_rx_ctrl = new_drb->pdcp->get_rx_upper_control_interface();
  pdcp_rx_ctrl.configure_security(sec_128);
  pdcp_rx_ctrl.set_integrity_protection(new_session.security_ind.integrity_protection_ind ==
                                                integrity_protection_indication_t::not_needed
                                            ? security::integrity_enabled::off
                                            : security::integrity_enabled::on);
  pdcp_rx_ctrl.set_ciphering(new_session.security_ind.confidentiality_protection_ind ==
                                     confidentiality_protection_indication_t::not_needed
                                 ? security::ciphering_enabled::off
                                 : security::ciphering_enabled::on);

  // Connect "PDCP-E1AP" adapter to E1AP
  new_drb->pdcp_tx_to_e1ap_adapter.connect_e1ap(); // TODO: pass actual E1AP handler
  new_drb->pdcp_rx_to_e1ap_adapter.connect_e1ap(); // TODO: pass actual E1AP handler

  // Create  F1-U bearer
  expected<gtpu_teid_t> ret = f1u_teid_allocator.request_teid();
  if (not ret.has_value()) {
    logger.log_error("Could not allocate ul_teid");
    return drb_result;
  }
  gtpu_teid_t f1u_ul_teid = ret.value();

  up_transport_layer_info f1u_ul_tunnel_addr(transport_layer_address::create_from_string(net_config.f1u_bind_addr),
                                             f1u_ul_teid);

  new_drb->f1u          = f1u_gw.create_cu_bearer(ue_index,
                                         drb_to_setup.drb_id,
                                         f1u_ul_tunnel_addr,
                                         new_drb->f1u_to_pdcp_adapter,
                                         new_drb->f1u_to_pdcp_adapter,
                                         ue_ul_exec,
                                         ue_dl_timer_factory,
                                         ue_inactivity_timer);
  new_drb->f1u_ul_teid  = f1u_ul_teid;
  drb_result.gtp_tunnel = f1u_ul_tunnel_addr;

  // Connect F1-U's "F1-U->PDCP adapter" directly to PDCP
  new_drb->f1u_to_pdcp_adapter.connect_pdcp(new_drb->pdcp->get_rx_lower_interface(),
                                            new_drb->pdcp->get_tx_lower_interface());
  new_drb->pdcp_to_f1u_adapter.connect_f1u(new_drb->f1u->get_tx_sdu_handler());

  // Connect QoS flows to DRB
  for (auto& new_qos_flow : new_drb->qos_flows) {
    new_qos_flow.second->sdap_to_pdcp_adapter.connect_pdcp(new_drb->pdcp->get_tx_upper_data_interface());
    new_drb->pdcp_to_sdap_adapter.connect_sdap(new_session.sdap->get_sdap_rx_pdu_handler(drb_to_setup.drb_id));
  }

  // Add result
  drb_result.success = true;

  return drb_result;
}

pdu_session_setup_result pdu_session_manager_impl::setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session)
{
  pdu_session_setup_result pdu_session_result = {};
  pdu_session_result.success                  = false;
  pdu_session_result.pdu_session_id           = session.pdu_session_id;
  pdu_session_result.cause                    = e1ap_cause_radio_network_t::unspecified;

  if (pdu_sessions.find(session.pdu_session_id) != pdu_sessions.end()) {
    logger.log_error("PDU Session with {} already exists", session.pdu_session_id);
    return pdu_session_result;
  }

  if (pdu_sessions.size() >= MAX_NUM_PDU_SESSIONS_PER_UE) {
    logger.log_error("PDU Session for {} cannot be created. Max number of PDU sessions reached",
                     session.pdu_session_id);
    return pdu_session_result;
  }

  pdu_sessions.emplace(session.pdu_session_id, std::make_unique<pdu_session>(session, gtpu_rx_demux));
  std::unique_ptr<pdu_session>& new_session    = pdu_sessions.at(session.pdu_session_id);
  const auto&                   ul_tunnel_info = new_session->ul_tunnel_info;

  // Get uplink transport address
  logger.log_debug("PDU session uplink tunnel info: {} teid={} addr={}",
                   session.pdu_session_id,
                   ul_tunnel_info.gtp_teid.value(),
                   ul_tunnel_info.tp_address);

  // Allocate local TEID
  new_session->local_teid = allocate_local_teid(new_session->pdu_session_id);

  pdu_session_result.gtp_tunnel = up_transport_layer_info(
      transport_layer_address::create_from_string(net_config.n3_bind_addr), new_session->local_teid);

  // Create SDAP entity
  sdap_entity_creation_message sdap_msg = {ue_index, session.pdu_session_id, &new_session->sdap_to_gtpu_adapter};
  new_session->sdap                     = create_sdap(sdap_msg);

  // Create GTPU entity
  gtpu_tunnel_ngu_creation_message msg = {};
  msg.ue_index                         = ue_index;
  msg.cfg.tx.peer_teid                 = int_to_gtpu_teid(ul_tunnel_info.gtp_teid.value());
  msg.cfg.tx.peer_addr                 = ul_tunnel_info.tp_address.to_string();
  msg.cfg.tx.peer_port                 = net_config.upf_port;
  msg.cfg.rx.local_teid                = new_session->local_teid;
  msg.cfg.rx.t_reordering              = n3_config.gtpu_reordering_timer;
  msg.rx_lower                         = &new_session->gtpu_to_sdap_adapter;
  msg.tx_upper                         = &gtpu_tx_notifier;
  msg.gtpu_pcap                        = &gtpu_pcap;
  msg.ue_dl_timer_factory              = ue_dl_timer_factory;
  new_session->gtpu                    = create_gtpu_tunnel_ngu(msg);

  // Connect adapters
  new_session->sdap_to_gtpu_adapter.connect_gtpu(*new_session->gtpu->get_tx_lower_layer_interface());
  new_session->gtpu_to_sdap_adapter.connect_sdap(new_session->sdap->get_sdap_tx_sdu_handler());

  // Register tunnel at demux
  if (!gtpu_rx_demux.add_tunnel(
          new_session->local_teid, ue_dl_exec, new_session->gtpu->get_rx_upper_layer_interface())) {
    logger.log_error(
        "PDU Session {} cannot be created. TEID {} already exists", session.pdu_session_id, new_session->local_teid);
    return pdu_session_result;
  }

  // Handle DRB setup
  for (const e1ap_drb_to_setup_item_ng_ran& drb_to_setup : session.drb_to_setup_list_ng_ran) {
    drb_setup_result drb_result = handle_drb_to_setup_item(*new_session, drb_to_setup);
    pdu_session_result.drb_setup_results.push_back(drb_result);
  }

  // Ref: TS 38.463 Sec. 8.3.1.2:
  // For each PDU session for which the Security Indication IE is included in the PDU Session Resource To Setup List
  // IE of the BEARER CONTEXT SETUP REQUEST message, and the Integrity Protection Indication IE or Confidentiality
  // Protection Indication IE is set to "preferred", then the gNB-CU-UP should, if supported, perform user plane
  // integrity protection or ciphering, respectively, for the concerned PDU session and shall notify whether it
  // performed the user plane integrity protection or ciphering by including the Integrity Protection Result IE or
  // Confidentiality Protection Result IE, respectively, in the PDU Session Resource Setup List IE of the BEARER
  // CONTEXT SETUP RESPONSE message.
  if (security_result_required(session.security_ind)) {
    pdu_session_result.security_result = security_result_t{};
    auto& sec_res                      = pdu_session_result.security_result.value();
    sec_res.integrity_protection_result =
        session.security_ind.integrity_protection_ind == integrity_protection_indication_t::not_needed
            ? integrity_protection_result_t::not_performed
            : integrity_protection_result_t::performed;
    sec_res.confidentiality_protection_result =
        session.security_ind.confidentiality_protection_ind == confidentiality_protection_indication_t::not_needed
            ? confidentiality_protection_result_t::not_performed
            : confidentiality_protection_result_t::performed;
  }
  pdu_session_result.success = true;
  return pdu_session_result;
}

pdu_session_modification_result
pdu_session_manager_impl::modify_pdu_session(const e1ap_pdu_session_res_to_modify_item& session,
                                             bool                                       new_tnl_info_required)
{
  pdu_session_modification_result pdu_session_result;
  pdu_session_result.success        = false;
  pdu_session_result.pdu_session_id = session.pdu_session_id;
  pdu_session_result.cause          = e1ap_cause_radio_network_t::unspecified;

  if (pdu_sessions.find(session.pdu_session_id) == pdu_sessions.end()) {
    logger.log_error("PDU Session {} doesn't exists", session.pdu_session_id);
    return pdu_session_result;
  }

  auto& pdu_session = pdu_sessions.at(session.pdu_session_id);

  // > DRB To Setup List
  for (const auto& drb_to_setup : session.drb_to_setup_list_ng_ran) {
    drb_setup_result drb_result = handle_drb_to_setup_item(*pdu_session, drb_to_setup);
    pdu_session_result.drb_setup_results.push_back(drb_result);
  }

  // > DRB To Modify List
  for (const auto& drb_to_mod : session.drb_to_modify_list_ng_ran) {
    // prepare DRB modification result
    drb_setup_result drb_result = {};
    drb_result.success          = false;
    drb_result.cause            = e1ap_cause_radio_network_t::unspecified;
    drb_result.drb_id           = drb_to_mod.drb_id;

    // find DRB in PDU session
    auto drb_iter = pdu_session->drbs.find(drb_to_mod.drb_id);
    if (drb_iter == pdu_session->drbs.end()) {
      logger.log_warning("Cannot modify {} not found in {}", drb_to_mod.drb_id, session.pdu_session_id);
      pdu_session_result.drb_setup_results.push_back(drb_result);
      continue;
    }
    srsran_assert(drb_to_mod.drb_id == drb_iter->second->drb_id,
                  "Query for {} in {} provided {}",
                  drb_to_mod.drb_id,
                  session.pdu_session_id,
                  drb_iter->second->drb_id);

    auto& drb = drb_iter->second;
    if (new_tnl_info_required) {
      if (not f1u_teid_allocator.release_teid(drb->f1u_ul_teid)) {
        logger.log_error("Could not free old ul_teid={}", drb->f1u_ul_teid);
      }

      // Allocate new UL TEID for DRB
      expected<gtpu_teid_t> ret = f1u_teid_allocator.request_teid();
      if (not ret.has_value()) {
        logger.log_error("Could not allocate ul_teid");
        continue;
      }
      drb->f1u_ul_teid = ret.value();

      // Create UL UP TNL address.
      up_transport_layer_info f1u_ul_tunnel_addr(transport_layer_address::create_from_string(net_config.f1u_bind_addr),
                                                 drb->f1u_ul_teid);

      // create new F1-U and connect it. This will automatically disconnect the old F1-U.
      drb->f1u = f1u_gw.create_cu_bearer(ue_index,
                                         drb->drb_id,
                                         f1u_ul_tunnel_addr,
                                         drb->f1u_to_pdcp_adapter,
                                         drb->f1u_to_pdcp_adapter,
                                         ue_dl_exec,
                                         ue_dl_timer_factory,
                                         ue_inactivity_timer);
      drb_iter->second->pdcp_to_f1u_adapter.disconnect_f1u();

      drb_result.gtp_tunnel = f1u_ul_tunnel_addr;
    }

    // F1-U apply modification
    if (!drb_to_mod.dl_up_params.empty()) {
      f1u_gw.attach_dl_teid(
          up_transport_layer_info(transport_layer_address::create_from_string(net_config.f1u_bind_addr),
                                  drb_iter->second->f1u_ul_teid),
          drb_to_mod.dl_up_params[0].up_tnl_info);

      drb_iter->second->pdcp_to_f1u_adapter.connect_f1u(drb_iter->second->f1u->get_tx_sdu_handler());
    }

    // Apply re-establishment at PDCP
    if (drb_to_mod.pdcp_cfg.has_value() && drb_to_mod.pdcp_cfg->pdcp_reest.has_value()) {
      security::sec_128_as_config sec_128 = security::truncate_config(security_info);

      // reestablish tx and configure tx security
      auto& pdcp_tx_ctrl = drb->pdcp->get_tx_upper_control_interface();
      pdcp_tx_ctrl.reestablish(sec_128);
      pdcp_tx_ctrl.set_integrity_protection(pdu_session->security_ind.integrity_protection_ind ==
                                                    integrity_protection_indication_t::not_needed
                                                ? security::integrity_enabled::off
                                                : security::integrity_enabled::on);
      pdcp_tx_ctrl.set_ciphering(pdu_session->security_ind.confidentiality_protection_ind ==
                                         confidentiality_protection_indication_t::not_needed
                                     ? security::ciphering_enabled::off
                                     : security::ciphering_enabled::on);
      // reestablish rx and configure rx security
      auto& pdcp_rx_ctrl = drb->pdcp->get_rx_upper_control_interface();
      pdcp_rx_ctrl.reestablish(sec_128);
      pdcp_rx_ctrl.set_integrity_protection(pdu_session->security_ind.integrity_protection_ind ==
                                                    integrity_protection_indication_t::not_needed
                                                ? security::integrity_enabled::off
                                                : security::integrity_enabled::on);
      pdcp_rx_ctrl.set_ciphering(pdu_session->security_ind.confidentiality_protection_ind ==
                                         confidentiality_protection_indication_t::not_needed
                                     ? security::ciphering_enabled::off
                                     : security::ciphering_enabled::on);
    }

    logger.log_info("Modified {}. {} f1u_teid={}", drb_to_mod.drb_id, session.pdu_session_id, drb->f1u_ul_teid);

    // Apply QoS flows
    for (auto& qos_flow_info : drb_to_mod.flow_map_info) {
      // TODO
      logger.log_warning("Unsupported modification of QoS flow for {}", qos_flow_info.qos_flow_id);
    }

    // Add result
    drb_result.success = true;
    pdu_session_result.drb_modification_results.push_back(drb_result);
  }

  // > DRB To Remove List
  for (const auto& drb_to_rem : session.drb_to_rem_list_ng_ran) {
    // unmap all QFI that use this DRB
    pdu_session->sdap->remove_mapping(drb_to_rem);

    // find DRB in PDU session
    auto drb_iter = pdu_session->drbs.find(drb_to_rem);
    if (drb_iter == pdu_session->drbs.end()) {
      logger.log_warning("Cannot remove {}. DRB not found in {}", drb_to_rem, session.pdu_session_id);
      continue;
    }
    srsran_assert(drb_to_rem == drb_iter->second->drb_id,
                  "Query for {} in {} provided different drb_id={}",
                  drb_to_rem,
                  session.pdu_session_id,
                  drb_iter->second->drb_id);

    // remove DRB (this will automatically disconnect from F1-U gateway)
    if (not f1u_teid_allocator.release_teid(drb_iter->second->f1u_ul_teid)) {
      logger.log_error("{} drb_id={} could not free ul_teid={}", session.pdu_session_id, drb_to_rem);
    }
    pdu_session->drbs.erase(drb_iter);

    logger.log_info("Removed {} for {}", drb_to_rem, session.pdu_session_id);
  }

  pdu_session_result.success = true;
  return pdu_session_result;
}

void pdu_session_manager_impl::remove_pdu_session(pdu_session_id_t pdu_session_id)
{
  if (pdu_sessions.find(pdu_session_id) == pdu_sessions.end()) {
    logger.log_error("PDU session {} not found", pdu_session_id);
    return;
  }

  // Disconnect all UL tunnels for this PDU session.
  auto& pdu_session = pdu_sessions.at(pdu_session_id);
  for (const auto& drb : pdu_session->drbs) {
    logger.log_debug("Disconnecting CU bearer with UL-TEID={}", drb.second->f1u_ul_teid);
    f1u_gw.disconnect_cu_bearer(up_transport_layer_info(
        transport_layer_address::create_from_string(net_config.f1u_bind_addr), drb.second->f1u_ul_teid));
    if (f1u_teid_allocator.release_teid(drb.second->f1u_ul_teid)) {
      logger.log_error(
          "{} could not remove ul_teid at session termination. ul_teid={}", pdu_session_id, drb.second->f1u_ul_teid);
    }
  }

  pdu_sessions.erase(pdu_session_id);
  logger.log_info("Removing PDU session with {}", pdu_session_id);
}

size_t pdu_session_manager_impl::get_nof_pdu_sessions()
{
  return pdu_sessions.size();
}

gtpu_teid_t pdu_session_manager_impl::allocate_local_teid(pdu_session_id_t pdu_session_id)
{
  // Local TEID is the concatenation of the unique UE index and the PDU session ID
  uint32_t local_teid = ue_index;
  local_teid <<= 8U;
  local_teid |= pdu_session_id_to_uint(pdu_session_id);
  return gtpu_teid_t{local_teid};
}
