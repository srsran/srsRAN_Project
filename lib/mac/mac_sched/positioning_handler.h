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

#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_positioning_measurement_handler.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/timers.h"

namespace srsran {

class scheduler_positioning_handler;
class task_executor;

class cell_positioning_handler
{
public:
  virtual ~cell_positioning_handler() = default;

  /// Handle SRS indication from lower layers.
  virtual void handle_srs_indication(const mac_srs_indication_message& msg) = 0;
};

class positioning_handler : public mac_positioning_measurement_handler
{
public:
  virtual ~positioning_handler() = default;

  /// Create a cell handler.
  virtual std::unique_ptr<cell_positioning_handler> add_cell(du_cell_index_t cell_index) = 0;
};

std::unique_ptr<positioning_handler> create_positioning_handler(scheduler_positioning_handler& sched,
                                                                task_executor&                 ctrl_exec,
                                                                timer_manager&                 timers,
                                                                srslog::basic_logger&          logger);

} // namespace srsran
