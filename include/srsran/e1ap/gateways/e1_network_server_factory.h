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

#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/gateways/e1_connection_server.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

/// Configuration of an SCTP-based E1 Gateway.
struct e1_cu_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// Creates an E1 Gateway server that listens for incoming SCTP connections, packs/unpacks E1AP PDUs and forwards
/// them to the GW/CU-CP E1AP handler.
std::unique_ptr<srs_cu_cp::e1_connection_server> create_e1_gateway_server(const e1_cu_sctp_gateway_config& params);

} // namespace srsran
