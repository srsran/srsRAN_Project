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
    time_data         = 0;
    time_uci          = 0;
    time_return       = 0;
    cpu_time_usage_ns = 0;

    // Clear processor results.
    sch_result.reset();
    uci_result.reset();

    resource_usage_utils::measurements measurements;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(measurements);
      processor->process(data, std::move(rm_buffer), *this, grid, pdu);
    }
    cpu_time_usage_ns = measurements.duration.count();
    time_return       = std::chrono::steady_clock::now().time_since_epoch().count();

    // Notify metrics.
    notify_metrics();
  }

private:
  // See pusch_processor_result_notifier for documentation.
  void on_uci(const pusch_processor_result_control& uci) override
  {
    srsran_assert(notifier, "Invalid notifier");

    // Save UCI reporting time.
    time_uci = std::chrono::steady_clock::now().time_since_epoch().count();

    // Notify higher layers.
    notifier->on_uci(uci);

    // Save results.
    uci_result = uci;
  }

  // See pusch_processor_result_notifier for documentation.
  void on_sch(const pusch_processor_result_data& sch) override
  {
    srsran_assert(notifier, "Invalid notifier");

    // Save data reporting time.
    time_data = std::chrono::steady_clock::now().time_since_epoch().count();

    // Save SCH results.
    sch_result = sch;

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
    // Check if the processing function returned.
    std::chrono::time_point<std::chrono::steady_clock> time_return_local =
        std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::duration(time_return));
    if (time_return_local == std::chrono::time_point<std::chrono::steady_clock>()) {
      // Skip notification if return did not happen.
      return;
    }

    // Check if data was notified.
    std::chrono::time_point<std::chrono::steady_clock> time_data_local =
        std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::duration(time_data));
    if (time_data_local == std::chrono::time_point<std::chrono::steady_clock>()) {
      // Skip notification if data decode did not happen.
      return;
    }

    std::optional<std::chrono::nanoseconds> elapsed_uci;
    if (uci_result.has_value()) {
      std::chrono::time_point<std::chrono::steady_clock> time_uci_local =
          std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::duration(time_uci));

      elapsed_uci = time_uci_local - time_start;
    }

    float sinr_dB = std::numeric_limits<float>::quiet_NaN();
    float evm     = std::numeric_limits<float>::quiet_NaN();
    if (sch_result.has_value()) {
      sinr_dB = sch_result.value().csi.get_sinr_dB().value_or(0.0);
      evm     = sch_result.value().csi.get_evm().value_or(0.0);
    }

    metric_notifier.on_new_metric({.slot              = pdu.slot,
                                   .tbs               = units::bytes(data.size()),
                                   .crc_ok            = sch_result.value().data.tb_crc_ok,
                                   .elapsed_return    = time_return_local - time_start,
                                   .elapsed_data      = time_data_local - time_start,
                                   .elapsed_uci       = elapsed_uci,
                                   .cpu_time_usage_ns = cpu_time_usage_ns,
                                   .sinr_dB           = sinr_dB,
                                   .evm               = evm});
  }

  std::unique_ptr<pusch_processor>                      processor;
  span<uint8_t>                                         data;
  pdu_t                                                 pdu;
  pusch_processor_result_notifier*                      notifier;
  std::chrono::steady_clock::time_point                 time_start;
  std::atomic<uint64_t>                                 time_uci;
  std::atomic<uint64_t>                                 time_data;
  std::atomic<uint64_t>                                 time_return;
  std::atomic<uint64_t>                                 cpu_time_usage_ns;
  pusch_processor_metric_notifier&                      metric_notifier;
  std::optional<srsran::pusch_processor_result_control> uci_result;
  std::optional<srsran::pusch_processor_result_data>    sch_result;

  // Makes sure atomics are lock free.
  static_assert(std::atomic<decltype(time_uci)>::is_always_lock_free);
  static_assert(std::atomic<decltype(time_data)>::is_always_lock_free);
  static_assert(std::atomic<decltype(time_return)>::is_always_lock_free);
  static_assert(std::atomic<decltype(cpu_time_usage_ns)>::is_always_lock_free);
};

} // namespace srsran
