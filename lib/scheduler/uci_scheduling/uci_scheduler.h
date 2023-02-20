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

namespace srsran {

struct cell_resource_allocator;

/// UCI scheduling interface, which handles the scheduling of SR and CSI opportunities.
class uci_scheduler
{
public:
  virtual ~uci_scheduler() = default;

  /// Schedules the SR opportunities and CSI occasions.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] sl_tx slot for which the SR should be allocated.
  virtual void run_slot(cell_resource_allocator& res_alloc, slot_point sl_tx) = 0;
};

} // namespace srsran
