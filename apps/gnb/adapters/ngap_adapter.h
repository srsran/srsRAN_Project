/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/ngap/ngap_asn1_packer.h"
#include "srsgnb/gateways/sctp_network_gateway_factory.h"
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/support/io_broker/io_broker.h"

namespace srsgnb {

namespace srs_cu_cp {

/// \brief NGAP bridge that uses the IO broker to handle the SCTP connection
class ngap_network_adapter : public ngap_message_notifier,
                             public ngap_message_handler,
                             public sctp_network_gateway_control_notifier,
                             public network_gateway_data_notifier
{
public:
  ngap_network_adapter(io_broker& broker_, ngap_pcap& pcap_) : broker(broker_), pcap(pcap_)
  {
    if (gateway_ctrl_handler != nullptr) {
      broker.unregister_fd(gateway_ctrl_handler->get_socket_fd());
    }
  }

  void connect_gateway(sctp_network_gateway_controller*   gateway_ctrl_handler_,
                       sctp_network_gateway_data_handler* gateway_data_handler_)
  {
    gateway_ctrl_handler = gateway_ctrl_handler_;
    gateway_data_handler = gateway_data_handler_;

    packer = std::make_unique<ngap_asn1_packer>(*gateway_data_handler, *this, pcap);

    gateway_ctrl_handler->create_and_connect();
    broker.register_fd(gateway_ctrl_handler->get_socket_fd(), [this](int fd) { gateway_ctrl_handler->receive(); });
  }

  void connect_ngap(ngap_message_handler* ngap_msg_handler_, ngap_event_handler* event_handler_)
  {
    ngap_msg_handler = ngap_msg_handler_;
    event_handler    = event_handler_;
  }

  void disconnect_gateway()
  {
    report_fatal_error_if_not(gateway_ctrl_handler, "Gateway handler not set.");
    broker.unregister_fd(gateway_ctrl_handler->get_socket_fd());

    gateway_ctrl_handler = nullptr;
    gateway_data_handler = nullptr;

    packer.reset();
  }

private:
  // NGAP calls interface to send (unpacked) NGAP PDUs
  void on_new_message(const ngap_message& msg) override
  {
    if (packer) {
      packer->handle_message(msg);
    } else {
      logger.debug("NGAP ASN1 packer disconnected, dropping msg");
    }
  }

  // SCTP network gateway calls interface to inject received PDUs (ASN1 packed)
  void on_new_pdu(byte_buffer pdu) override
  {
    if (packer) {
      packer->handle_packed_pdu(pdu);
    } else {
      logger.debug("NGAP ASN1 packer disconnected, dropping pdu");
    }
  }

  // The packer calls this interface to inject unpacked NGAP PDUs
  void handle_message(const ngap_message& msg) override
  {
    report_fatal_error_if_not(ngap_msg_handler, "NGAP handler not set.");
    ngap_msg_handler->handle_message(msg);
  }

  void on_connection_loss() override
  {
    report_fatal_error_if_not(event_handler, "NGAP handler not set.");
    event_handler->handle_connection_loss();
  }

  void on_connection_established() override
  {
    // TODO: extend event interface to inform about connection establishment
    logger.debug("on_connection_established");
  }

  io_broker&                         broker;
  ngap_pcap&                         pcap;
  std::unique_ptr<ngap_asn1_packer>  packer;
  sctp_network_gateway_controller*   gateway_ctrl_handler = nullptr;
  sctp_network_gateway_data_handler* gateway_data_handler = nullptr;
  ngap_message_handler*              ngap_msg_handler     = nullptr;
  ngap_event_handler*                event_handler        = nullptr;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("SCTP-GW");
};

}; // namespace srs_cu_cp

}; // namespace srsgnb
