/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_factory_impl.h"
#include "downlink_processor_factory_impl.h"
#include "srsgnb/phy/upper/downlink_processor_factory.h"
#include "upper_phy_impl.h"

using namespace srsgnb;

static std::unique_ptr<downlink_processor_pool> build_dl_processor_pool(const upper_phy_config& config)
{
  downlink_processor_single_executor_factory factory(*config.gateway, *config.executor);

  downlink_processor_pool_config config_pool;
  config_pool.num_sectors = 1;
  // :TODO: Change how to manage the numerologies in the future.
  downlink_processor_pool_config::info info = {0, 0, {}};

  for (unsigned i = 0, e = config.nof_dl_processors; i != e; ++i) {
    std::unique_ptr<downlink_processor> dl_proc = factory.create({});
    srsran_assert(dl_proc, "Invalid downlink processor");
    info.procs.push_back(std::move(dl_proc));
  }

  config_pool.dl_processors.push_back(std::move(info));

  return create_dl_processor_pool(std::move(config_pool));
}

static std::unique_ptr<resource_grid_pool> build_dl_resource_grid_pool(const upper_phy_config& config)
{
  resource_grid_pool_config rg_pool_config = {};
  // Configure one pool per upper PHY.
  rg_pool_config.nof_sectors = 1;
  rg_pool_config.nof_slots   = config.nof_slots;
  for (unsigned sector_idx = 0; sector_idx != rg_pool_config.nof_sectors; ++sector_idx) {
    for (unsigned slot_id = 0; slot_id != rg_pool_config.nof_slots; ++slot_id) {
      rg_pool_config.grids.push_back(create_resource_grid(config.nof_ports, MAX_NSYMB_PER_SLOT, config.bw_rb * NRE));
    }
  }

  return create_resource_grid_pool(rg_pool_config);
}

std::unique_ptr<upper_phy> upper_phy_factory_impl::create(const upper_phy_config& config)
{
  std::unique_ptr<resource_grid_pool> rg_pool = build_dl_resource_grid_pool(config);
  srsran_assert(rg_pool, "Invalid resource grid pool");
  std::unique_ptr<downlink_processor_pool> dl_pool = build_dl_processor_pool(config);
  srsran_assert(dl_pool, "Invalid downlink processor pool");

  return std::make_unique<upper_phy_impl>(config.sector_id, std::move(dl_pool), std::move(rg_pool));
}

std::unique_ptr<upper_phy_factory> srsgnb::create_upper_phy_factory()
{
  return std::make_unique<upper_phy_factory_impl>();
}
