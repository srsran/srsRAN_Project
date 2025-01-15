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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier_manager.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include <atomic>

namespace srsran {

/// Realtime worker that generates OTA symbol notifications.
class ru_emulator_timing_notifier : public ofh::ota_symbol_boundary_notifier_manager
{
  enum class worker_status { idle, running, stop_requested, stopped };

  srslog::basic_logger&                           logger;
  std::vector<ofh::ota_symbol_boundary_notifier*> ota_notifiers;
  task_executor&                                  executor;
  subcarrier_spacing                              scs;
  const unsigned                                  nof_symbols_per_slot;
  const unsigned                                  nof_symbols_per_sec;
  const unsigned                                  nof_slots_per_system_frame;
  const std::chrono::duration<double, std::nano>  symbol_duration;
  const std::chrono::nanoseconds                  sleep_time;
  unsigned                                        previous_symb_index = 0;
  std::atomic<worker_status>                      status{worker_status::idle};

public:
  ru_emulator_timing_notifier(srslog::basic_logger& logger_, task_executor& executor_);

  /// Starts operation of the timing notifier.
  void start();

  /// Stops operation of the timing notifier.
  void stop();

  /// See interface for documentation.
  void subscribe(span<ofh::ota_symbol_boundary_notifier*> notifiers) override;

private:
  /// Main timing loop.
  void timing_loop();

  /// Polls the system time checking for the start of a new OTA symbol.
  void poll();

  /// Notifies the given slot symbol point through the registered notifiers.
  void notify_slot_symbol_point(ofh::slot_symbol_point slot);
};

} // namespace srsran
