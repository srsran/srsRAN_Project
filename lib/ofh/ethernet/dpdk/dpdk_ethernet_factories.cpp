/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dpdk_ethernet_factories.h"
#include "dpdk_ethernet_port_context.h"
#include "dpdk_ethernet_receiver.h"
#include "dpdk_ethernet_transmitter.h"
#include "srsran/ofh/ethernet/ethernet_gw_config.h"

using namespace srsran;
using namespace ether;

std::pair<std::unique_ptr<gateway>, std::unique_ptr<receiver>>
srsran::ether::create_dpdk_txrx(const gw_config& config, task_executor& rx_executor, srslog::basic_logger& logger)
{
  dpdk_port_config port_cfg;
  port_cfg.id                          = config.interface;
  port_cfg.is_promiscuous_mode_enabled = config.is_promiscuous_mode_enabled;
  port_cfg.mtu_size                    = config.mtu_size;

  auto ctx = dpdk_port_context::create(port_cfg);
  return {std::make_unique<dpdk_transmitter_impl>(ctx, logger),
          std::make_unique<dpdk_receiver_impl>(rx_executor, ctx, logger)};
}
