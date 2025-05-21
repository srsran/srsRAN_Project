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

#include "ofh_data_flow_uplane_downlink_data.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane downlink data flow task dispatcher implementation.
class data_flow_uplane_downlink_task_dispatcher : public data_flow_uplane_downlink_data
{
public:
  data_flow_uplane_downlink_task_dispatcher(srslog::basic_logger&                           logger_,
                                            std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane_,
                                            task_executor&                                  executor_,
                                            unsigned                                        sector_id_) :
    logger(logger_), data_flow_uplane(std::move(data_flow_uplane_)), executor(executor_), sector_id(sector_id_)
  {
    srsran_assert(data_flow_uplane, "Invalid data flow");
  }

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_uplane_resource_grid_context& context,
                                      const shared_resource_grid&                   grid) override
  {
    if (!executor.execute(
            [this, context, rg = grid.copy()]() { data_flow_uplane->enqueue_section_type_1_message(context, rg); })) {
      logger.warning("Sector#{}: failed to dispatch message in the downlink data flow User-Plane for slot '{}'",
                     sector_id,
                     context.slot);
    }
  }

  // See interface for documentation.
  data_flow_message_encoding_metrics_collector* get_metrics_collector() override
  {
    return data_flow_uplane->get_metrics_collector();
  }

private:
  srslog::basic_logger&                           logger;
  std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane;
  task_executor&                                  executor;
  const unsigned                                  sector_id;
};

} // namespace ofh
} // namespace srsran
