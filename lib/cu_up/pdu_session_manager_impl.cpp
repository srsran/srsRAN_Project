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

#include "pdu_session_manager_impl.h"

#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/e1ap/cu_up/e1ap_config_converters.h"
#include "srsran/gtpu/gtpu_tunnel_ngu_factory.h"
#include "srsran/pdcp/pdcp_factory.h"
#include "srsran/sdap/sdap_factory.h"

using namespace srsran;
using namespace srs_cu_up;

pdu_session_manager_impl::pdu_session_manager_impl(ue_index_t                           ue_index_,
                                                   network_interface_config&            net_config_,
                                                   srslog::basic_logger&                logger_,
                                                   unique_timer&                        ue_inactivity_timer_,
                                                   timer_factory                        timers_,
                                                   f1u_cu_up_gateway&                   f1u_gw_,
                                                   gtpu_teid_pool&                      f1u_teid_allocator_,
                                                   gtpu_tunnel_tx_upper_layer_notifier& gtpu_tx_notifier_,
                                                   gtpu_demux_ctrl&                     gtpu_rx_demux_) :
  ue_index(ue_index_),
  net_config(net_config_),
  logger(logger_),
  ue_inactivity_timer(ue_inactivity_timer_),
  timers(timers_),
  gtpu_tx_notifier(gtpu_tx_notifier_),
  f1u_teid_allocator(f1u_teid_allocator_),
  gtpu_rx_demux(gtpu_rx_demux_),
  f1u_gw(f1u_gw_)
{
}

