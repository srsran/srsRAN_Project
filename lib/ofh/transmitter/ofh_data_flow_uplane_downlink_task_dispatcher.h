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
  data_flow_uplane_downlink_task_dispatcher(std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane_,
                                            task_executor&                                  executor_,
                                            unsigned                                        sector_id_) :
    data_flow_uplane(std::move(data_flow_uplane_)), executor(executor_), sector_id(sector_id_)
  {
    srsran_assert(data_flow_uplane, "Invalid data flow");
  }

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_uplane_resource_grid_context& context,
                                      const shared_resource_grid&                   grid) override
  {
    if (!executor.execute(
            [this, context, rg = grid.copy()]() { data_flow_uplane->enqueue_section_type_1_message(context, rg); })) {
      srslog::fetch_basic_logger("OFH").warning(
          "Sector#{}: failed to dispatch message in the downlink data flow User-Plane for slot '{}'",
          sector_id,
          context.slot);
    }
  }

private:
  std::unique_ptr<data_flow_uplane_downlink_data> data_flow_uplane;
  task_executor&                                  executor;
  const unsigned                                  sector_id;
};

} // namespace ofh
} // namespace srsran
