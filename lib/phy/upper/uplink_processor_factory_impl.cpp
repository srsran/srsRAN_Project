/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uplink_processor_factory_impl.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/support/error_handling.h"
#include "uplink_processor_pool_impl.h"
#include "uplink_processor_single_executor_impl.h"

using namespace srsgnb;

uplink_processor_single_executor_factory::uplink_processor_single_executor_factory(
    std::shared_ptr<prach_detector_factory> prach_factory_,
    task_executor&                          executor,
    upper_phy_rx_results_notifier&          results_notifier) :
  prach_factory(prach_factory_), executor(executor), results_notifier(results_notifier)
{
  report_fatal_error_if_not(prach_factory, "Invalid PRACH factory.");
}

std::unique_ptr<uplink_processor>
uplink_processor_single_executor_factory::create(const uplink_processor_config& config)
{
  std::unique_ptr<prach_detector> detector = prach_factory->create();
  report_fatal_error_if_not(detector, "Invalid PRACH detector.");

  return std::make_unique<uplink_processor_single_executor_impl>(std::move(detector), executor, results_notifier);
}

std::unique_ptr<uplink_processor_pool> srsgnb::create_uplink_processor_pool(uplink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  uplink_processor_pool_impl_config ul_processors;
  ul_processors.num_sectors = config.num_sectors;

  for (auto& proc : config.ul_processors) {
    ul_processors.procs.push_back({proc.sector, proc.numerology, std::move(proc.procs)});
  }

  return std::make_unique<uplink_processor_pool_impl>(std::move(ul_processors));
}
