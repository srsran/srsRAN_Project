/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/f1ap/gateways/f1c_network_client_factory.h"

namespace srsran {

/// Instantiates an F1-C DU client.
inline std::unique_ptr<srs_du::f1c_connection_client> create_f1c_client_gateway(const std::string& cu_cp_addr,
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
