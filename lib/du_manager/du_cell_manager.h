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

#include "srsran/du_manager/du_manager_params.h"
#include "srsran/ran/du_types.h"

namespace srsran {
namespace srs_du {

class du_cell_manager
{
public:
  explicit du_cell_manager(const du_manager_params& params_);

  size_t nof_cells() const { return cells.size(); }

  bool has_cell(du_cell_index_t cell_index) const { return cell_index < cells.size(); }

  bool is_cell_active(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->active;
  }

  void set_cell_state(du_cell_index_t cell_index, bool active)
  {
    assert_cell_exists(cell_index);
    cells[cell_index]->active = active;
  }

  const du_cell_config& get_cell_cfg(du_cell_index_t cell_index) const
  {
    assert_cell_exists(cell_index);
    return cells[cell_index]->cfg;
  }

private:
  struct cell_t {
    bool           active = false;
    du_cell_config cfg;
  };

  void assert_cell_exists(du_cell_index_t cell_index) const
  {
    srsran_assert(has_cell(cell_index), "cell_index={} does not exist", cell_index);
  }

  void add_cell(const du_cell_config& cfg);

  const du_manager_params& cfg;

  std::vector<std::unique_ptr<cell_t>> cells;
};

} // namespace srs_du
} // namespace srsran
