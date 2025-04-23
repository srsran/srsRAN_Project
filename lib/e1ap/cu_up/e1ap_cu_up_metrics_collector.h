
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

#include "srsran/e1ap/cu_up/e1ap_cu_up_metrics.h"
#include "srsran/support/srsran_assert.h"

namespace srsran::srs_cu_up {

/// Container to hold TX/RX metrics
class e1ap_cu_up_metrics_collector
{
  e1ap_cu_up_metrics_container metrics = {};
  bool                         enabled = false;

public:
  e1ap_cu_up_metrics_collector(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void reset_metrics() { metrics = {}; }

  e1ap_cu_up_metrics_container get_metrics() const
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    return metrics;
  }

  e1ap_cu_up_metrics_container get_metrics_and_reset()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    e1ap_cu_up_metrics_container ret = get_metrics();
    reset_metrics();
    return ret;
  }

  void add_successful_context_setup()
  {
    if (not enabled) {
      return;
    }
    metrics.nof_successful_bearer_context_setup++;
  }

  void add_successful_context_modification()
  {
    if (not enabled) {
      return;
    }
    metrics.nof_successful_bearer_context_modification++;
  }

  void add_context_release(std::chrono::microseconds proc_dur)
  {
    if (not enabled) {
      return;
    }
    metrics.nof_bearer_context_release++;

    metrics.sum_release_latency += proc_dur;

    unsigned bin_idx = proc_dur.count() / (1000 * e1ap_cu_up_metrics_container::nof_msec_per_bin);

    bin_idx = std::min(bin_idx, e1ap_cu_up_metrics_container::latency_hist_bins - 1);
    metrics.release_latency_hist[bin_idx]++;

    metrics.max_release_latency = std::max(proc_dur, metrics.max_release_latency);
  }
};
} // namespace srsran::srs_cu_up
