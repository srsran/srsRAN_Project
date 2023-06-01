/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../ue_scheduling/ue.h"

namespace srsran {

class cell_configuration;
struct cell_resource_allocator;
struct cell_slot_resource_allocator;

/// This class implements a scheduler that pre-reserve the PUCCH resources' REs on the scheduler grid, to prevent the
/// PUSCH allocator from using them. The pre-reserved REs are hereby referred as PUCCH guardbands.
class pucch_guardbands_scheduler
{
public:
  explicit pucch_guardbands_scheduler(const cell_configuration& cell_cfg_);

  /// \brief Runs the PUCCH guardbands scheduler.
  ///
  /// When called for the first time, this function pre-reserves the PUCCH's REs over the entire grid, until the last
  /// (farthest in the future) usable slot. From the second time onwards, the scheduler pre-reserves the PUCCH for the
  /// last usable slot of the grid.
  /// \param[out,in] res_alloc resource grid.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// Pre-allocates the PUCCH guardband on the resource grid for a given slot.
  void allocate_pucch_guardbands(cell_slot_resource_allocator& slot_alloc);

  const cell_configuration& cell_cfg;

  /// Flag indicating whether \c run_slot is called for the first time or not.
  bool first_run_slot{true};

  srslog::basic_logger& logger;
};

} // namespace srsran
