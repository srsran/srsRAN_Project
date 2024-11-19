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

#include "srsran/f1ap/gateways/f1c_network_client_factory.h"

namespace srsran {

/// Instantiates an F1-C DU client.
std::unique_ptr<srs_du::f1c_connection_client> create_f1c_client_gateway(const std::string& cu_cp_addr,
                                                                         const std::string& bind_addr,
                                                                         io_broker&         broker,
                                                                         task_executor&     io_rx_executor,
                                                                         dlt_pcap&          f1ap_pcap)
{
  sctp_network_connector_config f1c_sctp{};
  f1c_sctp.if_name         = "F1-C";
  f1c_sctp.dest_name       = "CU-CP";
  f1c_sctp.connect_address = cu_cp_addr;
  f1c_sctp.connect_port    = F1AP_PORT;
  f1c_sctp.ppid            = F1AP_PPID;
  f1c_sctp.bind_address    = bind_addr;
  return create_f1c_gateway_client(f1c_du_sctp_gateway_config{f1c_sctp, broker, io_rx_executor, f1ap_pcap});
}

} // namespace srsran
