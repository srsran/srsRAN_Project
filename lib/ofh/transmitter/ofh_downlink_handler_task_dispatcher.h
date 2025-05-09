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

#include "ofh_downlink_handler_impl.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace ofh {

///
class downlink_handler_task_dispatcher : public downlink_handler
{
  const unsigned    sector_id;
  downlink_handler& dl_handler;
  task_executor&    executor;

public:
  downlink_handler_task_dispatcher(const unsigned sector_id_, downlink_handler& dl_handler_, task_executor& executor_) :
    sector_id(sector_id_), dl_handler(dl_handler_), executor(executor_)
  {
  }

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override;
};

} // namespace ofh
} // namespace srsran
