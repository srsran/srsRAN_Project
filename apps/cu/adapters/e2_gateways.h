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

#include "apps/services/e2/e2_appconfig.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"

namespace srsran {

/// Instantiates an E2AP client.
std::unique_ptr<e2_connection_client>
create_cu_e2_client_gateway(const e2_appconfig& e2_cfg, io_broker& broker, dlt_pcap& f1ap_pcap)
{
  sctp_network_connector_config e2ap_sctp{};
  e2ap_sctp.if_name         = "E2AP";
  e2ap_sctp.dest_name       = "Near-RT-RIC";
  e2ap_sctp.connect_address = e2_cfg.ip_addr;
  e2ap_sctp.connect_port    = e2_cfg.port;
  e2ap_sctp.ppid            = E2_CP_PPID;
  e2ap_sctp.bind_address    = e2_cfg.bind_addr;
  return create_e2_gateway_client(e2_sctp_gateway_config{e2ap_sctp, broker, f1ap_pcap});
}

} // namespace srsran
