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

#include "../mac_config_interfaces.h"
#include "mac_scheduler_rach_handler.h"
#include "srsran/mac/mac.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief Handles the allocation of RNTIs and forwarding of RACH indications to scheduler.
/// This class is thread-safe.
class cell_rach_handler final : public mac_cell_rach_handler
{
public:
  explicit cell_rach_handler(du_cell_index_t cell_index_, mac_scheduler_rach_handler& sched_) :
    cell_index(cell_index_), sched(sched_)
  {
  }

  /// Handles detected PRACHs by allocating a temporary C-RNTI and signalling the scheduler to allocate an RAR.
  void handle_rach_indication(const mac_rach_indication& rach_ind) override
  {
    sched.handle_rach_indication(cell_index, rach_ind);
  }

private:
  const du_cell_index_t       cell_index;
  mac_scheduler_rach_handler& sched;
};

class rach_handler final : public rach_handler_configurator
{
public:
  rach_handler(mac_scheduler_rach_handler& sched_) : sched(sched_) {}

  void add_cell(du_cell_index_t cell_index) override
  {
    srsran_assert(not cells.contains(cell_index), "Cell {} already exists", cell_index);
    cells.emplace(cell_index, cell_index, sched);
  }

  void remove_cell(du_cell_index_t cell_index) override
  {
    srsran_assert(cells.contains(cell_index), "Cell {} already exists", cell_index);
    cells.erase(cell_index);
  }

  mac_cell_rach_handler& get_cell(du_cell_index_t cell_index)
  {
    srsran_assert(cells.contains(cell_index), "Cell index does not exist.");
    return cells[cell_index];
  }

private:
  mac_scheduler_rach_handler&                        sched;
  slotted_array<cell_rach_handler, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsran
