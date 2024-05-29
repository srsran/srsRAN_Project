/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/gtpu/ngu_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include <cstdint>

namespace srsran::srs_cu_up {

struct f1u_cu_up_split_gateway_creation_msg {
  ngu_gateway* udp_gw;
  gtpu_demux*  demux;
  dlt_pcap&    gtpu_pcap;
  uint16_t     peer_port;
};

std::unique_ptr<srsran::f1u_cu_up_udp_gateway> create_split_f1u_gw(f1u_cu_up_split_gateway_creation_msg msg);

} // namespace srsran::srs_cu_up
