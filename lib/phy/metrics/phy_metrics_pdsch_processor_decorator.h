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

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"
#include <memory>

namespace srsran {

/// PDSCH processor metric decorator.
class phy_metrics_pdsch_processor_decorator : public pdsch_processor, private pdsch_processor_notifier
{
public:
  /// Creates a PDSCH processor decorator from a base instance and a metric notifier.
  phy_metrics_pdsch_processor_decorator(std::unique_ptr<pdsch_processor> base_,
                                        pdsch_processor_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid encoder.");
  }

  // See pdsch_processor interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier_,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override
  {
    // Prepare transmission.
    start_time            = std::chrono::high_resolution_clock::now();
    elapsed_return_ns     = {};
    elapsed_completion_ns = {};
    processor_notifier    = &notifier_;
    slot                  = pdu.slot;
    tbs                   = units::bytes(data.front().get_buffer().size());

    // Use scoped resource usage class to measure CPU usage of this block.
    resource_usage_utils::measurements measurements;
    {
      resource_usage_utils::scoped_resource_usage rusage_tracker(measurements);
      base->process(grid, *this, data, pdu);
    }
    self_cpu_usage_ns = measurements.duration.count();
    elapsed_return_ns = std::chrono::nanoseconds(std::chrono::high_resolution_clock::now() - start_time).count();

    report_metrics();
  }

private:
  // See pdsch_processor_notifier interface for documentation.
  void on_finish_processing() override
  {
    srsran_assert(processor_notifier != nullptr, "Invalid PDSCH processor notifier.");
    processor_notifier->on_finish_processing();
    processor_notifier = nullptr;

    elapsed_completion_ns = std::chrono::nanoseconds(std::chrono::high_resolution_clock::now() - start_time).count();

    report_metrics();
  }

  void report_metrics()
  {
    if ((elapsed_return_ns == 0) || (elapsed_completion_ns == 0)) {
      return;
    }

    notifier.on_new_metric(
        pdsch_processor_metrics{.slot                = slot,
                                .tbs                 = tbs,
                                .elapsed_return      = std::chrono::nanoseconds(elapsed_return_ns),
                                .elapsed_completion  = std::chrono::nanoseconds(elapsed_completion_ns),
                                .self_cpu_time_usage = std::chrono::nanoseconds(self_cpu_usage_ns)});
  }

  std::chrono::high_resolution_clock::time_point start_time            = {};
  std::atomic<uint64_t>                          elapsed_return_ns     = {};
  std::atomic<uint64_t>                          elapsed_completion_ns = {};
  std::atomic<uint64_t>                          self_cpu_usage_ns     = {};
  slot_point                                     slot;
  units::bytes                                   tbs;
  pdsch_processor_notifier*                      processor_notifier = nullptr;
  std::unique_ptr<pdsch_processor>               base;
  pdsch_processor_metric_notifier&               notifier;
};

} // namespace srsran
