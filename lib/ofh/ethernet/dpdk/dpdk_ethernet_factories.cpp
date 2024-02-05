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
  port_cfg.pcie_id                     = config.interface;
  port_cfg.is_promiscuous_mode_enabled = config.is_promiscuous_mode_enabled;
  port_cfg.mtu_size                    = config.mtu_size;

  auto ctx = dpdk_port_context::create(port_cfg);
  return {std::make_unique<dpdk_transmitter_impl>(ctx, logger),
          std::make_unique<dpdk_receiver_impl>(rx_executor, ctx, logger)};
}
