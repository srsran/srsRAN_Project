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

#include "dummy_ric.h"

using namespace srsran;

class ric_to_gw_pdu_notifier final : public e2_message_notifier
{
public:
  ric_to_gw_pdu_notifier(std::unique_ptr<sctp_association_sdu_notifier> sctp_sender_,
                         dlt_pcap&                                      pcap_writer_,
                         srslog::basic_logger&                          logger_) :
    sctp_sender(std::move(sctp_sender_)), pcap_writer(pcap_writer_), logger(logger_)
  {
  }

  /// Handle unpacked Tx E2AP PDU by packing and forwarding it into the SCTP GW.
  void on_new_message(const e2_message& msg) override
  {
    // pack E2AP PDU into SCTP SDU.
    byte_buffer   tx_sdu{byte_buffer::fallback_allocation_tag{}};
    asn1::bit_ref bref(tx_sdu);
    if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack E2AP PDU");
      return;
    }

    // Push Tx PDU to pcap.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(tx_sdu.copy());
    }

    // Forward packed E2AP Tx PDU to SCTP gateway.
    sctp_sender->on_new_sdu(std::move(tx_sdu));
  }

private:
  std::unique_ptr<sctp_association_sdu_notifier> sctp_sender;
  dlt_pcap&                                      pcap_writer;
  srslog::basic_logger&                          logger;
};

/// Notifier passed to the SCTP GW, which the GW will use to forward E2AP Rx PDUs to the RIC.
class gw_to_ric_pdu_notifier final : public sctp_association_sdu_notifier
{
public:
  gw_to_ric_pdu_notifier(std::unique_ptr<e2_message_notifier> e2_notifier_,
                         dlt_pcap&                            pcap_writer_,
                         srslog::basic_logger&                logger_,
                         e2_message_notifier*                 rx_sniffer_) :
    e2_notifier(std::move(e2_notifier_)), pcap_writer(pcap_writer_), logger(logger_), rx_sniffer(rx_sniffer_)
  {
  }

  bool on_new_sdu(byte_buffer sdu) override
  {
    // Unpack SCTP SDU into E2AP PDU.
    asn1::cbit_ref bref(sdu);
    e2_message     msg;
    if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't unpack E2AP PDU");
      return false;
    }

    // Forward SCTP Rx SDU to pcap, if enabled.
    if (pcap_writer.is_write_enabled()) {
      pcap_writer.push_pdu(sdu.copy());
    }

    if (rx_sniffer) {
      rx_sniffer->on_new_message(msg);
    }

    // Forward unpacked Rx PDU to the RIC.
    e2_notifier->on_new_message(msg);

    return true;
  }

private:
  std::unique_ptr<e2_message_notifier> e2_notifier;
  dlt_pcap&                            pcap_writer;
  srslog::basic_logger&                logger;
  e2_message_notifier*                 rx_sniffer;
};

class ric_sctp_server final : public e2_connection_server, public sctp_network_association_factory
{
public:
  ric_sctp_server(const e2_sctp_gateway_config& params_) : params(params_)
  {
    // Create SCTP server.
    sctp_server = create_sctp_network_server(sctp_network_server_config{params.sctp, params.broker, *this});
  }

  void attach_ric(ric_e2_handler& ric_) override
  {
    ric_e2_con_handler = &ric_;

    // Start listening for new SCTP connections.
    bool result = sctp_server->listen();
    report_fatal_error_if_not(result, "Failed to start SCTP server.\n");
  }

  std::optional<uint16_t> get_listen_port() const override { return sctp_server->get_listen_port(); }

  std::unique_ptr<sctp_association_sdu_notifier>
  create(std::unique_ptr<sctp_association_sdu_notifier> sctp_send_notifier) override
  {
    // Create an unpacked E2AP PDU notifier and pass it to the RIC.
    auto e2_sender = std::make_unique<ric_to_gw_pdu_notifier>(std::move(sctp_send_notifier), params.pcap, logger);

    std::unique_ptr<e2_message_notifier> e2_receiver =
        ric_e2_con_handler->handle_new_du_connection(std::move(e2_sender));

    // Wrap the received E2AP Rx PDU notifier in an SCTP notifier and return it.
    if (e2_receiver == nullptr) {
      return nullptr;
    }
    return std::make_unique<gw_to_ric_pdu_notifier>(std::move(e2_receiver), params.pcap, logger, params.rx_sniffer);
  }

private:
  const e2_sctp_gateway_config         params;
  srslog::basic_logger&                logger             = srslog::fetch_basic_logger("RIC");
  ric_e2_handler*                      ric_e2_con_handler = nullptr;
  std::unique_ptr<sctp_network_server> sctp_server;
};

std::unique_ptr<e2_connection_server> srsran::create_e2_gateway_server(const e2_sctp_gateway_config& cfg)
{
  return std::make_unique<ric_sctp_server>(cfg);
}

class e2_agent_connection_manager::e2_gw_to_ric_pdu_adapter final : public e2_message_notifier
{
public:
  e2_gw_to_ric_pdu_adapter(e2_agent_connection_manager& parent_, uint32_t e2_agent_id_) :
    parent(parent_), e2_agent_id(e2_agent_id_)
  {
  }

  void on_new_message(const e2_message& msg) override
  {
    parent.logger.info("RIC received a msg from E2Agent id={}", e2_agent_id);
  }

private:
  e2_agent_connection_manager& parent;
  unsigned                     e2_agent_id;
};

e2_agent_connection_manager::e2_agent_connection_manager(e2_agent_repository& e2_agents_) :
  logger(srslog::fetch_basic_logger("E2-RIC")), e2_agents(e2_agents_){};

unsigned e2_agent_connection_manager::get_next_e2_agent_index()
{
  for (unsigned idx = 0; idx < 100; idx++) {
    if (e2_agents.db.find(idx) == e2_agents.db.end()) {
      return idx;
    }
  }
  return 255;
}

std::unique_ptr<e2_message_notifier>
e2_agent_connection_manager::handle_new_du_connection(std::unique_ptr<e2_message_notifier> e2_tx_pdu_notifier)
{
  logger.info("RIC handled a new E2Agent connection.");
  // Increment number of E2 Agents connections.
  unsigned agent_idx = get_next_e2_agent_index();
  if (agent_idx == 255) {
    logger.error("Failed to store new E2 Agent connection");
    return nullptr;
  }
  if (not e2_agents.db.insert(std::make_pair(agent_idx, std::move(e2_tx_pdu_notifier))).second) {
    logger.error("Failed to store new E2 Agent connection idx={}", agent_idx);
    return nullptr;
  }
  return std::make_unique<e2_gw_to_ric_pdu_adapter>(*this, agent_idx);
}

near_rt_ric::near_rt_ric() : logger(srslog::fetch_basic_logger("E2-RIC")), e2_agent_mng(e2_agents){};

void near_rt_ric::send_msg(unsigned e2_agent_idx, const e2_message& msg)
{
  auto it = e2_agents.db.find(e2_agent_idx);
  if (it == e2_agents.db.end()) {
    return;
  }

  logger.info("RIC sends msg.");
  it->second->on_new_message(msg);
};
