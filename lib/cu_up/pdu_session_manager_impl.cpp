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
#include "srsgnb/pdcp/pdcp_factory.h"
#include "srsgnb/sdap/sdap_factory.h"

using namespace srsgnb;
using namespace srs_cu_up;

pdu_session_manager_impl::pdu_session_manager_impl(ue_index_t            ue_index_,
                                                   srslog::basic_logger& logger_,
                                                   timer_manager&        timers_,
                                                   f1u_cu_up_connector&  f1u_conn_,
                                                   gtpu_demux_ctrl&      ngu_demux_) :
  ue_index(ue_index_), logger(logger_), timers(timers_), ngu_demux(ngu_demux_), f1u_conn(f1u_conn_)
{
  (void)f1u_conn;
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

  pdu_sessions[session.pdu_session_id] = std::make_unique<pdu_session>(session);
  auto&       new_session              = pdu_sessions[session.pdu_session_id];
  const auto& peer_teid                = new_session->tunnel_info.gtp_tunnel().gtp_teid.to_number();

  // Allocate local TEID
  new_session->local_teid = allocate_local_teid(new_session->pdu_session_id);

  // Create SDAP entity
  sdap_entity_creation_message sdap_msg = {};
  sdap_msg.rx_sdu_notifier              = &new_session->sdap_to_gtpu_adapter;
  sdap_msg.tx_pdu_notifier              = &new_session->sdap_to_pdcp_adapter;
  new_session->sdap                     = create_sdap(sdap_msg);

  // Create GTPU entity
  gtpu_tunnel_creation_message msg = {};
  msg.cfg.tx.peer_teid             = peer_teid;
  msg.cfg.rx.local_teid            = new_session->local_teid;
  msg.rx_lower                     = &new_session->gtpu_to_sdap_adapter;
  // msg.tx_upper                         = &gtpu_tx; // TODO: register UDP gateway
  new_session->gtpu = create_gtpu_tunnel(msg);

  // Connect adapters
  new_session->sdap_to_gtpu_adapter.connect_gtpu(*new_session->gtpu->get_tx_lower_layer_interface());
  new_session->gtpu_to_sdap_adapter.connect_sdap(new_session->sdap->get_sdap_tx_sdu_handler());

  // Register tunnel at demux
  if (ngu_demux.add_tunnel(peer_teid, new_session->gtpu->get_rx_upper_layer_interface()) == false) {
    logger.error("PDU Session {} cannot be created. TEID {} already exists", session.pdu_session_id, peer_teid);
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
    auto& drb                     = session.drb_to_setup_list_ng_ran[i];
    new_session->drbs[drb.drb_id] = std::make_unique<drb_context>(drb);
    auto& new_drb                 = new_session->drbs[drb.drb_id];

    // Create PDCP entity
    srsgnb::pdcp_entity_creation_message pdcp_msg = {};
    pdcp_msg.ue_index                             = 0;  // What do we use here?
    pdcp_msg.lcid               = lcid_t(LCID_MIN_DRB); // What do we use here? There is no LCID in the CU-UP
    pdcp_msg.config             = {};                   // TODO: writer converter from ASN1 for msg.pdcp_cfg;
    pdcp_msg.config.tx.rb_type  = pdcp_rb_type::drb;
    pdcp_msg.config.tx.rlc_mode = pdcp_rlc_mode::am;
    pdcp_msg.config.rx.rb_type  = pdcp_rb_type::drb;
    pdcp_msg.config.rx.rlc_mode = pdcp_rlc_mode::am;
    pdcp_msg.tx_lower           = nullptr; // TODO: add adapter towards F1-U
    pdcp_msg.tx_upper_cn        = nullptr; // TODO: add adapter towards RRC
    pdcp_msg.rx_upper_dn        = &new_drb->pdcp_to_sdap_adapter;
    pdcp_msg.rx_upper_cn        = nullptr; // TODO: add adapter towards RRC
    pdcp_msg.timers             = &timers;
    new_drb->pdcp_bearer        = srsgnb::create_pdcp_entity(pdcp_msg);

    srsgnb_assert(
        drb.qos_flow_info_to_be_setup.size() <= 1,
        "DRB with drbid={} of PDU Session {} cannot be created: Current implementation assumes one QoS flow per DRB!",
        drb.drb_id,
        session.pdu_session_id);

    // Create QoS flows
    for (size_t k = 0; k < drb.qos_flow_info_to_be_setup.size(); ++k) {
      // prepare QoS flow creation result
      qos_flow_setup_result flow_result = {};
      flow_result.success               = false;
      flow_result.cause.set_radio_network();
      flow_result.qos_flow_id = drb.qos_flow_info_to_be_setup[k].qo_s_flow_id;

      // create QoS flow context
      auto& qos_flow                            = drb.qos_flow_info_to_be_setup[k];
      new_drb->qos_flows[qos_flow.qo_s_flow_id] = std::make_unique<qos_flow_context>(qos_flow);
      auto& new_qos_flow                        = new_drb->qos_flows[qos_flow.qo_s_flow_id];
      logger.info(
          "Created QoS flow with qos_flow_id={} and five_qi={}", new_qos_flow->qos_flow_id, new_qos_flow->five_qi);

      // TODO: somehow take the FiveQI and configure SDAP

      // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
      // Connect the DRB's "PDCP->SDAP adapter" directly to SDAP
      new_drb->pdcp_to_sdap_adapter.connect_sdap(new_session->sdap->get_sdap_rx_pdu_handler());
      // Connect SDAP's "SDAP->PDCP adapter" directly to PDCP
      new_session->sdap_to_pdcp_adapter.connect_pdcp(new_drb->pdcp_bearer->get_tx_upper_data_interface());

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
  local_teid <<= 8;
  local_teid |= pdu_session_id;
  return local_teid;
}
