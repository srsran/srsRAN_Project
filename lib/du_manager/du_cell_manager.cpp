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

using namespace srsgnb;

du_cell_manager::du_cell_manager(const du_manager_config_t& cfg_) : cfg(cfg_)
{
  for (const du_cell_config& cell_cfg : cfg.du_cells) {
    add_cell(cell_cfg);
  }
}

void du_cell_manager::add_cell(const du_cell_config& cell_cfg)
{
  cells.emplace_back(std::make_unique<cell_t>());
  cells.back()->cfg = cell_cfg;
}
