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
#include "dpdk_ethernet_receiver.h"
#include "dpdk_ethernet_transmitter.h"

using namespace srsran;
using namespace ether;

std::unique_ptr<gateway> srsran::ether::create_dpdk_gateway(const gw_config& config, srslog::basic_logger& logger)
{
  return std::make_unique<dpdk_transmitter_impl>(config, logger);
}

std::unique_ptr<receiver>
srsran::ether::create_dpdk_receiver(task_executor& executor, frame_notifier& notifier, srslog::basic_logger& logger)
{
  return std::make_unique<dpdk_receiver_impl>(executor, notifier, logger);
}
