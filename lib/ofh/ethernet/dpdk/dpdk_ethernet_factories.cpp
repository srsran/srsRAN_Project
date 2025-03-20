/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_factories.h"
#include "dpdk_ethernet_receiver.h"
#include "dpdk_ethernet_transmitter.h"
#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_gateway_config.h"

using namespace srsran;
using namespace ether;

std::shared_ptr<dpdk_port_context> srsran::ether::create_dpdk_port_context(const gw_config& config)
{
  dpdk_port_config port_cfg;
  port_cfg.id                           = config.interface;
  port_cfg.is_promiscuous_mode_enabled  = config.is_promiscuous_mode_enabled;
  port_cfg.is_link_status_check_enabled = config.is_link_status_check_enabled;
  port_cfg.mtu_size                     = config.mtu_size;

  return dpdk_port_context::create(port_cfg);
}

std::pair<std::unique_ptr<gateway>, std::unique_ptr<receiver>>
srsran::ether::create_dpdk_txrx(const gw_config& config, task_executor& rx_executor, srslog::basic_logger& logger)
{
  auto ctx = create_dpdk_port_context(config);

  return {std::make_unique<dpdk_transmitter_impl>(ctx, logger, config.are_metrics_enabled),
          std::make_unique<dpdk_receiver_impl>(rx_executor, ctx, logger, config.are_metrics_enabled)};
}
