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

#include "../support/metrics_helpers.h"
#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {
namespace ofh {

/// Metrics decorator class for the Open Fronthaul Control-Plane scheduling and beamforming commands data flow.
class data_flow_cplane_metrics_decorator : public data_flow_cplane_scheduling_commands,
                                           public data_flow_message_encoding_metrics_collector
{
public:
  explicit data_flow_cplane_metrics_decorator(std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane_) :
    data_flow_cplane(std::move(data_flow_cplane_))
  {
    srsran_assert(data_flow_cplane, "Invalid data flow");
  }

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return controller; }

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_cplane_type_1_context& context) override
  {
    resource_usage_utils::measurements m;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(m);
      data_flow_cplane->enqueue_section_type_1_message(context);
    }
    count.fetch_add(1u, std::memory_order_relaxed);
    sum_elapsed_ns.fetch_add(m.duration.count(), std::memory_order_relaxed);
    update_minmax(static_cast<uint32_t>(m.duration.count()), max_latency_ns, min_latency_ns);
  }

  // See interface for documentation.
  void enqueue_section_type_3_prach_message(const data_flow_cplane_scheduling_prach_context& context) override
  {
    resource_usage_utils::measurements m;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(m);
      data_flow_cplane->enqueue_section_type_3_prach_message(context);
    }
    count.fetch_add(1u, std::memory_order_relaxed);
    sum_elapsed_ns.fetch_add(m.duration.count(), std::memory_order_relaxed);
    update_minmax(static_cast<uint32_t>(m.duration.count()), max_latency_ns, min_latency_ns);
  }

  // See interface for documentation.
  data_flow_message_encoding_metrics_collector* get_metrics_collector() override { return this; }

  // See interface for documentation.
  void collect_metrics(tx_data_flow_perf_metrics& metrics) override
  {
    uint32_t total_count = count.load(std::memory_order_relaxed);
    uint32_t min_latency = min_latency_ns.load(std::memory_order_relaxed);
    uint32_t max_latency = max_latency_ns.load(std::memory_order_relaxed);
    uint64_t sum_elapsed = sum_elapsed_ns.load(std::memory_order_relaxed);

    metrics.message_packing_avg_latency_us =
        total_count ? static_cast<double>(sum_elapsed) / total_count / 1000.0 : 0.f;

    metrics.message_packing_max_latency_us =
        (max_latency == default_max_latency_ns) ? 0.f : static_cast<float>(max_latency) / 1000.f;

    metrics.message_packing_min_latency_us =
        (min_latency == default_min_latency_ns) ? 0.f : static_cast<float>(min_latency) / 1000.f;

    metrics.cpu_usage_us = static_cast<double>(sum_elapsed) / 1000.0;

    reset();
  }

private:
  static constexpr uint32_t default_min_latency_ns = std::numeric_limits<uint32_t>::max();
  static constexpr uint32_t default_max_latency_ns = std::numeric_limits<uint32_t>::min();

  /// Resets values of all internal counters.
  void reset()
  {
    count.store(0, std::memory_order_relaxed);
    sum_elapsed_ns.store(0, std::memory_order_relaxed);
    min_latency_ns.store(default_min_latency_ns, std::memory_order_relaxed);
    max_latency_ns.store(default_max_latency_ns, std::memory_order_relaxed);
  }

  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;

  std::atomic<uint32_t>      count          = {};
  std::atomic<uint64_t>      sum_elapsed_ns = {};
  std::atomic<uint32_t>      min_latency_ns = default_min_latency_ns;
  std::atomic<uint32_t>      max_latency_ns = default_max_latency_ns;
  operation_controller_dummy controller;
};

} // namespace ofh
} // namespace srsran
