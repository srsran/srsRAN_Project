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

#include "srsran/ofh/transmitter/ofh_downlink_handler.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul downlink handler task dispatcher implementation.
class downlink_handler_task_dispatcher : public downlink_handler
{
public:
  downlink_handler_task_dispatcher(std::unique_ptr<downlink_handler> dl_handler_, task_executor& executor_) :
    dl_handler(std::move(dl_handler_)), executor(executor_)
  {
    srsran_assert(dl_handler, "Invalid downlink handler");
  }

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    executor.execute([this, context, &grid]() { dl_handler->handle_dl_data(context, grid); });
  }

private:
  std::unique_ptr<downlink_handler> dl_handler;
  task_executor&                    executor;
};

} // namespace ofh
} // namespace srsran
