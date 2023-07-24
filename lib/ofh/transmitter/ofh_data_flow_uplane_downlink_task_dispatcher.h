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

#include "ofh_data_flow_uplane_downlink_data.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Task dispatcher entry.
struct data_flow_uplane_downlink_task_dispatcher_entry {
  data_flow_uplane_downlink_task_dispatcher_entry(std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane_,
                                                  task_executor&                                  executor_) :
    data_flow_uplane(std::move(data_flow_uplane_)), executor(executor_)
  {
    srsran_assert(data_flow_uplane, "Invalid data flow");
  }

  std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane;
  task_executor&                                  executor;
};

/// Open Fronthaul downlink User-Plane task dispatcher implementation.
class data_flow_uplane_downlink_task_dispatcher : public data_flow_uplane_downlink_data
{
public:
  data_flow_uplane_downlink_task_dispatcher(std::vector<data_flow_uplane_downlink_task_dispatcher_entry>&& config_) :
    dispatchers(std::move(config_))
  {
  }

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_resource_grid_context& context,
                                      const resource_grid_reader&            grid,
                                      unsigned                               eaxc) override
  {
    unsigned                                         index            = context.port % dispatchers.size();
    data_flow_uplane_downlink_task_dispatcher_entry& dispatcher       = dispatchers[index];
    data_flow_uplane_downlink_data&                  data_flow_uplane = *dispatcher.data_flow_uplane;

    dispatcher.executor.execute([&data_flow_uplane, context, &grid, eaxc]() {
      data_flow_uplane.enqueue_section_type_1_message(context, grid, eaxc);
    });
  }

private:
  std::vector<data_flow_uplane_downlink_task_dispatcher_entry> dispatchers;
};

} // namespace ofh
} // namespace srsran
