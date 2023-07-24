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

#pragma once

#include "lib/ngap/ngap_asn1_packer.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/io/io_broker.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief NGAP bridge that uses the IO broker to handle the SCTP connection
class ngap_network_adapter : public ngap_message_notifier,
                             public ngap_message_handler,
                             public sctp_network_gateway_control_notifier,
                             public network_gateway_data_notifier
{
public:
  ngap_network_adapter(io_broker& broker_, dlt_pcap& pcap_) : broker(broker_), pcap(pcap_)
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

    if (!gateway_ctrl_handler->create_and_connect()) {
      report_error("Failed to create SCTP gateway.\n");
    }
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
  dlt_pcap&                          pcap;
  std::unique_ptr<ngap_asn1_packer>  packer;
  sctp_network_gateway_controller*   gateway_ctrl_handler = nullptr;
  sctp_network_gateway_data_handler* gateway_data_handler = nullptr;
  ngap_message_handler*              ngap_msg_handler     = nullptr;
  ngap_event_handler*                event_handler        = nullptr;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("SCTP-GW");
};

}; // namespace srs_cu_cp

}; // namespace srsran
