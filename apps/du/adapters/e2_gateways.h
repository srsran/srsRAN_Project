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

#include "apps/services/e2/e2_appconfig.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"

namespace srsran {

/// Instantiates an E2AP client.
std::unique_ptr<e2_connection_client>
create_du_e2_client_gateway(const e2_appconfig& e2_cfg, io_broker& broker, dlt_pcap& f1ap_pcap)
{
  sctp_network_connector_config e2ap_sctp{};
  e2ap_sctp.if_name         = "E2AP";
  e2ap_sctp.dest_name       = "Near-RT-RIC";
  e2ap_sctp.connect_address = e2_cfg.ip_addr;
  e2ap_sctp.connect_port    = e2_cfg.port;
  e2ap_sctp.ppid            = E2_DU_PPID;
  e2ap_sctp.bind_address    = e2_cfg.bind_addr;

  return create_e2_gateway_client(e2_sctp_gateway_config{e2ap_sctp, broker, f1ap_pcap});
}

} // namespace srsran
