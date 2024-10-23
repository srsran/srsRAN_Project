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

#include "srsran/support/tracing/resource_usage.h"
#include <optional>

namespace srsran {

/// Traces the latency at which the scheduler handles new slot indications.
class scheduler_slot_tracer
{
public:
  /// \brief Creates a new scheduler slot tracer.
  ///
  /// \param[in] logger_ Logger to which warnings of slow slots are reported.
  /// \param[in] cell_index_ Cell index associated with this tracer.
  /// \param[in] log_warn_thres_ Threshold in microseconds, above which, a warning is reported of a slow slot handling.
  scheduler_slot_tracer(srslog::basic_logger&     logger_,
                        du_cell_index_t           cell_index_,
                        std::chrono::microseconds log_warn_thres_ = std::chrono::microseconds{0}) :
    logger(logger_), cell_index(cell_index_), log_warn_thres(log_warn_thres_)
  {
  }

  /// Marks the start of a new slot indication.
  void start()
  {
    slot_start_tp = std::chrono::high_resolution_clock::now();
    if (is_resource_usage_traced()) {
      slot_start_resusage = resource_usage::now().value();
    }
  }

  /// \brief Marks the end of a slot indication, and computes how much time has passed. If \c log_warn_thres > 0 and
  /// the slot latency is greater than \c log_warn_thres, a warning is logged with some metrics.
  void stop()
  {
    auto slot_stop_tp = std::chrono::high_resolution_clock::now();
    slot_dur          = std::chrono::duration_cast<std::chrono::microseconds>(slot_stop_tp - slot_start_tp);

    if (slot_dur > log_warn_thres) {
      auto slot_stop_resusage = resource_usage::now().value();
      diff_resusage           = slot_stop_resusage - slot_start_resusage;

      logger.warning("cell={}: Detected slow slot processing. Latency={}usec, voluntary_context_switches={} "
                     "involutary_context_switches={}",
                     cell_index,
                     slot_dur.count(),
                     diff_resusage.vol_ctxt_switch_count,
                     diff_resusage.invol_ctxt_switch_count);
    }
  }

  /// Returns how much time has passed between the last start() and stop().
  std::chrono::microseconds time_elapsed() const { return slot_dur; }

private:
  bool is_resource_usage_traced() const { return log_warn_thres.count() > 0; }

  srslog::basic_logger&     logger;
  const du_cell_index_t     cell_index;
  std::chrono::microseconds log_warn_thres;

  std::chrono::time_point<std::chrono::high_resolution_clock> slot_start_tp;
  std::chrono::microseconds                                   slot_dur;

  resource_usage::snapshot slot_start_resusage;
  resource_usage::diff     diff_resusage;
};

} // namespace srsran