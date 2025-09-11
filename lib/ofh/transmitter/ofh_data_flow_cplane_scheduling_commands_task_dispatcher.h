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

#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/synchronization/stop_event.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Open Fronthaul Control-Plane scheduling and beamforming commands data flow task dispatcher implementation.
class data_flow_cplane_downlink_task_dispatcher : public data_flow_cplane_scheduling_commands,
                                                  public operation_controller
{
public:
  data_flow_cplane_downlink_task_dispatcher(srslog::basic_logger&                                 logger_,
                                            std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane_,
                                            task_executor&                                        executor_,
                                            unsigned                                              sector_id_) :
    logger(logger_), data_flow_cplane(std::move(data_flow_cplane_)), executor(executor_), sector_id(sector_id_)
  {
    srsran_assert(data_flow_cplane, "Invalid data flow");
  }

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override { stop_manager.reset(); }

  // See interface for documentation.
  void stop() override { stop_manager.stop(); }

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_cplane_type_1_context& context) override
  {
    // Do not process Control Plane if the stop was requested.
    if (stop_manager.stop_was_requested()) {
      return;
    }

    if (!executor.defer([this, context, token = stop_manager.get_token()]() noexcept SRSRAN_RTSAN_NONBLOCKING {
          data_flow_cplane->enqueue_section_type_1_message(context);
        })) {
      logger.warning(
          "Sector#{}: failed to dispatch Control-Plane type 1 message for slot '{}'", sector_id, context.slot);
    }
  }

  // See interface for documentation.
  void enqueue_section_type_3_prach_message(const data_flow_cplane_scheduling_prach_context& context) override
  {
    // Do not process Control Plane if the stop was requested.
    if (stop_manager.stop_was_requested()) {
      return;
    }

    if (!executor.defer([this, context, token = stop_manager.get_token()]() noexcept SRSRAN_RTSAN_NONBLOCKING {
          data_flow_cplane->enqueue_section_type_3_prach_message(context);
        })) {
      logger.warning(
          "Sector#{}: failed to dispatch Control-Plane type 3 message for slot '{}'", sector_id, context.slot);
    }
  }

  // See interface for documentation.
  data_flow_message_encoding_metrics_collector* get_metrics_collector() override
  {
    return data_flow_cplane->get_metrics_collector();
  }

private:
  srslog::basic_logger&                                 logger;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  task_executor&                                        executor;
  const unsigned                                        sector_id;
  stop_event_source                                     stop_manager;
};

} // namespace ofh
} // namespace srsran
