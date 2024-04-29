/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gtpu/gtpu_tunnel_nru_factory.h"
#include "gtpu_tunnel_impl_nru.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;

std::unique_ptr<gtpu_tunnel_nru> srsran::create_gtpu_tunnel_nru(gtpu_tunnel_nru_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_nru_impl>(
      msg.ue_index, msg.cfg, *msg.gtpu_pcap, *msg.rx_lower, *msg.tx_upper, msg.ue_dl_timer_factory);
}
