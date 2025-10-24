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

#include "positioning_handler.h"
#include "srsran/support/async/async_no_op_task.h"

#ifndef SRSRAN_HAS_ENTERPRISE

using namespace srsran;

namespace {

/// Positioning handler when feature is not available.
class disabled_positioning_handler final : public positioning_handler
{
public:
  class disabled_cell_positioning_handler final : public cell_positioning_handler
  {
    void handle_srs_indication(const mac_srs_indication_message& msg) override
    {
      // do nothing.
    }
  };

  std::unique_ptr<cell_positioning_handler> add_cell(du_cell_index_t cell_index) override
  {
    return std::make_unique<disabled_cell_positioning_handler>();
  }

  async_task<mac_positioning_measurement_response>
  handle_positioning_measurement_request(const mac_positioning_measurement_request& req) override
  {
    return launch_no_op_task(mac_positioning_measurement_response{});
  }
};

} // namespace

std::unique_ptr<positioning_handler> srsran::create_positioning_handler(scheduler_positioning_handler& sched,
                                                                        task_executor&                 ctrl_exec,
                                                                        timer_manager&                 timers,
                                                                        srslog::basic_logger&          logger)
{
  return std::make_unique<disabled_positioning_handler>();
}

#endif
