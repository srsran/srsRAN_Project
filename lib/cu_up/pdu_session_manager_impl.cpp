/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdu_session_manager_impl.h"
#include "ue_context.h"
#include "srsgnb/e1/cu_up/e1_config_converters.h"
#include "srsgnb/pdcp/pdcp_factory.h"
#include "srsgnb/sdap/sdap_factory.h"

using namespace srsgnb;
using namespace srs_cu_up;

pdu_session_manager_impl::pdu_session_manager_impl(ue_index_t                           ue_index_,
                                                   network_interface_config&            net_config_,
                                                   srslog::basic_logger&                logger_,
                                                   timer_manager&                       timers_,
                                                   f1u_cu_up_gateway&                   f1u_gw_,
                                                   gtpu_tunnel_tx_upper_layer_notifier& gtpu_tx_notifier_,
                                                   gtpu_demux_ctrl&                     gtpu_rx_demux_) :
  ue_index(ue_index_),
  net_config(net_config_),
  logger(logger_),
  timers(timers_),
  gtpu_tx_notifier(gtpu_tx_notifier_),
  gtpu_rx_demux(gtpu_rx_demux_),
  f1u_gw(f1u_gw_)
{
}

pdu_session_manager_impl::~pdu_session_manager_impl()
{
  for (auto& session_it : pdu_sessions) {
    // Remove GTP-U tunnel from GTP-U demux
    gtpu_rx_demux.remove_tunnel(session_it.second->local_teid);
  }
}

