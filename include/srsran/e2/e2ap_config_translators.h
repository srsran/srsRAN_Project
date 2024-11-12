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

#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"

namespace srsran {

/// Retuns the E2 SCTP gateway configuration from the given parameters.
inline e2_sctp_gateway_config
generate_e2_client_gateway_config(const e2_config& e2_cfg, io_broker& broker, dlt_pcap& f1ap_pcap, uint16_t ppid)
{
  sctp_network_connector_config e2ap_sctp{};
  e2ap_sctp.if_name         = "E2AP";
  e2ap_sctp.dest_name       = "Near-RT-RIC";
  e2ap_sctp.connect_address = e2_cfg.ip_addr;
  e2ap_sctp.connect_port    = e2_cfg.port;
  e2ap_sctp.ppid            = ppid;
  e2ap_sctp.bind_address    = e2_cfg.bind_addr;

  return {e2ap_sctp, broker, f1ap_pcap};
}

} // namespace srsran