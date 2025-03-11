/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../config/cell_configuration.h"

namespace srsran {

class csi_rs_scheduler
{
public:
  explicit csi_rs_scheduler(const cell_configuration& cell_cfg);

  void run_slot(cell_slot_resource_allocator& res_grid);

private:
  const cell_configuration& cell_cfg;

  std::vector<csi_rs_info> cached_csi_rs;
};

} // namespace srsran
