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
    // Save reference to the notifier for this transmission. It must be nullptr to ensure that the processor was
    // released from previous processing.
    [[maybe_unused]] pdsch_processor_notifier* prev_proc_notifier = std::exchange(processor_notifier, &notifier_);
    srsran_assert(prev_proc_notifier == nullptr, "The PDSCH processor is in use.");

    // Prepare transmission.
    start_time                       = std::chrono::high_resolution_clock::now();
    elapsed_completion_and_return_ns = {};
    slot                             = pdu.slot;
    tbs                              = units::bytes(data.front().get_buffer().size());

    // Use scoped resource usage class to measure CPU usage of this block.
    resource_usage_utils::measurements measurements;
    {
      resource_usage_utils::scoped_resource_usage rusage_tracker(measurements);
      base->process(grid, *this, data, pdu);
    }
    self_cpu_usage_ns = measurements.duration.count();
    elapsed_completion_and_return_ns |=
        std::min(std::chrono::nanoseconds(std::chrono::high_resolution_clock::now() - start_time).count(), 0xffffffffL);

    report_metrics();
  }

private:
  // See pdsch_processor_notifier interface for documentation.
  void on_finish_processing() override
  {
    // Update elapsed time.
    elapsed_completion_and_return_ns |=
        std::min(std::chrono::nanoseconds(std::chrono::high_resolution_clock::now() - start_time).count(), 0xffffffffL)
        << 32;

    // Report metrics.
    report_metrics();
  }

  /// Reports the PDSCH processing metrics if the underlying PDSCH processor has returned and notified the completion of
  /// the processing.
  void report_metrics()
  {
    // The processing is considered complete if the processor has returned and notified the completion.
    uint64_t current_elapsed_completion_and_return_ns = elapsed_completion_and_return_ns;
    uint64_t elapsed_return_ns(current_elapsed_completion_and_return_ns & 0xffffffff);
    uint64_t elapsed_completion_ns(current_elapsed_completion_and_return_ns >> 32);
    if ((elapsed_return_ns == 0) || (elapsed_completion_ns == 0)) {
      return;
    }
    if (!elapsed_completion_and_return_ns.compare_exchange_weak(current_elapsed_completion_and_return_ns, 0)) {
      return;
    }

    notifier.on_new_metric(
        pdsch_processor_metrics{.slot                = slot,
                                .tbs                 = tbs,
                                .elapsed_return      = std::chrono::nanoseconds(elapsed_return_ns),
                                .elapsed_completion  = std::chrono::nanoseconds(elapsed_completion_ns),
                                .self_cpu_time_usage = std::chrono::nanoseconds(self_cpu_usage_ns)});

    // Notify the completion of the PDSCH processing. From now on, the processor might become available.
    pdsch_processor_notifier* current_proc_notifier = std::exchange(processor_notifier, nullptr);
    srsran_assert(current_proc_notifier != nullptr, "PDSCH processor is still busy.");
    current_proc_notifier->on_finish_processing();
  }

  std::chrono::high_resolution_clock::time_point start_time                       = {};
  std::atomic<uint64_t>                          elapsed_completion_and_return_ns = {};
  std::atomic<uint64_t>                          self_cpu_usage_ns                = {};
  pdsch_processor_notifier*                      processor_notifier               = nullptr;
  slot_point                                     slot;
  units::bytes                                   tbs;
  std::unique_ptr<pdsch_processor>               base;
  pdsch_processor_metric_notifier&               notifier;

  // Makes sure atomics are lock free.
  static_assert(std::atomic<decltype(elapsed_completion_and_return_ns)>::is_always_lock_free);
  static_assert(std::atomic<decltype(self_cpu_usage_ns)>::is_always_lock_free);
};

} // namespace srsran
