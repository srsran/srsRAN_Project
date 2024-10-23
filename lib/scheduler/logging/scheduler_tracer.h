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

class scheduler_slot_tracer
{
public:
  scheduler_slot_tracer(srslog::basic_logger&     logger_,
                        du_cell_index_t           cell_index_,
                        std::chrono::microseconds log_warn_thres_ = std::chrono::microseconds{0}) :
    logger(logger_), cell_index(cell_index_), log_warn_thres(log_warn_thres_)
  {
  }

  void start()
  {
    slot_start_tp = std::chrono::high_resolution_clock::now();
    if (is_resource_usage_traced()) {
      slot_start_resusage = resource_usage::now().value();
    }
  }

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