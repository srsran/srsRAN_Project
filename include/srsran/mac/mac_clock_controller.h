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

#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point_extended.h"
#include <memory>

namespace srsran {

class timer_manager;

/// Interface used by MAC to report new slot indications and deactivations for a given cell.
class mac_cell_clock_controller
{
public:
  virtual ~mac_cell_clock_controller() = default;

  /// Called on each new slot indication for a given cell.
  void on_slot_indication(slot_point sl_tx) { cached_now = do_on_slot_indication(sl_tx); }

  /// Called when a cell is deactivated.
  virtual void on_cell_deactivation() = 0;

  /// Get the latest slot.
  slot_point_extended now() const { return cached_now; }

protected:
  /// Called on each new slot indication for a given cell.
  virtual slot_point_extended do_on_slot_indication(slot_point sl_tx) = 0;

  slot_point_extended cached_now;
};

class mac_clock_controller
{
public:
  virtual ~mac_clock_controller() = default;

  /// Creates a new cell, which will be used to notify new slot indications and cell deactivations.
  virtual std::unique_ptr<mac_cell_clock_controller> add_cell(du_cell_index_t cell_index) = 0;

  /// Retrieves repository of unique_timers for the DU-high.
  virtual timer_manager& get_timer_manager() = 0;
};

} // namespace srsran
