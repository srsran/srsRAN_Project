/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_cell_manager.h"
#include "srsran/du/du_cell_config_validation.h"

using namespace srsran;
using namespace srs_du;

du_cell_manager::du_cell_manager(const du_manager_params& cfg_) : cfg(cfg_)
{
  for (const du_cell_config& cell_cfg : cfg.ran.cells) {
    add_cell(cell_cfg);
  }
}

void du_cell_manager::add_cell(const du_cell_config& cell_cfg)
{
  // Verify that DU cell configuration is valid. Abort application otherwise.
  auto ret = is_du_cell_config_valid(cell_cfg);
  if (ret.is_error()) {
    report_error("ERROR: Invalid DU Cell Configuration. Cause: {}.", ret.error());
  }

  cells.emplace_back(std::make_unique<cell_t>());
  cells.back()->cfg = cell_cfg;
}
