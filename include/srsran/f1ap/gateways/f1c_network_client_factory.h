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

#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;
class task_executor;

struct f1c_du_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_connector_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
};

/// \brief Create an F1-C gateway connector that the DU can use to connect to the CU-CP.
std::unique_ptr<srs_du::f1c_connection_client> create_f1c_gateway_client(const f1c_du_sctp_gateway_config& params);

} // namespace srsran
