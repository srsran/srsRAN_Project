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

#pragma once

#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/scheduler_sys_info_handler.h"

namespace srsran {
namespace srs_du {

struct du_cell_param_config_request;

/// Current DU cell context.
struct du_cell_context {
  enum class state_t { active, inactive, deactivating };

  /// Current configuration.
  du_cell_config cfg;
  /// Encoded System Information being currently sent by the DU cell.
  mac_cell_sys_info_config si_cfg;
  /// Current cell state.
  state_t state;
};

/// Result of the reconfiguration of a DU cell.
struct du_cell_reconfig_result {
  /// Cell configured.
  du_cell_index_t cell_index;
  /// Whether the CU needs to be notified of the gNB-DU cell configuration update.
  bool cu_notif_required;
  /// Whether the Scheduler needs to be notified about an update in the SI scheduling.
  bool sched_notif_required;
};

class du_cell_manager
{
public:
  explicit du_cell_manager(const du_manager_params& params_);

  size_t nof_cells() const { return cells.size(); }

  bool has_cell(du_cell_index_t cell_index) const { return cell_index < cells.size(); }

  /// Determine whether cell is activated.
  bool is_cell_active(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->state == du_cell_context::state_t::active;
  }

  du_cell_index_t get_cell_index(nr_cell_global_id_t nr_cgi) const;

  du_cell_index_t get_cell_index(pci_t pci) const;

  void add_cell(const du_cell_config& cfg);

  const du_cell_config& get_cell_cfg(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->cfg;
  }
  du_cell_config& get_cell_cfg(du_cell_index_t cell_index)
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->cfg;
  }

  /// Stop accepting new UE creations in the given cell.
  void stop_accepting_ues(du_cell_index_t cell_index)
  {
    cells[cell_index]->state = du_cell_context::state_t::deactivating;
  }

  /// Handle request to update a cell configuration.
  /// \return true if a change was detected and applied.
  expected<du_cell_reconfig_result> handle_cell_reconf_request(const du_cell_param_config_request& req);

  /// Retrieve current cell system information configuration.
  const mac_cell_sys_info_config& get_sys_info(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->si_cfg;
  }

  /// Start a specific cell in the DU.
  async_task<bool> start(du_cell_index_t cell_index);

  /// Stop a specific cell in the DU.
  async_task<void> stop(du_cell_index_t cell_index);

  /// Stop all cells in the DU.
  async_task<void> stop_all();

private:
  void assert_cell_exists(du_cell_index_t cell_index) const
  {
    srsran_assert(has_cell(cell_index), "cell_index={} does not exist", fmt::underlying(cell_index));
  }

  const du_manager_params& cfg;
  srslog::basic_logger&    logger;

  std::vector<std::unique_ptr<du_cell_context>> cells;
};

} // namespace srs_du
} // namespace srsran
