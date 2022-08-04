/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/support/support_factories.h"
#include "prach_buffer_impl.h"
#include "prach_buffer_pool_impl.h"
#include "resource_grid_impl.h"
#include "resource_grid_pool_impl.h"

using namespace srsgnb;

std::unique_ptr<resource_grid> srsgnb::create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  return std::make_unique<resource_grid_impl>(nof_ports, nof_symbols, nof_subc);
}

std::unique_ptr<resource_grid_pool> srsgnb::create_resource_grid_pool(resource_grid_pool_config& config)
{
  return std::make_unique<resource_grid_pool_impl>(config);
}

std::unique_ptr<prach_buffer_pool>
srsgnb::create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements)
{
  return std::make_unique<prach_buffer_pool_impl>(std::move(elements));
}

std::unique_ptr<prach_buffer> srsgnb::create_prach_buffer_long()
{
  return std::make_unique<prach_buffer_static<839, 4>>();
}

std::unique_ptr<prach_buffer> srsgnb::create_prach_buffer_short()
{
  return std::make_unique<prach_buffer_static<139, 12>>();
}
