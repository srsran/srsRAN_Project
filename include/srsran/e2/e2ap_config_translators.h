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

#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"

namespace srsran {

/// Retuns the E2 SCTP gateway configuration from the given parameters.
inline e2_sctp_gateway_config generate_e2_client_gateway_config(const e2_config& e2_cfg,
                                                                io_broker&       broker,
                                                                task_executor&   io_rx_executor,
                                                                dlt_pcap&        f1ap_pcap,
                                                                uint16_t         ppid)
{
  sctp_network_connector_config e2ap_sctp{};
  e2ap_sctp.if_name         = "E2AP";
  e2ap_sctp.dest_name       = "Near-RT-RIC";
  e2ap_sctp.connect_address = e2_cfg.ip_addr;
  e2ap_sctp.connect_port    = e2_cfg.port;
  e2ap_sctp.ppid            = ppid;
  e2ap_sctp.bind_address    = e2_cfg.bind_addr;

  const std::string logger_id = (ppid == E2_DU_PPID)   ? "E2-DU"
                                : (ppid == E2_UP_PPID) ? "E2-CU-UP"
                                : (ppid == E2_CP_PPID) ? "E2-CU-CP"
                                                       : "E2";
  return {e2ap_sctp, broker, io_rx_executor, f1ap_pcap, srslog::fetch_basic_logger(logger_id)};
}

} // namespace srsran
