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

#include "ofh_timing_metrics_collector_impl.h"
#include "srsran/adt/span.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier_manager.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/synchronization/stop_event.h"
#include <atomic>

namespace srsran {
namespace ofh {

class ota_symbol_boundary_notifier;
class slot_symbol_point;

/// Configuration for the realtime timing worker.
struct realtime_worker_cfg {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// GPS Alpha.
  unsigned gps_Alpha;
  /// GPS Beta.
  int gps_Beta;
  /// If set to true, logs late events as warnings, otherwise as info.
  bool enable_log_warnings_for_lates;
};

/// Realtime worker that generates OTA symbol notifications.
class realtime_timing_worker : public operation_controller, public ota_symbol_boundary_notifier_manager
{
  /// A GPS clock implementation.
  struct gps_clock {
    /// Difference between Unix seconds to GPS seconds.
    /// GPS epoch: 1980.1.6 00:00:00 (UTC); Unix time epoch: 1970:1.1 00:00:00 UTC
    /// Last leap second added in 31st Dec 2016 by IERS.
    /// 1970:1.1 - 1980.1.6: 3657 days
    /// 3657*24*3600=315 964 800 seconds (Unix seconds value at 1980.1.6 00:00:00 (UTC))
    /// There are 18 leap seconds inserted after 1980.1.6 00:00:00 (UTC), which means GPS is 18 seconds larger.
    static constexpr uint64_t UNIX_TO_GPS_SECONDS_OFFSET = 315964800ULL - 18ULL;

    /// Offset for converting from UTC to GPS time including Alpha and Beta parameters.
    static inline std::chrono::nanoseconds gps_offset = std::chrono::seconds(UNIX_TO_GPS_SECONDS_OFFSET);

    using duration   = std::chrono::nanoseconds;
    using rep        = duration::rep;
    using period     = duration::period;
    using time_point = std::chrono::time_point<gps_clock>;
    // static constexpr bool is_steady = false;

    static time_point now()
    {
      ::timespec ts;
      ::clock_gettime(CLOCK_REALTIME, &ts);

      time_point now(std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec));

      return now - gps_offset;
    }
  };

  srslog::basic_logger&                          logger;
  std::vector<ota_symbol_boundary_notifier*>     ota_notifiers;
  task_executor&                                 executor;
  const subcarrier_spacing                       scs;
  const unsigned                                 nof_symbols_per_slot;
  const unsigned                                 nof_symbols_per_sec;
  const std::chrono::duration<double, std::nano> symbol_duration;
  const std::chrono::nanoseconds                 sleep_time;
  bool                                           enable_log_warnings_for_lates;
  unsigned                                       previous_symb_index       = 0;
  gps_clock::rep                                 previous_time_since_epoch = 0;
  stop_event_source                              stop_manager;
  timing_metrics_collector_impl                  metrics_collector;

public:
  realtime_timing_worker(srslog::basic_logger& logger_, task_executor& executor_, const realtime_worker_cfg& cfg);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  void subscribe(span<ota_symbol_boundary_notifier*> notifiers) override;

  /// Returns the metrics collector of this timing worker.
  timing_metrics_collector& get_metrics_collector() { return metrics_collector; }

private:
  /// Main timing loop.
  void timing_loop() noexcept;

  /// Polls the system time checking for the start of a new OTA symbol.
  void poll();

  /// Notifies the given slot symbol point through the registered notifiers.
  void notify_slot_symbol_point(const slot_symbol_point_context& slot_context);

  /// Calculates the fractional part inside a second from the given time point.
  static std::chrono::nanoseconds calculate_ns_fraction_from(gps_clock::time_point tp)
  {
    auto tp_sec = std::chrono::time_point_cast<std::chrono::seconds>(tp);
    return tp - tp_sec;
  }
};

} // namespace ofh
} // namespace srsran
