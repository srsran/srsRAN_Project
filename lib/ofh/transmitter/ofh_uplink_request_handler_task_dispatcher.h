/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ofh_uplink_request_handler_impl.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace ofh {

/// Uplink request handler task dispatcher.
class uplink_request_handler_task_dispatcher : public uplink_request_handler
{
  const unsigned          sector_id;
  uplink_request_handler& uplink_handler;
  task_executor&          executor;

public:
  uplink_request_handler_task_dispatcher(unsigned                sector_id_,
                                         uplink_request_handler& uplink_handler_,
                                         task_executor&          executor_) :
    sector_id(sector_id_), uplink_handler(uplink_handler_), executor(executor_)
  {
  }

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;
};

} // namespace ofh
} // namespace srsran
