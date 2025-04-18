/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "du_cell_manager.h"
#include "srsran/du/du_cell_config_validation.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_du;

du_cell_manager::du_cell_manager(const du_manager_params& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void du_cell_manager::add_cell(const du_cell_config& cell_cfg)
{
  // Verify that DU cell configuration is valid. Abort application otherwise.
  auto ret = is_du_cell_config_valid(cell_cfg);
  if (not ret.has_value()) {
    report_error("ERROR: Invalid DU Cell Configuration. Cause: {}.\n", ret.error());
  }

  // Save config.
  cells.emplace_back(std::make_unique<cell_t>());
  cells.back()->cfg    = cell_cfg;
  cells.back()->active = false;
}

async_task<void> du_cell_manager::start(du_cell_index_t cell_index)
{
  return launch_async([this, cell_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Start called for a cell that does not exist.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN();
    }
    if (cells[cell_index]->active) {
      logger.warning("cell={}: Start called for an already active cell.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN();
    }

    // Start cell in the MAC.
    CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(cell_index).start());

    cells[cell_index]->active = true;

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::stop(du_cell_index_t cell_index)
{
  return launch_async([this, cell_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Stop called for a cell that does not exist.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN();
    }
    if (not cells[cell_index]->active) {
      // Ignore.
      CORO_EARLY_RETURN();
    }

    cells[cell_index]->active = false;

    // Stop cell in the MAC.
    CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(cell_index).stop());

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::stop()
{
  return launch_async([this, i = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    for (; i != cells.size(); ++i) {
      if (cells[i] != nullptr and cells[i]->active) {
        cells[i]->active = false;

        CORO_AWAIT(cfg.mac.cell_mng.get_cell_controller(to_du_cell_index(i)).stop());
      }
    }

    CORO_RETURN();
  });
}

du_cell_index_t du_cell_manager::get_cell_index(nr_cell_global_id_t nr_cgi) const
{
  du_cell_index_t cell_index = du_cell_index_t::INVALID_DU_CELL_INDEX;
  for (unsigned i = 0, e = nof_cells(); i != e; ++i) {
    const du_cell_config& cell_it = get_cell_cfg(to_du_cell_index(i));
    if (cell_it.nr_cgi == nr_cgi) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  return cell_index;
}

du_cell_index_t du_cell_manager::get_cell_index(pci_t pci) const
{
  du_cell_index_t cell_index = du_cell_index_t::INVALID_DU_CELL_INDEX;
  for (unsigned i = 0, e = nof_cells(); i != e; ++i) {
    const du_cell_config& cell_it = get_cell_cfg(to_du_cell_index(i));
    if (cell_it.pci == pci) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  return cell_index;
}
