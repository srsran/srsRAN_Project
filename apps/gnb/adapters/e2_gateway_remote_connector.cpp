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

#include "e2_gateway_remote_connector.h"
#include "../adapters/e2ap_adapter.h"
#include "srsran/e2/e2.h"
#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {

e2_gateway_remote_connector::e2_gateway_remote_connector(io_broker&                   broker_,
                                                         sctp_network_gateway_config& net_gw_config_,
                                                         dlt_pcap&                    e2ap_pcap_writer_) :
  logger(srslog::fetch_basic_logger("E2AP")),
  broker(broker_),
  net_gw_config(net_gw_config_),
  e2ap_pcap_writer(e2ap_pcap_writer_)
{
}

std::unique_ptr<e2_message_notifier> e2_gateway_remote_connector::handle_connection_request()
{
  // Create E2AP and SCTP network adapter for the E2 Agent
  logger.info("Connecting to E2 Agent RIC ({})..", net_gw_config.connect_address, net_gw_config.connect_port);
  std::unique_ptr<e2ap_network_adapter> e2ap_adapter = std::make_unique<e2ap_network_adapter>(broker, e2ap_pcap_writer);

  std::unique_ptr<sctp_network_gateway> sctp_du_e2_gateway =
      create_sctp_network_gateway({net_gw_config, *e2ap_adapter, *e2ap_adapter});

  // Connect E2AP adapter to SCTP network gateway.
  e2ap_adapter->connect_gateway(std::move(sctp_du_e2_gateway));
  logger.info("E2 connection established");

  return e2ap_adapter;
}

void e2_gateway_remote_connector::connect_e2ap(e2_message_notifier* e2_rx_pdu_notifier,
                                               e2_message_handler*  e2ap_msg_handler_,
                                               e2_event_handler*    event_handler_)
{
  e2ap_network_adapter* e2ap_adapter = dynamic_cast<e2ap_network_adapter*>(e2_rx_pdu_notifier);
  e2ap_adapter->connect_e2ap(e2ap_msg_handler_, event_handler_);
  e2ap_notifiers.push_back(e2_rx_pdu_notifier);
}

void e2_gateway_remote_connector::close()
{
  logger.info("Closing E2 network connections...");
  for (auto& e2ap_notifier : e2ap_notifiers) {
    e2ap_network_adapter* e2ap_adapter = dynamic_cast<e2ap_network_adapter*>(e2ap_notifier);
    if (e2ap_adapter) {
      e2ap_adapter->disconnect_gateway();
    }
  }
  logger.info("E2 Network connections closed successfully");
}

} // namespace srsran
