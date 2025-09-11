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

#include "../support/prach_context_repository.h"
#include "../support/uplink_context_repository.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"
#include "srsran/ofh/receiver/ofh_receiver_timing_parameters.h"
#include "srsran/ofh/receiver/ofh_receiver_warn_unreceived_frames_parameters.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/timing/slot_symbol_point.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/synchronization/stop_event.h"

namespace srsran {

class task_executor;

namespace ofh {

/// Closed reception window handler configuration.
struct closed_rx_window_handler_config {
  /// Radio sector identifier.
  unsigned sector;
  /// Time in number of symbols that the decoder needs to process an Open Fronthaul message. It delays closing the
  /// reception window.
  unsigned nof_symbols_to_process_uplink = 0;
  /// Open Fronthaul receive window parameters.
  rx_window_timing_parameters rx_timing_params;
  /// Warn unreceived Open Fronthaul messages.
  warn_unreceived_ru_frames warn_unreceived_frames = warn_unreceived_ru_frames::after_traffic_detection;
};

/// Closed reception window handler dependencies.
struct closed_rx_window_handler_dependencies {
  srslog::basic_logger*                      logger   = nullptr;
  task_executor*                             executor = nullptr;
  std::shared_ptr<prach_context_repository>  prach_repo;
  std::shared_ptr<uplink_context_repository> uplink_repo;
  std::shared_ptr<uplane_rx_symbol_notifier> notifier;
};

/// Open Fronthaul closed reception window handler.
class closed_rx_window_handler : public ota_symbol_boundary_notifier, public operation_controller
{
public:
  closed_rx_window_handler(const closed_rx_window_handler_config&  config,
                           closed_rx_window_handler_dependencies&& dependencies);

  void start() override { stop_manager.reset(); }

  void stop() override { stop_manager.stop(); }

  // See interface for documentation.
  void on_new_symbol(const slot_symbol_point_context& symbol_point_context) override;

  /// Starts logging unreceived OFH messages.
  void start_logging_unreceived_messages()
  {
    if (warn_unreceived_frames == warn_unreceived_ru_frames::after_traffic_detection && !log_unreceived_messages) {
      log_unreceived_messages = true;
    }
  }

  /// Collects the metrics of the closed reception window metrics.
  void collect_metrics(closed_rx_window_metrics& metrics);

private:
  /// \brief Handles the uplink context for the closed reception window given by symbol point.
  ///
  /// Pops an uplink context from the uplink repository and when the context is valid, notifies it using the User-Plane
  /// received symbol notifier.
  void handle_uplink_context(slot_symbol_point symbol_point);

  /// \brief Handles the PRACH context for the closed reception window given by symbol point.
  ///
  /// Pops a PRACH context from the PRACH repository and when the context is valid, notifies it using the User-Plane
  /// received symbol notifier.
  void handle_prach_context(slot_symbol_point symbol_point);

private:
  /// \brief Notification delay of the resource grid or PRACH buffer in symbol units.
  ///
  /// This delay is calculated with the T4a_max parameter plus the number of symbols that takes to decode a received
  /// Open Fronthaul message.
  const unsigned                             notification_delay_in_symbols;
  const unsigned                             sector_id;
  const warn_unreceived_ru_frames            warn_unreceived_frames;
  bool                                       log_unreceived_messages;
  srslog::basic_logger&                      logger;
  task_executor&                             executor;
  std::shared_ptr<prach_context_repository>  prach_repo;
  std::shared_ptr<uplink_context_repository> uplink_repo;
  std::shared_ptr<uplane_rx_symbol_notifier> notifier;
  /// Counts every symbol not received when reception window closes.
  std::atomic<unsigned> nof_missed_uplink_symbols;
  /// Counts every PRACH not received when the reception window closes.
  std::atomic<unsigned> nof_missed_prach_contexts;
  stop_event_source     stop_manager;
};

} // namespace ofh
} // namespace srsran
