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

#include "realtime_timing_worker.h"
#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"
#include <thread>

using namespace srsran;
using namespace ofh;

/// Difference between Unix seconds to GPS seconds.
/// GPS epoch: 1980.1.6 00:00:00 (UTC); Unix time epoch: 1970:1.1 00:00:00 UTC
/// Last leap second added in 31st Dec 2016 by IERS.
/// 1970:1.1 - 1980.1.6: 3657 days
/// 3657*24*3600=315 964 800 seconds (Unix seconds value at 1980.1.6 00:00:00 (UTC))
/// There are 18 leap seconds inserted after 1980.1.6 00:00:00 (UTC), which means GPS is 18 seconds larger.
static constexpr uint64_t UNIX_TO_GPS_SECONDS_OFFSET = 315964800ULL - 18ULL;

/// Offset for converting from UTC to GPS time including Alpha and Beta parameters.
static std::chrono::nanoseconds gps_offset;

namespace {

/// A GPS clock implementation.
struct gps_clock {
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

} // namespace

/// Calculates the fractional part inside a second from the given time point.
static std::chrono::nanoseconds calculate_ns_fraction_from(gps_clock::time_point tp)
{
  auto tp_sec = std::chrono::time_point_cast<std::chrono::seconds>(tp);
  return tp - tp_sec;
}

realtime_timing_worker::realtime_timing_worker(srslog::basic_logger&         logger_,
                                               ota_symbol_boundary_notifier& notifier_,
                                               task_executor&                executor_,
                                               const realtime_worker_cfg&    cfg) :
  logger(logger_),
  notifier(notifier_),
  executor(executor_),
  scs(cfg.scs),
  nof_symbols_per_slot(get_nsymb_per_slot(cfg.cp)),
  nof_symbols_per_sec(nof_symbols_per_slot * get_nof_slots_per_subframe(scs) * NOF_SUBFRAMES_PER_FRAME * 100),
  symbol_duration(1e9 / nof_symbols_per_sec),
  sleep_time(std::chrono::duration_cast<std::chrono::nanoseconds>(symbol_duration) / 15)
{
  gps_offset = std::chrono::nanoseconds(static_cast<unsigned>(std::round(cfg.gps_Alpha / 1.2288))) +
               std::chrono::milliseconds(cfg.gps_Beta * 10) + std::chrono::seconds(UNIX_TO_GPS_SECONDS_OFFSET);
}

/// Returns the symbol index inside a second.
static unsigned get_symbol_index(std::chrono::nanoseconds                 fractional_ns,
                                 std::chrono::duration<double, std::nano> symbol_duration)
{
  // Perform operation with enough precision to avoid rounding errors when the amount of fractional nanoseconds is big.
  return fractional_ns / symbol_duration;
}

void realtime_timing_worker::start()
{
  logger.info("Starting the realtime timing worker");
  executor.defer([this]() {
    auto ns_fraction    = calculate_ns_fraction_from(gps_clock::now());
    previous_symb_index = get_symbol_index(ns_fraction, symbol_duration);
    timing_loop();
  });
}

void realtime_timing_worker::stop()
{
  logger.info("Requesting stop of the realtime timing worker");
  is_stop_requested.store(true, std::memory_order::memory_order_relaxed);
}

void realtime_timing_worker::timing_loop()
{
  poll();

  if (is_stop_requested.load(std::memory_order_relaxed)) {
    return;
  }

  executor.defer([this]() { timing_loop(); });
}

/// Returns the difference between cur and prev taking into account wrap arounds of the values.
static unsigned circular_distance(unsigned cur, unsigned prev, unsigned size)
{
  return (cur >= prev) ? (cur - prev) : (size + cur - prev);
}

/// Calculates the current slot point given the GPS time.
static slot_point
calculate_slot_point(subcarrier_spacing scs, uint64_t gps_seconds, uint32_t fractional_us, uint32_t slot_duration_us)
{
  unsigned subframe_index      = (fractional_us / (SUBFRAME_DURATION_MSEC * 1000)) % NOF_SUBFRAMES_PER_FRAME;
  unsigned slot_subframe_index = (fractional_us / slot_duration_us) % get_nof_slots_per_subframe(scs);

  static constexpr unsigned NUM_FRAMES_PER_SEC = 100;
  static constexpr unsigned FRAME_DURATION_US  = SUBFRAME_DURATION_MSEC * NOF_SUBFRAMES_PER_FRAME * 1000;
  unsigned                  sfn = (gps_seconds * NUM_FRAMES_PER_SEC + (fractional_us / FRAME_DURATION_US)) % NOF_SFNS;

  return {to_numerology_value(scs), sfn, subframe_index, slot_subframe_index};
}

void realtime_timing_worker::poll()
{
  auto now         = gps_clock::now();
  auto ns_fraction = calculate_ns_fraction_from(now);

  unsigned current_symbol_index = get_symbol_index(ns_fraction, symbol_duration);
  unsigned delta                = circular_distance(current_symbol_index, previous_symb_index, nof_symbols_per_sec);
  previous_symb_index           = current_symbol_index;

  // Are we still in the same symbol as before?
  if (delta == 0) {
    std::this_thread::sleep_for(sleep_time);
    return;
  }

  // Check if we have missed more than one symbol.
  if (delta > 1) {
    logger.warning("Real-time timing worker late, skipped {} symbols", delta);
  }

  slot_symbol_point symbol_point(
      calculate_slot_point(scs,
                           std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count(),
                           std::chrono::duration_cast<std::chrono::microseconds>(ns_fraction).count(),
                           1000 / get_nof_slots_per_subframe(scs)),
      current_symbol_index % nof_symbols_per_slot,
      nof_symbols_per_slot);

  for (unsigned i = 0; i != delta; ++i) {
    // Notify pending symbols from oldest to newest.
    notifier.on_new_symbol(symbol_point - (delta - 1 - i));
  }
}
