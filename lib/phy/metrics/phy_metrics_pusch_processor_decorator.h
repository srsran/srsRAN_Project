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
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/support/resource_usage/scoped_resource_usage.h"

namespace srsran {

/// PUSCH processor metric decorator.
class phy_metrics_pusch_processor_decorator : public pusch_processor, private pusch_processor_result_notifier
{
public:
  /// Creates a PUSCH processor decorator from a base PUSCH processor instance and a metric notifier.
  phy_metrics_pusch_processor_decorator(std::unique_ptr<pusch_processor> processor_,
                                        pusch_processor_metric_notifier& metric_notifier_) :
    processor(std::move(processor_)), metric_notifier(metric_notifier_)
  {
    srsran_assert(processor, "Invalid processor.");
  }

  // See pusch_processor interface for documentation.
  void process(span<uint8_t>                    data_,
               unique_rx_buffer                 rm_buffer,
               pusch_processor_result_notifier& notifier_,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu_) override
  {
    notifier = &notifier_;
    data     = data_;
    pdu      = pdu_;

    // Setup times.
    time_start        = std::chrono::steady_clock::now();
    elapsed_data_ns   = 0;
    elapsed_uci_ns    = 0;
    elapsed_return_ns = 0;
    cpu_time_usage_ns = 0;

    resource_usage_utils::measurements measurements;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(measurements);
      processor->process(data, std::move(rm_buffer), *this, grid, pdu);
    }
    cpu_time_usage_ns = measurements.duration.count();
    elapsed_return_ns = std::chrono::nanoseconds(std::chrono::steady_clock::now() - time_start).count();

    // Notify metrics.
    notify_metrics();
  }

private:
  // See pusch_processor_result_notifier for documentation.
  void on_uci(const pusch_processor_result_control& uci) override
  {
    srsran_assert(notifier, "Invalid notifier");

    // Save UCI reporting time.
    elapsed_uci_ns = std::chrono::nanoseconds(std::chrono::steady_clock::now() - time_start).count();

    // Notify higher layers.
    notifier->on_uci(uci);
  }

  // See pusch_processor_result_notifier for documentation.
  void on_sch(const pusch_processor_result_data& sch) override
  {
    srsran_assert(notifier, "Invalid notifier");

    // Save SCH results.
    sch_result = sch;

    // Save data reporting time.
    elapsed_data_ns = std::chrono::nanoseconds(std::chrono::steady_clock::now() - time_start).count();

    // Notify metrics.
    notify_metrics();

    // Exchanges the notifier before notifying the reception of SCH.
    pusch_processor_result_notifier* notifier_ = nullptr;
    std::exchange(notifier_, notifier);

    // Notify the SCH reception.
    notifier_->on_sch(sch);
  }

  /// Notifies the gathered metrics through the given notifier.
  void notify_metrics()
  {
    static float invalid_sinr_and_evm = std::numeric_limits<float>::quiet_NaN();
    uint64_t     local_elapsed_return_ns(elapsed_return_ns);
    uint64_t     local_elapsed_data_ns(elapsed_data_ns);

    // Make sure that the processor returned and notified the completion of data.
    if ((local_elapsed_return_ns == 0) || (local_elapsed_data_ns == 0)) {
      return;
    }

    // If there is thread contention, ensure only one thread reaches this point.
    bool success = elapsed_return_ns.compare_exchange_weak(local_elapsed_return_ns, 0);
    if (!success) {
      return;
    }

    // Load the elapsed time to deliver UCI. It is present only if it is not zero.
    uint64_t                                local_elapsed_uci_ns = elapsed_uci_ns.load();
    std::optional<std::chrono::nanoseconds> elapsed_uci;
    if (local_elapsed_uci_ns != 0) {
      elapsed_uci.emplace(local_elapsed_uci_ns);
    }

    float sinr_dB = sch_result.csi.get_sinr_dB().value_or(invalid_sinr_and_evm);
    float evm     = sch_result.csi.get_total_evm().value_or(invalid_sinr_and_evm);
    bool  crc_ok  = sch_result.data.tb_crc_ok;

    metric_notifier.on_new_metric({.slot              = pdu.slot,
                                   .tbs               = units::bytes(data.size()),
                                   .crc_ok            = crc_ok,
                                   .elapsed_return    = std::chrono::nanoseconds(local_elapsed_return_ns),
                                   .elapsed_data      = std::chrono::nanoseconds(local_elapsed_data_ns),
                                   .elapsed_uci       = elapsed_uci,
                                   .cpu_time_usage_ns = cpu_time_usage_ns,
                                   .sinr_dB           = sinr_dB,
                                   .evm               = evm});
  }

  std::unique_ptr<pusch_processor>      processor;
  span<uint8_t>                         data;
  pdu_t                                 pdu;
  pusch_processor_result_notifier*      notifier;
  std::chrono::steady_clock::time_point time_start;
  std::atomic<uint64_t>                 elapsed_uci_ns;
  std::atomic<uint64_t>                 elapsed_data_ns;
  std::atomic<uint64_t>                 elapsed_return_ns;
  std::atomic<uint64_t>                 cpu_time_usage_ns;
  pusch_processor_metric_notifier&      metric_notifier;
  pusch_processor_result_data           sch_result;

  // Makes sure atomics are lock free.
  static_assert(std::atomic<decltype(elapsed_uci_ns)>::is_always_lock_free);
  static_assert(std::atomic<decltype(elapsed_data_ns)>::is_always_lock_free);
  static_assert(std::atomic<decltype(elapsed_return_ns)>::is_always_lock_free);
  static_assert(std::atomic<decltype(cpu_time_usage_ns)>::is_always_lock_free);
};

} // namespace srsran
