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

#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/f1ap/cu_cp/f1c_connection_server.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

struct f1c_cu_gateway_params {
  dlt_pcap&                        pcap;
  io_broker&                       broker;
  srsran::sctp_network_node_config sctp;
};

std::unique_ptr<srs_cu_cp::f1c_connection_server> create_f1c_gateway_server(const f1c_cu_gateway_params& params);

} // namespace srsran