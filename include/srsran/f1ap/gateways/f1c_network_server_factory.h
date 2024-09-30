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

#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/f1ap/gateways/f1c_connection_server.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

/// Configuration of an SCTP-based F1-C Gateway.
struct f1c_cu_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
};

/// Creates an F1-C Gateway server that listens for incoming SCTP connections, packs/unpacks F1AP PDUs and forwards
/// them to the GW/CU-CP F1AP handler.
std::unique_ptr<srs_cu_cp::f1c_connection_server> create_f1c_gateway_server(const f1c_cu_sctp_gateway_config& params);

} // namespace srsran