pdu_session_setup_result
pdu_session_manager_impl::setup_pdu_session(const asn1::e1ap::pdu_session_res_to_setup_item_s& session)
{
  pdu_session_setup_result pdu_session_result = {};
  pdu_session_result.success                  = false;
  pdu_session_result.pdu_session_id           = session.pdu_session_id;
  pdu_session_result.cause.set_radio_network();

  if (pdu_sessions.find(session.pdu_session_id) != pdu_sessions.end()) {
    logger.error("PDU Session {} already exists", session.pdu_session_id);
    return pdu_session_result;
  }

  if (pdu_sessions.size() >= MAX_NUM_PDU_SESSIONS_PER_UE) {
    logger.error("PDU Session {} cannot be created, max number of PDU sessions reached", session.pdu_session_id);
    return pdu_session_result;
  }

  pdu_sessions[session.pdu_session_id]         = std::make_unique<pdu_session>(session);
  std::unique_ptr<pdu_session>& new_session    = pdu_sessions[session.pdu_session_id];
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
  n3_dl_tunnel_addr.gtp_teid = int_to_gtp_teid(new_session->local_teid);
  up_transport_layer_info_to_asn1(pdu_session_result.gtp_tunnel, n3_dl_tunnel_addr);

  // Create SDAP entity
  sdap_entity_creation_message sdap_msg = {};
  sdap_msg.rx_sdu_notifier              = &new_session->sdap_to_gtpu_adapter;
  sdap_msg.tx_pdu_notifier              = &new_session->sdap_to_pdcp_adapter;
  new_session->sdap                     = create_sdap(sdap_msg);

  // Create GTPU entity
  gtpu_tunnel_creation_message msg = {};
  msg.ue_index                     = ue_index;
  msg.cfg.tx.peer_teid             = ul_tunnel_info.gtp_teid.value();
  msg.cfg.tx.peer_addr             = ul_tunnel_info.tp_address.to_string();
  msg.cfg.tx.peer_port             = net_config.upf_port;
  msg.cfg.rx.local_teid            = new_session->local_teid;
  msg.rx_lower                     = &new_session->gtpu_to_sdap_adapter;
  msg.tx_upper                     = &gtpu_tx_notifier;
  new_session->gtpu                = create_gtpu_tunnel(msg);

  // Connect adapters
  new_session->sdap_to_gtpu_adapter.connect_gtpu(*new_session->gtpu->get_tx_lower_layer_interface());
  new_session->gtpu_to_sdap_adapter.connect_sdap(new_session->sdap->get_sdap_tx_sdu_handler());

  // Register tunnel at demux
  if (gtpu_rx_demux.add_tunnel(new_session->local_teid, new_session->gtpu->get_rx_upper_layer_interface()) == false) {
    logger.error(
        "PDU Session {} cannot be created. TEID {} already exists", session.pdu_session_id, new_session->local_teid);
    return pdu_session_result;
  }

  srsgnb_assert(session.drb_to_setup_list_ng_ran.size() <= 1,
                "PDU Session {} cannot be created: Current implementation assumes one DRB per PDU session!",
                session.pdu_session_id);

  // Handle DRB setup
  for (size_t i = 0; i < session.drb_to_setup_list_ng_ran.size(); ++i) {
    // prepare DRB creation result
    drb_setup_result drb_result = {};
    drb_result.success          = false;
    drb_result.cause.set_radio_network();
    drb_result.drb_id = session.drb_to_setup_list_ng_ran[i].drb_id;

    // get DRB from list and create context
    const asn1::e1ap::drb_to_setup_item_ng_ran_s& drb = session.drb_to_setup_list_ng_ran[i];
    new_session->drbs[drb.drb_id]                     = std::make_unique<drb_context>(drb);
    auto& new_drb                                     = new_session->drbs[drb.drb_id];

    // Create PDCP entity
    srsgnb::pdcp_entity_creation_message pdcp_msg = {};
    pdcp_msg.ue_index                             = ue_index;
    pdcp_msg.rb_id                                = uint_to_drb_id(drb.drb_id);
    pdcp_msg.config                               = make_pdcp_drb_config(drb.pdcp_cfg);
    pdcp_msg.tx_lower                             = &new_drb->pdcp_to_f1u_adapter;
    pdcp_msg.tx_upper_cn                          = &new_drb->pdcp_tx_to_e1_adapter;
    pdcp_msg.rx_upper_dn                          = &new_drb->pdcp_to_sdap_adapter;
    pdcp_msg.rx_upper_cn                          = &new_drb->pdcp_rx_to_e1_adapter;
    pdcp_msg.timers                               = &timers;
    new_drb->pdcp                                 = srsgnb::create_pdcp_entity(pdcp_msg);

    // Connect "PDCP-E1" adapter to E1
    new_drb->pdcp_tx_to_e1_adapter.connect_e1(); // TODO: pass actual E1 handler
    new_drb->pdcp_rx_to_e1_adapter.connect_e1(); // TODO: pass actual E1 handler

    // Create  F1-U bearer
    uint32_t f1u_ul_teid = allocate_local_f1u_teid(new_session->pdu_session_id, drb.drb_id);
    new_drb->f1u =
        f1u_gw.create_cu_bearer(ue_index, f1u_ul_teid, new_drb->f1u_to_pdcp_adapter, new_drb->f1u_to_pdcp_adapter);
    new_drb->f1u_ul_teid = f1u_ul_teid;

    up_transport_layer_info f1u_ul_tunnel_addr;
    f1u_ul_tunnel_addr.tp_address.from_string(net_config.f1u_bind_addr);
    f1u_ul_tunnel_addr.gtp_teid = int_to_gtp_teid(f1u_ul_teid);
    up_transport_layer_info_to_asn1(drb_result.gtp_tunnel, f1u_ul_tunnel_addr);

    srsgnb_assert(drb.qos_flow_info_to_be_setup.size() <= 1,
                  "DRB with drbid={} of PDU Session {} cannot be created: Current implementation assumes one QoS "
                  "flow per DRB!",
                  drb.drb_id,
                  session.pdu_session_id);

    // Connect F1-U's "F1-U->PDCP adapter" directly to PDCP
    new_drb->f1u_to_pdcp_adapter.connect_pdcp(new_drb->pdcp->get_rx_lower_interface(),
                                              new_drb->pdcp->get_tx_lower_interface());
    new_drb->pdcp_to_f1u_adapter.connect_f1u(new_drb->f1u->get_tx_sdu_handler());

    // Create QoS flows
    for (size_t k = 0; k < drb.qos_flow_info_to_be_setup.size(); ++k) {
      // prepare QoS flow creation result
      qos_flow_setup_result flow_result = {};
      flow_result.success               = false;
      flow_result.cause.set_radio_network();
      flow_result.qos_flow_id = drb.qos_flow_info_to_be_setup[k].qos_flow_id;

      // create QoS flow context
      auto& qos_flow                           = drb.qos_flow_info_to_be_setup[k];
      new_drb->qos_flows[qos_flow.qos_flow_id] = std::make_unique<qos_flow_context>(qos_flow);
      auto& new_qos_flow                       = new_drb->qos_flows[qos_flow.qos_flow_id];
      logger.debug(
          "Created QoS flow with qos_flow_id={} and five_qi={}", new_qos_flow->qos_flow_id, new_qos_flow->five_qi);

      // TODO: somehow take the FiveQI and configure SDAP

      // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
      // Connect the DRB's "PDCP->SDAP adapter" directly to SDAP
      new_drb->pdcp_to_sdap_adapter.connect_sdap(new_session->sdap->get_sdap_rx_pdu_handler());
      // Connect SDAP's "SDAP->PDCP adapter" directly to PDCP
      new_session->sdap_to_pdcp_adapter.connect_pdcp(new_drb->pdcp->get_tx_upper_data_interface());

      // Add QoS flow creation result
      flow_result.success = true;
      drb_result.qos_flow_results.push_back(flow_result);
    }

    // Add result
    drb_result.success = true;
    pdu_session_result.drb_setup_results.push_back(drb_result);
  }

  pdu_session_result.success = true;
  return pdu_session_result;
}

