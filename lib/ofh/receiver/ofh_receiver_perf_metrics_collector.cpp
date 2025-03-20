/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_receiver_perf_metrics_collector.h"
#include "../support/metrics_helpers.h"

using namespace srsran;
using namespace ofh;

void receiver_performance_metrics_collector::update_receiver_stats(std::chrono::nanoseconds exec_latency, bool is_prach)
{
  if (is_disabled) {
    return;
  }

  auto& stats = is_prach ? processing_stats[PRACH] : processing_stats[DATA];

  stats.sum_elapsed_ns.fetch_add(exec_latency.count(), std::memory_order_relaxed);
  stats.count.fetch_add(1u, std::memory_order_relaxed);
  update_minmax(static_cast<uint32_t>(exec_latency.count()), stats.max_latency_ns, stats.min_latency_ns);
}

void receiver_performance_metrics_collector::collect_metrics(receiver_performance_metrics& metrics)
{
  // Collect PRACH processing performance metrics.
  update_metrics(metrics.prach_processing_metrics, processing_stats[PRACH]);

  // Collect data processing performance metrics.
  update_metrics(metrics.data_processing_metrics, processing_stats[DATA]);

  reset();
}

void receiver_performance_metrics_collector::update_metrics(
    receiver_performance_metrics::unpacking_perf_metrics& metrics,
    const message_processing_stats&                       stats) const
{
  uint32_t count          = stats.count.load(std::memory_order_relaxed);
  uint32_t min_latency_ns = stats.min_latency_ns.load(std::memory_order_relaxed);
  uint32_t max_latency_ns = stats.max_latency_ns.load(std::memory_order_relaxed);
  uint64_t sum_elapsed_ns = stats.sum_elapsed_ns.load(std::memory_order_relaxed);

  metrics.message_unpacking_avg_latency_us = count ? static_cast<double>(sum_elapsed_ns) / count / 1000.0 : 0.f;

  metrics.message_unpacking_min_latency_us =
      (min_latency_ns == default_min_latency_ns) ? 0.f : static_cast<float>(min_latency_ns) / 1000.f;

  metrics.message_unpacking_max_latency_us =
      (max_latency_ns == default_max_latency_ns) ? 0.f : static_cast<float>(max_latency_ns) / 1000.f;

  metrics.cpu_usage_us = static_cast<double>(sum_elapsed_ns) / 1000.0;
}

void receiver_performance_metrics_collector::reset()
{
  for (auto& stats : processing_stats) {
    stats.count.store(0, std::memory_order_relaxed);
    stats.sum_elapsed_ns.store(0, std::memory_order_relaxed);
    stats.min_latency_ns.store(default_min_latency_ns, std::memory_order_relaxed);
    stats.max_latency_ns.store(default_max_latency_ns, std::memory_order_relaxed);
  }
}
