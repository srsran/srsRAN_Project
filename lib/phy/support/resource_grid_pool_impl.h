/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/srsvec/aligned_vec.h"
#include <vector>

namespace srsran {

class resource_grid_pool_impl : public resource_grid_pool
{
private:
  unsigned nof_slots;
  unsigned nof_sectors;

  std::vector<std::unique_ptr<resource_grid>> grids;

public:
  resource_grid_pool_impl(unsigned                                      nof_sectors_,
                          unsigned                                      nof_slots_,
                          std::vector<std::unique_ptr<resource_grid>>&& grids_);
  resource_grid& get_resource_grid(const resource_grid_context& context) override;
};
} // namespace srsran
