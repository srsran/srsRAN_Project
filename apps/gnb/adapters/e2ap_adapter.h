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

#pragma once

#include "srsran/e2/e2.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/io/io_broker.h"

namespace srsran {

/// \brief E2AP bridge that uses the IO broker to handle the SCTP connection
class e2ap_network_adapter : public e2_message_notifier,
                             public e2_message_handler,
                             public sctp_network_gateway_control_notifier,
                             public network_gateway_data_notifier
{
public:
  e2ap_network_adapter(io_broker& broker_, dlt_pcap& pcap_) : broker(broker_), pcap(pcap_) {}

  void connect_gateway(std::unique_ptr<sctp_network_gateway> gateway_)
  {
    gateway              = std::move(gateway_);
    gateway_ctrl_handler = gateway.get();
    gateway_data_handler = gateway.get();

    packer = create_e2ap_asn1_packer(*gateway_data_handler, *this, pcap);

    if (!gateway_ctrl_handler->create_and_connect()) {
      report_error("Failed to create SCTP gateway.\n");
    }
    bool success =
        broker.register_fd(gateway_ctrl_handler->get_socket_fd(), [this](int fd) { gateway_ctrl_handler->receive(); });
    if (!success) {
      report_fatal_error("Failed to register E2 (SCTP) network gateway at IO broker. socket_fd={}",
                         gateway_ctrl_handler->get_socket_fd());
    }
  }

  void bind_and_listen(std::unique_ptr<sctp_network_gateway> gateway_)
  {
    gateway              = std::move(gateway_);
    gateway_ctrl_handler = gateway.get();
    gateway_data_handler = gateway.get();

    packer = create_e2ap_asn1_packer(*gateway_data_handler, *this, pcap);

    if (!gateway_ctrl_handler->create_and_bind()) {
      report_error("Failed to create SCTP gateway.\n");
    }

    if (!gateway_ctrl_handler->listen()) {
      report_error("Failed to listen SCTP gateway.\n");
    }

    bool success =
        broker.register_fd(gateway_ctrl_handler->get_socket_fd(), [this](int fd) { gateway_ctrl_handler->receive(); });
    if (!success) {
      report_fatal_error("Failed to register E2 (SCTP) network gateway at IO broker. socket_fd={}",
                         gateway_ctrl_handler->get_socket_fd());
    }
  }

  /// \brief Return the port on which the gateway is listening.
  ///
  /// In case the gateway was configured to listen on port 0, i.e. the operating system shall pick a random free port,
  /// this function can be used to get the actual port number.
  optional<uint16_t> get_listen_port()
  {
    if (gateway_ctrl_handler == nullptr) {
      return {};
    }
    return gateway_ctrl_handler->get_listen_port();
  }

  void connect_e2ap(e2_message_handler* e2ap_msg_handler_, e2_event_handler* event_handler_)
  {
    e2ap_msg_handler = e2ap_msg_handler_;
    event_handler    = event_handler_;
  }

  void disconnect_gateway()
  {
    srsran_assert(gateway_ctrl_handler, "Gateway handler not set.");
    bool success = broker.unregister_fd(gateway_ctrl_handler->get_socket_fd());
    if (!success) {
      report_fatal_error("Failed to unregister E2 (SCTP) network gateway at IO broker. socket_fd={}",
                         gateway_ctrl_handler->get_socket_fd());
    }

    gateway_ctrl_handler = nullptr;
    gateway_data_handler = nullptr;

    packer.reset();
  }

private:
  // E2AP calls interface to send (unpacked) E2AP PDUs
  void on_new_message(const e2_message& msg) override
  {
    srsran_assert(packer, "E2AP ASN1 packer disconnected");
    packer->handle_message(msg);
  }

  // SCTP network gateway calls interface to inject received PDUs (ASN1 packed)
  void on_new_pdu(byte_buffer pdu) override
  {
    srsran_assert(packer, "E2AP ASN1 packer disconnected");
    packer->handle_packed_pdu(pdu);
  }

  // The packer calls this interface to inject unpacked E2AP PDUs
  void handle_message(const e2_message& msg) override
  {
    srsran_assert(e2ap_msg_handler, "E2AP msg handler not set.");
    e2ap_msg_handler->handle_message(msg);
  }

  void on_connection_loss() override
  {
    srsran_assert(event_handler, "E2AP event handler not set.");
    event_handler->handle_connection_loss();
  }

  void on_connection_established() override
  {
    // TODO: extend event interface to inform about connection establishment
    logger.debug("on_connection_established");
  }

  io_broker&                            broker;
  dlt_pcap&                             pcap;
  std::unique_ptr<e2ap_packer>          packer;
  std::unique_ptr<sctp_network_gateway> gateway;
  sctp_network_gateway_controller*      gateway_ctrl_handler;
  sctp_network_gateway_data_handler*    gateway_data_handler;
  e2_message_handler*                   e2ap_msg_handler;
  e2_event_handler*                     event_handler;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("SCTP-GW");
};

}; // namespace srsran
