/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/resource_grid_pool.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <vector>

namespace srsgnb {

class resource_grid_pool_impl : public resource_grid_pool
{
private:
  unsigned nof_slots;
  unsigned nof_sectors;

  std::vector<std::unique_ptr<resource_grid> > grids;

public:
  resource_grid_pool_impl(resource_grid_pool_config& config);
  resource_grid& get_resource_grid(const resource_grid_context& context) override;
};
} // namespace srsgnb
