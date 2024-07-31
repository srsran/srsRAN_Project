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
    io_broker&             broker;
    std::string            bind_address;
    std::string            bind_interface;
    std::string            amf_address;
    int                    amf_port = NGAP_PORT;
    std::optional<int32_t> rto_initial;
    std::optional<int32_t> rto_min;
    std::optional<int32_t> rto_max;
    std::optional<int32_t> init_max_attempts;
    std::optional<int32_t> max_init_timeo;
    std::optional<bool>    nodelay;
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
