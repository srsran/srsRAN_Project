/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_timing_metrics_collector_impl.h"
#include "../support/metrics_helpers.h"
#include "srsran/ofh/timing/ofh_timing_metrics.h"

using namespace srsran;
using namespace ofh;

void timing_metrics_collector_impl::collect_metrics(timing_metrics& metrics)
{
  metrics.nof_skipped_symbols       = nof_skipped_symbols.exchange(0, std::memory_order_relaxed);
  metrics.skipped_symbols_max_burst = skipped_symbols_max_burst.exchange(0, std::memory_order_relaxed);

  uint32_t count_val          = count.exchange(0, std::memory_order_relaxed);
  uint64_t sum_elapsed_val_ns = sum_elapsed_ns.exchange(0, std::memory_order_relaxed);
  uint32_t min_latency_val_ns = min_latency_ns.exchange(default_min_latency_ns, std::memory_order_relaxed);
  uint32_t max_latency_val_ns = max_latency_ns.exchange(default_max_latency_ns, std::memory_order_relaxed);

  metrics.notification_avg_latency_us = count_val ? static_cast<double>(sum_elapsed_val_ns) / count_val / 1000.0 : 0.f;
  metrics.notification_min_latency_us =
      (min_latency_val_ns == default_min_latency_ns) ? 0.f : static_cast<float>(min_latency_val_ns) / 1000.f;
  metrics.notification_max_latency_us =
      (max_latency_val_ns == default_max_latency_ns) ? 0.f : static_cast<float>(max_latency_val_ns) / 1000.f;
}

void timing_metrics_collector_impl::update_skipped_symbols(unsigned num_skipped_symbols)
{
  nof_skipped_symbols.fetch_add(num_skipped_symbols, std::memory_order_relaxed);

  // Update the maximum number of continuous symbols skipped.
  unsigned current_max = skipped_symbols_max_burst.load(std::memory_order_relaxed);
  while (num_skipped_symbols > current_max &&
         !skipped_symbols_max_burst.compare_exchange_weak(current_max, num_skipped_symbols)) {
  }
}

void timing_metrics_collector_impl::update_symbol_notification_latency(std::chrono::nanoseconds notifier_latency)
{
  sum_elapsed_ns.fetch_add(notifier_latency.count(), std::memory_order_relaxed);
  count.fetch_add(1u, std::memory_order_relaxed);
  update_minmax(static_cast<uint32_t>(notifier_latency.count()), max_latency_ns, min_latency_ns);
}
