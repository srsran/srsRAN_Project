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

#include "srsran/ran/du_types.h"
#include "srsran/scheduler/config/si_scheduling_config.h"
#include "srsran/support/units.h"

namespace srsran {

/// Identifier for the version of the system information scheduling information.
using si_version_type = unsigned;

/// Information relative to the update of a cell SIB1 or SI messages.
struct si_scheduling_update_request {
  /// Cell index specific to the update of the SI scheduling.
  du_cell_index_t cell_index;
  /// SI epoch counter, monotonically increasing with each update.
  si_version_type version;
  /// SIB1 payload length.
  units::bytes sib1_len;
  /// Configuration of SI message scheduling.
  std::optional<si_scheduling_config> si_sched_cfg;
};

/// Interface used to notify new SIB1 or SI message updates to the scheduler.
class scheduler_sys_info_handler
{
public:
  virtual ~scheduler_sys_info_handler() = default;

  /// Handle cell system information scheduling update.
  virtual void handle_si_update_request(const si_scheduling_update_request& req) = 0;
};

} // namespace srsran
