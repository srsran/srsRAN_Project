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
    cpu_time_usage_us = 0;

    // Clear processor results.
    sch_result.reset();
    uci_result.reset();

    resource_usage_utils::measurements cpu_measurements;
    {
      // Use scoped resource usage class to measure CPU usage of this block.
      resource_usage_utils::scoped_resource_usage rusage_tracker(cpu_measurements,
                                                                 resource_usage_utils::rusage_measurement_type::THREAD);
      processor->process(data, std::move(rm_buffer), *this, grid, pdu);
    }
    cpu_time_usage_us = (cpu_measurements.user_time + cpu_measurements.system_time).count();
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

    pusch_processor_metrics metrics;
    metrics.tbs               = units::bytes(data.size());
    metrics.crc_ok            = sch_result.value().data.tb_crc_ok;
    metrics.elapsed_return    = time_return_local - time_start;
    metrics.elapsed_data      = time_data_local - time_start;
    metrics.cpu_time_usage_us = cpu_time_usage_us;

    if (uci_result.has_value()) {
      std::chrono::time_point<std::chrono::steady_clock> time_uci_local =
          std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::duration(time_uci));

      metrics.elapsed_uci = time_uci_local - time_start;
    }

    if (sch_result.has_value()) {
      metrics.sinr_dB = sch_result.value().csi.get_sinr_dB().value_or(0.0);
      metrics.evm     = sch_result.value().csi.get_evm().value_or(0.0);
    }

    metric_notifier.on_new_metric(metrics);
  }

  std::unique_ptr<pusch_processor>                      processor;
  span<uint8_t>                                         data;
  pdu_t                                                 pdu;
  pusch_processor_result_notifier*                      notifier;
  std::chrono::steady_clock::time_point                 time_start;
  std::atomic<uint64_t>                                 time_uci;
  std::atomic<uint64_t>                                 time_data;
  std::atomic<uint64_t>                                 time_return;
  std::atomic<uint64_t>                                 cpu_time_usage_us;
  pusch_processor_metric_notifier&                      metric_notifier;
  std::optional<srsran::pusch_processor_result_control> uci_result;
  std::optional<srsran::pusch_processor_result_data>    sch_result;

  // Makes sure atomics are lock free.
  static_assert(std::atomic<decltype(time_uci)>::is_always_lock_free);
  static_assert(std::atomic<decltype(time_data)>::is_always_lock_free);
  static_assert(std::atomic<decltype(time_return)>::is_always_lock_free);
  static_assert(std::atomic<decltype(cpu_time_usage_us)>::is_always_lock_free);
};

} // namespace srsran
