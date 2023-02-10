/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/cell_configuration.h"
#include "../cell/resource_grid.h"

namespace srsgnb {

class csi_rs_scheduler
{
public:
  explicit csi_rs_scheduler(const cell_configuration& cell_cfg);

  void run_slot(cell_slot_resource_allocator& res_grid);

private:
  const cell_configuration& cell_cfg;

  const optional<const csi_rs_info> cached_csi_rs;
};

} // namespace srsgnb