drb_setup_result pdu_session_manager_impl::handle_drb_to_setup_item(pdu_session&                         new_session,
                                                                    const e1ap_drb_to_setup_item_ng_ran& drb_to_setup)
{
  // prepare DRB creation result
  drb_setup_result drb_result = {};
  drb_result.success          = false;
  drb_result.cause            = cause_t::radio_network;
  drb_result.drb_id           = drb_to_setup.drb_id;

  // get DRB from list and create context
  new_session.drbs.emplace(drb_to_setup.drb_id, std::make_unique<drb_context>(drb_to_setup.drb_id));
  auto& new_drb = new_session.drbs.at(drb_to_setup.drb_id);

  // Create PDCP entity
  srsran::pdcp_entity_creation_message pdcp_msg = {};
  pdcp_msg.ue_index                             = ue_index;
  pdcp_msg.rb_id                                = drb_to_setup.drb_id;
  pdcp_msg.config                               = make_pdcp_drb_config(drb_to_setup.pdcp_cfg);
  pdcp_msg.tx_lower                             = &new_drb->pdcp_to_f1u_adapter;
  pdcp_msg.tx_upper_cn                          = &new_drb->pdcp_tx_to_e1ap_adapter;
  pdcp_msg.rx_upper_dn                          = &new_drb->pdcp_to_sdap_adapter;
  pdcp_msg.rx_upper_cn                          = &new_drb->pdcp_rx_to_e1ap_adapter;
  pdcp_msg.timers                               = timers;
  new_drb->pdcp                                 = srsran::create_pdcp_entity(pdcp_msg);

  // Connect "PDCP-E1AP" adapter to E1AP
  new_drb->pdcp_tx_to_e1ap_adapter.connect_e1ap(); // TODO: pass actual E1AP handler
  new_drb->pdcp_rx_to_e1ap_adapter.connect_e1ap(); // TODO: pass actual E1AP handler

  // Create  F1-U bearer
  expected<gtpu_teid_t> ret = f1u_teid_allocator.request_teid();
  if (not ret.has_value()) {
    logger.error("ue={} could not allocate ul_teid", ue_index);
    return drb_result;
  }
  gtpu_teid_t f1u_ul_teid = ret.value();

  up_transport_layer_info f1u_ul_tunnel_addr;
  f1u_ul_tunnel_addr.tp_address.from_string(net_config.f1u_bind_addr);
  f1u_ul_tunnel_addr.gtp_teid = f1u_ul_teid;
  new_drb->f1u                = f1u_gw.create_cu_bearer(
      ue_index, f1u_ul_tunnel_addr, new_drb->f1u_to_pdcp_adapter, new_drb->f1u_to_pdcp_adapter, timers);
  new_drb->f1u_ul_teid  = f1u_ul_teid;
  drb_result.gtp_tunnel = f1u_ul_tunnel_addr;

  // Connect F1-U's "F1-U->PDCP adapter" directly to PDCP
  new_drb->f1u_to_pdcp_adapter.connect_pdcp(new_drb->pdcp->get_rx_lower_interface(),
                                            new_drb->pdcp->get_tx_lower_interface());
  new_drb->pdcp_to_f1u_adapter.connect_f1u(new_drb->f1u->get_tx_sdu_handler());

  // Create QoS flows
  for (auto& qos_flow_info : drb_to_setup.qos_flow_info_to_be_setup) {
    // prepare QoS flow creation result
    qos_flow_setup_result flow_result = {};
    flow_result.success               = false;
    flow_result.cause                 = cause_t::radio_network;
    flow_result.qos_flow_id           = qos_flow_info.qos_flow_id;

    // create QoS flow context
    auto& qos_flow                           = qos_flow_info;
    new_drb->qos_flows[qos_flow.qos_flow_id] = std::make_unique<qos_flow_context>(qos_flow);
    auto& new_qos_flow                       = new_drb->qos_flows[qos_flow.qos_flow_id];
    logger.debug(
        "Created QoS flow with qos_flow_id={} and five_qi={}", new_qos_flow->qos_flow_id, new_qos_flow->five_qi);

    sdap_config sdap_cfg = make_sdap_drb_config(drb_to_setup.sdap_cfg);
    new_session.sdap->add_mapping(
        qos_flow.qos_flow_id, drb_to_setup.drb_id, sdap_cfg, new_qos_flow->sdap_to_pdcp_adapter);
    new_qos_flow->sdap_to_pdcp_adapter.connect_pdcp(new_drb->pdcp->get_tx_upper_data_interface());
    new_drb->pdcp_to_sdap_adapter.connect_sdap(new_session.sdap->get_sdap_rx_pdu_handler(drb_to_setup.drb_id));

    // Add QoS flow creation result
    flow_result.success = true;
    drb_result.qos_flow_results.push_back(flow_result);
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
  pdu_session_result.cause                    = cause_t::radio_network;

  if (pdu_sessions.find(session.pdu_session_id) != pdu_sessions.end()) {
    logger.error("PDU Session {} already exists", session.pdu_session_id);
    return pdu_session_result;
  }

  if (pdu_sessions.size() >= MAX_NUM_PDU_SESSIONS_PER_UE) {
    logger.error("PDU Session {} cannot be created, max number of PDU sessions reached", session.pdu_session_id);
    return pdu_session_result;
  }

  pdu_sessions.emplace(session.pdu_session_id, std::make_unique<pdu_session>(session, gtpu_rx_demux));
  std::unique_ptr<pdu_session>& new_session    = pdu_sessions.at(session.pdu_session_id);
  const auto&                   ul_tunnel_info = new_session->ul_tunnel_info;

  // Get uplink transport address
  logger.debug("PDU session {} uplink tunnel info: TEID={}, address={}",
               session.pdu_session_id,
               ul_tunnel_info.gtp_teid.value(),
               ul_tunnel_info.tp_address);

  // Allocate local TEID
  new_session->local_teid = allocate_local_teid(new_session->pdu_session_id);

  up_transport_layer_info n3_dl_tunnel_addr;
  n3_dl_tunnel_addr.tp_address.from_string(net_config.n3_bind_addr);
  n3_dl_tunnel_addr.gtp_teid    = new_session->local_teid;
  pdu_session_result.gtp_tunnel = n3_dl_tunnel_addr;

  // Create SDAP entity
  sdap_entity_creation_message sdap_msg = {
      ue_index, session.pdu_session_id, ue_inactivity_timer, &new_session->sdap_to_gtpu_adapter};
  new_session->sdap = create_sdap(sdap_msg);

  // Create GTPU entity
  gtpu_tunnel_ngu_creation_message msg = {};
  msg.ue_index                         = ue_index;
  msg.cfg.tx.peer_teid                 = int_to_gtpu_teid(ul_tunnel_info.gtp_teid.value());
  msg.cfg.tx.peer_addr                 = ul_tunnel_info.tp_address.to_string();
  msg.cfg.tx.peer_port                 = net_config.upf_port;
  msg.cfg.rx.local_teid                = new_session->local_teid;
  msg.rx_lower                         = &new_session->gtpu_to_sdap_adapter;
  msg.tx_upper                         = &gtpu_tx_notifier;
  new_session->gtpu                    = create_gtpu_tunnel_ngu(msg);

  // Connect adapters
  new_session->sdap_to_gtpu_adapter.connect_gtpu(*new_session->gtpu->get_tx_lower_layer_interface());
  new_session->gtpu_to_sdap_adapter.connect_sdap(new_session->sdap->get_sdap_tx_sdu_handler());

  // Register tunnel at demux
  if (!gtpu_rx_demux.add_tunnel(new_session->local_teid, new_session->gtpu->get_rx_upper_layer_interface())) {
    logger.error(
        "PDU Session {} cannot be created. TEID {} already exists", session.pdu_session_id, new_session->local_teid);
    return pdu_session_result;
  }

  // Handle DRB setup
  for (const e1ap_drb_to_setup_item_ng_ran& drb_to_setup : session.drb_to_setup_list_ng_ran) {
    drb_setup_result drb_result = handle_drb_to_setup_item(*new_session, drb_to_setup);
    pdu_session_result.drb_setup_results.push_back(drb_result);
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
  pdu_session_result.cause          = cause_t::misc;

  if (pdu_sessions.find(session.pdu_session_id) == pdu_sessions.end()) {
    logger.error("PDU Session {} doesn't exists", session.pdu_session_id);
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
    drb_result.cause            = cause_t::radio_network;
    drb_result.drb_id           = drb_to_mod.drb_id;

    // find DRB in PDU session
    auto drb_iter = pdu_session->drbs.find(drb_to_mod.drb_id);
    if (drb_iter == pdu_session->drbs.end()) {
      logger.warning(
          "Cannot modify DRB: drb_id={} not found in pdu_session_id={}", drb_to_mod.drb_id, session.pdu_session_id);
      pdu_session_result.drb_setup_results.push_back(drb_result);
      continue;
    }
    srsran_assert(drb_to_mod.drb_id == drb_iter->second->drb_id,
                  "Query for drb_id={} in pdu_session_id={} provided different drb_id={}",
                  drb_to_mod.drb_id,
                  session.pdu_session_id,
                  drb_iter->second->drb_id);

    auto& drb = drb_iter->second;
    if (new_tnl_info_required) {
      if (not f1u_teid_allocator.release_teid(drb->f1u_ul_teid)) {
        logger.error("ue={} could not free old ul_teid={}", ue_index, drb->f1u_ul_teid);
      }

      // Allocate new UL TEID for DRB
      expected<gtpu_teid_t> ret = f1u_teid_allocator.request_teid();
      if (not ret.has_value()) {
        logger.error("ue={} could not allocate ul_teid", ue_index);
        continue;
      }
      drb->f1u_ul_teid = ret.value();

      // Create UL UP TNL address.
      up_transport_layer_info f1u_ul_tunnel_addr;
      f1u_ul_tunnel_addr.tp_address.from_string(net_config.f1u_bind_addr);
      f1u_ul_tunnel_addr.gtp_teid = drb->f1u_ul_teid;

      // create new F1-U and connect it. This will automatically disconnect the old F1-U.
      drb->f1u = f1u_gw.create_cu_bearer(
          ue_index, f1u_ul_tunnel_addr, drb->f1u_to_pdcp_adapter, drb->f1u_to_pdcp_adapter, timers);
      drb_iter->second->pdcp_to_f1u_adapter.disconnect_f1u();

      drb_result.gtp_tunnel = f1u_ul_tunnel_addr;
    }

    // F1-U apply modification
    if (!drb_to_mod.dl_up_params.empty()) {
      up_transport_layer_info f1u_ul_tunnel_addr;
      f1u_ul_tunnel_addr.tp_address.from_string(net_config.f1u_bind_addr);
      f1u_ul_tunnel_addr.gtp_teid = drb_iter->second->f1u_ul_teid;
      f1u_gw.attach_dl_teid(f1u_ul_tunnel_addr, drb_to_mod.dl_up_params[0].up_tnl_info);

      drb_iter->second->pdcp_to_f1u_adapter.connect_f1u(drb_iter->second->f1u->get_tx_sdu_handler());
    }

    // Apply re-establishment at PDCP
    if (drb_to_mod.pdcp_cfg.has_value() && drb_to_mod.pdcp_cfg->pdcp_reest.has_value()) {
      // TODO get correct AS security config from E1AP
      drb_iter->second->pdcp->get_rx_upper_control_interface().reestablish({});
      drb_iter->second->pdcp->get_tx_upper_control_interface().reestablish({});
    }

    logger.info("Modified DRB. drb_id={}, pdu_session_id={}, f1u_teid={}.",
                drb_to_mod.drb_id,
                session.pdu_session_id,
                drb->f1u_ul_teid);

    // Apply QoS flows
    for (auto& qos_flow_info : drb_to_mod.flow_map_info) {
      // TODO
      logger.warning("Unsupported modification of QoS flow. qos_flow_id={}", qos_flow_info.qos_flow_id);
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
      logger.warning("Cannot remove DRB: drb_id={} not found in pdu_session_id={}", drb_to_rem, session.pdu_session_id);
      continue;
    }
    srsran_assert(drb_to_rem == drb_iter->second->drb_id,
                  "Query for drb_id={} in pdu_session_id={} provided different drb_id={}",
                  drb_to_rem,
                  session.pdu_session_id,
                  drb_iter->second->drb_id);

    // remove DRB (this will automatically disconnect from F1-U gateway)
    if (not f1u_teid_allocator.release_teid(drb_iter->second->f1u_ul_teid)) {
      logger.error("ue={} psi={} drb_id={} could not free ul_teid={}", ue_index, session.pdu_session_id, drb_to_rem);
    }
    pdu_session->drbs.erase(drb_iter);

    logger.info("Removed DRB. drb_id={}, pdu_session_id={}.", drb_to_rem, session.pdu_session_id);
  }

  pdu_session_result.success = true;
  return pdu_session_result;
}

void pdu_session_manager_impl::remove_pdu_session(pdu_session_id_t pdu_session_id)
{
  if (pdu_sessions.find(pdu_session_id) == pdu_sessions.end()) {
    logger.error("PDU session {} not found", pdu_session_id);
    return;
  }

  // Disconnect all UL tunnels for this PDU session.
  auto& pdu_session = pdu_sessions.at(pdu_session_id);
  for (const auto& drb : pdu_session->drbs) {
    logger.debug("Disconnecting CU bearer with UL-TEID={}", drb.second->f1u_ul_teid);
    up_transport_layer_info f1u_ul_tunnel_addr;
    f1u_ul_tunnel_addr.tp_address.from_string(net_config.f1u_bind_addr);
    f1u_ul_tunnel_addr.gtp_teid = drb.second->f1u_ul_teid;
    f1u_gw.disconnect_cu_bearer(f1u_ul_tunnel_addr);
    if (f1u_teid_allocator.release_teid(drb.second->f1u_ul_teid)) {
      logger.error("ue={} psi={} could not remove ul_teid at session termination. ul_teid={}",
                   ue_index,
                   pdu_session_id,
                   drb.second->f1u_ul_teid);
    }
  }

  pdu_sessions.erase(pdu_session_id);
  logger.info("Removing PDU session {}", pdu_session_id);
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
