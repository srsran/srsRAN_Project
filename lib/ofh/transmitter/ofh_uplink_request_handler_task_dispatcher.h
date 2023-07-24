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

#include "srsran/ofh/transmitter/ofh_uplink_request_handler.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul uplink request handler task dispatcher implementation.
class uplink_request_handler_task_dispatcher : public uplink_request_handler
{
public:
  uplink_request_handler_task_dispatcher(std::unique_ptr<uplink_request_handler> ul_handler_,
                                         task_executor&                          executor_) :
    ul_handler(std::move(ul_handler_)), executor(executor_)
  {
    srsran_assert(ul_handler, "Invalid uplink request handler");
  }

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override
  {
    executor.execute([this, context, &buffer]() { ul_handler->handle_prach_occasion(context, buffer); });
  }

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) override
  {
    executor.execute([this, context, &grid]() { ul_handler->handle_new_uplink_slot(context, grid); });
  }

private:
  std::unique_ptr<uplink_request_handler> ul_handler;
  task_executor&                          executor;
};

} // namespace ofh
} // namespace srsran
