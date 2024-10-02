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

#include "e2_connection_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

struct e2_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_connector_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the E2AP messages.
  dlt_pcap& pcap;
};

/// \brief Create an E2AP gateway connector that the DU can use to connect to the Near-RT RIC.
std::unique_ptr<e2_connection_client> create_e2_gateway_client(const e2_sctp_gateway_config& params);

} // namespace srsran