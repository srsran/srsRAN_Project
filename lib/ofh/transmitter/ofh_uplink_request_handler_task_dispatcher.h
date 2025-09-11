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

#include "ofh_uplink_request_handler_impl.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/synchronization/stop_event.h"

namespace srsran {
namespace ofh {

/// Uplink request handler task dispatcher.
class uplink_request_handler_task_dispatcher : public uplink_request_handler, operation_controller
{
  const unsigned          sector_id;
  srslog::basic_logger&   logger;
  uplink_request_handler& uplink_handler;
  task_executor&          executor;
  stop_event_source       stop_manager;

public:
  uplink_request_handler_task_dispatcher(unsigned                sector_id_,
                                         srslog::basic_logger&   logger_,
                                         uplink_request_handler& uplink_handler_,
                                         task_executor&          executor_) :
    sector_id(sector_id_), logger(logger_), uplink_handler(uplink_handler_), executor(executor_)
  {
  }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;
};

} // namespace ofh
} // namespace srsran
