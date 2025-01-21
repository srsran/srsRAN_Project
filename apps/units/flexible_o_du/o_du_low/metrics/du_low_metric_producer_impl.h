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

#include "ldpc_decoder_metrics_producer.h"
#include "pusch_channel_estimator_metrics_producer.h"
#include "pusch_processor_metrics_producer.h"
#include <fmt/format.h>

namespace srsran {

/// Implements upper physical layer notifiers.
class du_low_metric_producer_impl : public app_services::metrics_producer, private upper_phy_metrics_notifiers
{
public:
  /// Default constructor.
  du_low_metric_producer_impl() = default;

  /// Default destructor - prints current metrics.
  ~du_low_metric_producer_impl() override
  {
    std::chrono::nanoseconds ldpc_decoder_total_time            = ldpc_decoder_producer.get_total_time();
    std::chrono::nanoseconds pusch_channel_estimator_total_time = pusch_channel_estimator_producer.get_total_time();
    std::chrono::nanoseconds pusch_processor_total_time         = pusch_processor_producer.get_total_time();

    double ldpc_decoder_percent = 100.0 * static_cast<double>(ldpc_decoder_total_time.count()) /
                                  static_cast<double>(pusch_processor_total_time.count());
    double pusch_channel_estimator_percent = 100.0 * static_cast<double>(pusch_channel_estimator_total_time.count()) /
                                             static_cast<double>(pusch_processor_total_time.count());

    fmt::print("--- LDPC Decoder metrics ---\n");
    fmt::print("  Avg. CB size: {:.2f} bits/codeblock\n", ldpc_decoder_producer.get_avg_cb_size());
    fmt::print("  Avg. latency: {:.2f} us/codeblock\n", ldpc_decoder_producer.get_avg_cb_latency_us());
    fmt::print("   Decode rate: {:.2f} Mbps\n", ldpc_decoder_producer.get_decode_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- Channel estimator metrics ---\n");
    fmt::print("          Avg. latency: {:.2f} us\n", pusch_channel_estimator_producer.get_average_latency());
    fmt::print("  Avg. processing rate: {:.2f} MPRBps\n", pusch_channel_estimator_producer.get_processing_rate());
    fmt::print("\n");
    fmt::print("--- PUSCH Processor metrics ---\n");
    fmt::print("           Avg. data latency: {:.2f} us\n", pusch_processor_producer.get_avg_data_latency_us());
    fmt::print("            Avg. UCI latency: {:.2f} us\n", pusch_processor_producer.get_avg_uci_latency_us());
    fmt::print("  Avg. processing PUSCH rate: {:.2f} Mbps\n", pusch_processor_producer.get_processing_rate_Mbps());
    fmt::print("                Decoder time: {:.2f} %\n", ldpc_decoder_percent);
    fmt::print("      Channel estimator time: {:.2f} %\n", pusch_channel_estimator_percent);
    fmt::print("\n");
  }

  upper_phy_metrics_notifiers& get_notifiers() { return *this; }

private:
  // See interface for documentation.
  ldpc_decoder_metric_notifier& get_ldpc_decoder_notifier() override { return ldpc_decoder_producer.get_notifier(); }

  // See interface for documentation.
  pusch_processor_metric_notifier& get_pusch_processor_notifier() override
  {
    return pusch_processor_producer.get_notifier();
  }
  // See interface for documentation.
  pusch_channel_estimator_metric_notifier& get_pusch_channel_estimator_notifier() override
  {
    return pusch_channel_estimator_producer.get_notifier();
  }

  /// LDPC decoder metric producer implementation.
  ldpc_decoder_metric_producer_impl ldpc_decoder_producer;
  /// PUSCH channel estimator metric producer implementation.
  pusch_channel_estimator_metric_producer_impl pusch_channel_estimator_producer;
  /// PUSCH processor metric producer implementation.
  pusch_processor_metric_producer_impl pusch_processor_producer;
};

} // namespace srsran
