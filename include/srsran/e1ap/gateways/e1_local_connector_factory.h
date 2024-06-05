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
#include "srsran/e1ap/gateways/e1_connection_server.h"

namespace srsran {

class dlt_pcap;

class e1_local_connector : public srs_cu_up::e1_connection_client, public srs_cu_cp::e1_connection_server
{};

struct e1_local_connector_config {
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// Creates a local connector between CU-CP and CU-UP E1 interfaces, avoiding the need to pack/unpack the exchanged
/// E1AP PDUs or any socket send/recv.
std::unique_ptr<e1_local_connector> create_e1_local_connector(const e1_local_connector_config& cfg);

} // namespace srsran