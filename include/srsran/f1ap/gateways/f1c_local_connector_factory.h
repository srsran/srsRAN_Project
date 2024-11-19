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

#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/f1ap/gateways/f1c_connection_server.h"

namespace srsran {

class dlt_pcap;
class io_broker;
class task_executor;

class f1c_local_connector : public srs_du::f1c_connection_client, public srs_cu_cp::f1c_connection_server
{};

struct f1c_local_connector_config {
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
};

/// Creates a local connector between CU-CP and DU F1-C interfaces, avoiding the need to pack/unpack the exchanged
/// F1AP PDUs or any socket send/recv.
std::unique_ptr<f1c_local_connector> create_f1c_local_connector(const f1c_local_connector_config& cfg);

struct f1c_local_sctp_connector_config {
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
  /// IO broker to handle the SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
};

/// Creates an F1-C local connector using an SCTP socket as channel.
///
/// Note: This class is useful for testing.
std::unique_ptr<f1c_local_connector> create_f1c_local_connector(const f1c_local_sctp_connector_config& cfg);

} // namespace srsran
