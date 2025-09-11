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

#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/ran/du_types.h"
#include <vector>

namespace srsran {

class mac_test_mode_event_handler
{
  static constexpr unsigned queue_size = 128U;

  using cell_event_queue =
      concurrent_queue<unique_task, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

public:
  mac_test_mode_event_handler(unsigned nof_cells)
  {
    cells.reserve(nof_cells);
    for (unsigned i = 0, e = nof_cells; i < e; ++i) {
      cells.emplace_back(std::make_unique<cell_event_queue>(queue_size));
    }
  }

  bool schedule(du_cell_index_t cell_index, unique_task t)
  {
    srsran_assert(cell_index < cells.size(), "Invalid cell index {}", fmt::underlying(cell_index));
    return cells[cell_index]->try_push(std::move(t));
  }

  void process_pending_tasks(du_cell_index_t cell_index)
  {
    srsran_assert(cell_index < cells.size(), "Invalid cell index {}", fmt::underlying(cell_index));
    auto& c = *cells[cell_index];

    unique_task task;
    while (c.try_pop(task)) {
      task();
    }
  }

private:
  std::vector<std::unique_ptr<cell_event_queue>> cells;
};

} // namespace srsran
