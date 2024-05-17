/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  explicit e2_gateway_remote_connector(io_broker&                     broker_,
                                       sctp_network_connector_config& net_gw_config_,
                                       dlt_pcap&                      e2ap_pcap_writer_);

  // E2 Agent interface.
  std::unique_ptr<e2_message_notifier> handle_connection_request() override;

  void connect_e2ap(e2_message_notifier* e2_rx_pdu_notifier,
                    e2_message_handler*  e2ap_msg_handler_,
                    e2_event_handler*    event_handler_) override;

  void close() override;

private:
  srslog::basic_logger&             logger;
  io_broker&                        broker;
  sctp_network_connector_config&    net_gw_config;
  dlt_pcap&                         e2ap_pcap_writer;
  std::vector<e2_message_notifier*> e2ap_notifiers;
};

} // namespace srsran
