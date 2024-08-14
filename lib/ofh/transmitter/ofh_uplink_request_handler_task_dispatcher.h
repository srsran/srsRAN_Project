/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    if (!executor.execute([this, context, &buffer]() { ul_handler->handle_prach_occasion(context, buffer); })) {
      srslog::fetch_basic_logger("OFH").warning("Failed to dispatch PRACH occasion for slot '{}'", context.slot);
    }
  }

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override
  {
    if (!executor.execute([this, context, rg = grid.copy()]() { ul_handler->handle_new_uplink_slot(context, rg); })) {
      srslog::fetch_basic_logger("OFH").warning("Failed to dispatch new uplink slot for slot '{}'", context.slot);
    }
  }

private:
  std::unique_ptr<uplink_request_handler> ul_handler;
  task_executor&                          executor;
};

} // namespace ofh
} // namespace srsran
