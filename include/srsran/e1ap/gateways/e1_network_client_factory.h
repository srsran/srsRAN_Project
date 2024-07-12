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

#include "srsran/e1ap/gateways/e1_connection_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

struct e1_du_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_connector_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// \brief Create an E1 gateway connector that the CU-UP can use to connect to the CU-CP.
std::unique_ptr<srs_cu_up::e1_connection_client> create_e1_gateway_client(const e1_du_sctp_gateway_config& params);

} // namespace srsran
