/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
