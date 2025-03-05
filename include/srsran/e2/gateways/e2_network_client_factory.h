/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;
class task_executor;

struct e2_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_connector_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
  /// PCAP writer for the E2AP messages.
  dlt_pcap& pcap;
  /// Logger.
  srslog::basic_logger& logger;
};

/// \brief Create an E2AP gateway connector that the DU can use to connect to the Near-RT RIC.
std::unique_ptr<e2_connection_client> create_e2_gateway_client(const e2_sctp_gateway_config& params);

} // namespace srsran
