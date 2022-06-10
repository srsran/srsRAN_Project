/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_factory_impl.h"
#include "downlink_processor_pool_impl.h"
#include "downlink_processor_single_executor_impl.h"

using namespace srsgnb;

std::unique_ptr<downlink_processor>
downlink_processor_single_executor_factory::create(const downlink_processor_config& config)
{
  // :TODO: Implement me!!
  (void)gateway;
  (void)executor;
  return {nullptr};
}

std::unique_ptr<downlink_processor_pool> srsgnb::create_dl_processor_pool(downlink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  downlink_processor_pool_config_impl dl_processors;
  dl_processors.num_sectors = config.num_sectors;

  for (auto& proc : config.dl_processors) {
    dl_processors.procs.push_back({proc.sector, proc.numerology, std::move(proc.procs)});
  }

  return std::make_unique<downlink_processor_pool_impl>(std::move(dl_processors));
}
