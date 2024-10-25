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
    if (active()) {
      slot_start_resusage = resource_usage::now().value();
    }
  }

  /// \brief If the slot latency is greater than \c log_warn_thres, a warning logged with resource usage metrics.
  void stop(std::chrono::microseconds slot_dur)
  {
    if (active() and slot_dur > log_warn_thres) {
      auto slot_stop_resusage = resource_usage::now().value();
      diff_resusage           = slot_stop_resusage - slot_start_resusage;

      logger.warning("cell={}: High latency detected. Latency={}usec {}", cell_index, slot_dur.count(), diff_resusage);
    }
  }

  bool active() const { return log_warn_thres.count() > 0; }

private:
  srslog::basic_logger&           logger;
  const du_cell_index_t           cell_index;
  const std::chrono::microseconds log_warn_thres;

  resource_usage::snapshot slot_start_resusage;
  resource_usage::diff     diff_resusage;
};

} // namespace srsran