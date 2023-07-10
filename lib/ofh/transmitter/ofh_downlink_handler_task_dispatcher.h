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
