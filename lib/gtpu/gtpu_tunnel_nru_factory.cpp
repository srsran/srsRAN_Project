/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
  return std::make_unique<gtpu_tunnel_nru_impl>(msg.ue_index, msg.cfg, *msg.gtpu_pcap, *msg.rx_lower, *msg.tx_upper);
}

std::unique_ptr<gtpu_tunnel_common_rx_upper_layer_interface>
srsran::create_gtpu_tunnel_nru_rx(gtpu_tunnel_nru_rx_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_nru_rx_impl>(msg.ue_index, msg.rx_cfg, *msg.rx_lower);
}

std::unique_ptr<gtpu_tunnel_nru_tx_lower_layer_interface>
srsran::create_gtpu_tunnel_nru_tx(gtpu_tunnel_nru_tx_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_nru_tx_impl>(msg.ue_index, msg.tx_cfg, *msg.gtpu_pcap, *msg.tx_upper);
}
