/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
