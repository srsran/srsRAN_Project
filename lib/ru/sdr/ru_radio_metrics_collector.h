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

#include "srsran/radio/radio_event_notifier.h"
#include "srsran/ru/sdr/ru_sdr_metrics.h"
#include <atomic>

namespace srsran {

/// Radio Unit radio notification counter.
class ru_radio_metrics_collector : public radio_event_notifier
{
public:
  // See interface for documentation.
  void on_radio_rt_event(const event_description& description) override;

  /// Collect radio metrics.
  void collect_metrics(radio_metrics& metrics);

private:
  std::atomic<unsigned> late_count      = {};
  std::atomic<unsigned> underflow_count = {};
  std::atomic<unsigned> overflow_count  = {};
};

} // namespace srsran
