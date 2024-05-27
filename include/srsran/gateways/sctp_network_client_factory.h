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

#include "srsran/gateways/sctp_network_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

/// Configuration of an SCTP client.
struct sctp_network_client_config {
  std::string                 client_name;
  sctp_network_gateway_config sctp;
  io_broker&                  broker;
};

/// Creates an SCTP network client.
std::unique_ptr<sctp_network_client> create_sctp_network_client(const sctp_network_client_config& config);

} // namespace srsran