pdu_session_modification_result
pdu_session_manager_impl::modify_pdu_session(const asn1::e1ap::pdu_session_res_to_modify_item_s& session)
{
  pdu_session_modification_result pdu_session_result;
  auto&                           pdu_session = pdu_sessions[session.pdu_session_id];

  // > DRB To Setup List
  for (const auto& drb_to_setup : session.drb_to_setup_list_ng_ran) {
    // prepare DRB creation result
    drb_setup_result drb_result = {};
    drb_result.success          = false;
    drb_result.cause.set_radio_network();
    drb_result.drb_id = drb_to_setup.drb_id;

    // TODO: handle session.drb_to_setup_list_ng_ran
    logger.warning("Setup of new DRBs via PDU session modification is not supported: session={}, drb_id={}",
                   session.pdu_session_id,
                   drb_to_setup.drb_id);

    // Add result - success == false
    pdu_session_result.drb_setup_results.push_back(drb_result);
  }

  // > DRB To Modify List
  for (const auto& drb_to_mod : session.drb_to_modify_list_ng_ran) {
    // prepare DRB modification result
    drb_setup_result drb_result = {};
    drb_result.success          = false;
    drb_result.cause.set_radio_network();
    drb_result.drb_id = drb_to_mod.drb_id;

    // find DRB in PDU session
    auto drb_iter = pdu_session->drbs.find(drb_to_mod.drb_id);
    if (drb_iter == pdu_session->drbs.end()) {
      logger.warning(
          "Cannot modify DRB: drb_id={} not found in pdu_session_id={}", drb_to_mod.drb_id, session.pdu_session_id);
      pdu_session_result.drb_setup_results.push_back(drb_result);
      continue;
    }
    srsgnb_assert(drb_to_mod.drb_id == drb_iter->second->drb_id,
                  "Query for drb_id={} in pdu_session_id={} provided different drb_id={}",
                  drb_to_mod.drb_id,
                  session.pdu_session_id,
                  drb_iter->second->drb_id);

    // apply modification
    f1u_gw.attach_dl_teid(drb_iter->second->f1u_ul_teid,
                          drb_to_mod.dl_up_params[0].up_tnl_info.gtp_tunnel().gtp_teid.to_number());
    logger.info("Modified DRB. drb_id={}, pdu_session_id={}.", drb_to_mod.drb_id, session.pdu_session_id);

    // Add result
    drb_result.success = true;
    pdu_session_result.drb_setup_results.push_back(drb_result);
  }

  // > DRB To Remove List
  for (const auto& drb_to_rem : session.drb_to_rem_list_ng_ran) {
    // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
    // disconnect SDAP->PDCP direct connection
    pdu_session->sdap_to_pdcp_adapter.disconnect_pdcp();

    // remove DRB from PDU session
    auto drb_iter = pdu_session->drbs.find(drb_to_rem.drb_id);
    if (drb_iter == pdu_session->drbs.end()) {
      logger.warning(
          "Cannot remove DRB: drb_id={} not found in pdu_session_id={}", drb_to_rem.drb_id, session.pdu_session_id);
      continue;
    }
    srsgnb_assert(drb_to_rem.drb_id == drb_iter->second->drb_id,
                  "Query for drb_id={} in pdu_session_id={} provided different drb_id={}",
                  drb_to_rem.drb_id,
                  session.pdu_session_id,
                  drb_iter->second->drb_id);
    // remove DRB (this will automatically disconnect from F1-U gateway)
    pdu_session->drbs.erase(drb_iter);

    logger.info("Removed DRB. drb_id={}, pdu_session_id={}.", drb_to_rem.drb_id, session.pdu_session_id);
  }

  return pdu_session_result;
}

void pdu_session_manager_impl::remove_pdu_session(uint8_t pdu_session_id)
{
  if (pdu_sessions.find(pdu_session_id) == pdu_sessions.end()) {
    logger.error("PDU session {} not found", pdu_session_id);
    return;
  }

  pdu_sessions.erase(pdu_session_id);
  logger.info("Removing PDU session {}", pdu_session_id);
}

size_t pdu_session_manager_impl::get_nof_pdu_sessions()
{
  return pdu_sessions.size();
}

uint32_t pdu_session_manager_impl::allocate_local_teid(uint8_t pdu_session_id)
{
  // Local TEID is the concatenation of the unique UE index and the PDU session ID
  uint32_t local_teid = ue_index;
  local_teid <<= 8U;
  local_teid |= pdu_session_id;
  return local_teid;
}

uint32_t pdu_session_manager_impl::allocate_local_f1u_teid(uint8_t pdu_session_id, uint8_t drb_id)
{
  // Local TEID is the concatenation of the unique UE index, the PDU session ID and the DRB Id
  uint32_t local_teid = ue_index;
  local_teid <<= 8U;
  local_teid |= pdu_session_id;
  local_teid <<= 8U;
  local_teid |= drb_id;
  return local_teid;
}
