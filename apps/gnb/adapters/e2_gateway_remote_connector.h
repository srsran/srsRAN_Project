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

#include "srsran/e2/e2_connection_client.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/support/io/io_broker.h"

namespace srsran {

class dlt_pcap;

/// Implementation of a E2 Agent and RIC gateway for the case of socket-based communication.
class e2_gateway_remote_connector final : public e2_connection_client
{
public:
  explicit e2_gateway_remote_connector(io_broker&                   broker_,
                                       sctp_network_gateway_config& net_gw_config_,
                                       dlt_pcap&                    e2ap_pcap_writer_);

  // E2 Agent interface.
  std::unique_ptr<e2_message_notifier> handle_connection_request() override;

  void connect_e2ap(e2_message_notifier* e2_rx_pdu_notifier,
                    e2_message_handler*  e2ap_msg_handler_,
                    e2_event_handler*    event_handler_) override;

  void close() override;

private:
  srslog::basic_logger&             logger;
  io_broker&                        broker;
  sctp_network_gateway_config&      net_gw_config;
  dlt_pcap&                         e2ap_pcap_writer;
  std::vector<e2_message_notifier*> e2ap_notifiers;
};

} // namespace srsran
