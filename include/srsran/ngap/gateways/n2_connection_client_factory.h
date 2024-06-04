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

#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/ngap/gateways/n2_connection_client.h"
#include <variant>

namespace srsran {

class io_broker;
class dlt_pcap;

namespace srs_cu_cp {

/// Parameters for the NG gateway instantiation.
struct n2_connection_client_config {
  /// Parameters for a local AMF stub connection.
  struct no_core {};

  /// Parameters specific to an SCTP network gateway.
  struct network {
    io_broker&                            broker;
    srsran::sctp_network_connector_config sctp;
  };

  /// PCAP writer for the NGAP messages.
  dlt_pcap& pcap;

  /// Mode of operation.
  std::variant<no_core, network> mode;
};

/// Create an N2 connection client.
std::unique_ptr<n2_connection_client> create_n2_connection_client(const n2_connection_client_config& params);

} // namespace srs_cu_cp
} // namespace srsran