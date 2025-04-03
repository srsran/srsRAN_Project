
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

#include "cu_up/metrics/e1ap_cu_up_metrics.h"
#include "srsran/support/srsran_assert.h"

namespace srsran::srs_cu_up {

/// Container to hold TX/RX metrics
struct e1ap_cu_up_metrics {
  e1ap_cu_up_metrics_container metrics = {};
  bool                         enabled = false;

public:
  e1ap_cu_up_metrics(bool enabled_) : enabled(enabled_) {}

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

  void add_successful_context_release()
  {
    if (not enabled) {
      return;
    }
    metrics.nof_successful_bearer_context_modification++;
  }
};
} // namespace srsran::srs_cu_up
