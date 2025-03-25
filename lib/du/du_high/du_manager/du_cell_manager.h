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

#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/ran/du_types.h"

namespace srsran {
namespace srs_du {

struct du_cell_param_config_request;

/// Cached packed System Information being currently sent by the cell.
struct du_cell_packed_sys_info {
  byte_buffer              sib1;
  std::vector<byte_buffer> si_messages;
};

/// Current DU cell context.
struct du_cell_context {
  /// Current configuration.
  du_cell_config cfg;
  /// System Information being signalled by the cell.
  du_cell_packed_sys_info packed_sys_info;
  /// Whether the cell is active.
  bool active = false;
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
    return cells[cell_index]->active;
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

  /// Handle request to update a cell configuration.
  bool handle_cell_reconf_request(const du_cell_param_config_request& req);

  const du_cell_packed_sys_info& get_packed_sys_info(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->packed_sys_info;
  }

  async_task<bool> start(du_cell_index_t cell_index);

  async_task<void> stop(du_cell_index_t cell_index);

  async_task<void> stop();

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
