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
