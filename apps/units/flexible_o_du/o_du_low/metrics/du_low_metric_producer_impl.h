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

#include "channel_equalizer_metrics_producer.h"
#include "channel_precoder_metrics_producer.h"
#include "crc_calculator_metrics_producer.h"
#include "ldpc_decoder_metrics_producer.h"
#include "ldpc_encoder_metrics_producer.h"
#include "pseudo_random_sequence_metrics_producer.h"
#include "pusch_channel_estimator_metrics_producer.h"
#include "pusch_processor_metrics_producer.h"
#include "fmt/format.h"

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
    std::chrono::nanoseconds pusch_scrambling_total_time        = pusch_scrambling_producer.get_total_time();
    std::chrono::nanoseconds pusch_equalizer_total_time         = pusch_channel_equalizer_producer.get_total_time();

    double ldpc_decoder_percent = 100.0 * static_cast<double>(ldpc_decoder_total_time.count()) /
                                  static_cast<double>(pusch_processor_total_time.count());
    double pusch_channel_estimator_percent = 100.0 * static_cast<double>(pusch_channel_estimator_total_time.count()) /
                                             static_cast<double>(pusch_processor_total_time.count());
    double pusch_scrambling_percent = 100.0 * static_cast<double>(pusch_scrambling_total_time.count()) /
                                      static_cast<double>(pusch_processor_total_time.count());
    double pusch_equalizer_percent = 100.0 * static_cast<double>(pusch_equalizer_total_time.count()) /
                                     static_cast<double>(pusch_processor_total_time.count());

    fmt::print("--- LDPC Encoder metrics (per codeblock) ---\n");
    fmt::print("     Avg. CB size: {:.2f} bits\n", ldpc_encoder_producer.get_avg_cb_size());
    fmt::print("     Avg. latency: {:.2f} us\n", ldpc_encoder_producer.get_avg_cb_latency_us());
    fmt::print("      Encode rate: {:.2f} Mbps\n", ldpc_encoder_producer.get_encode_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- CRC calculator ---\n");
    fmt::print("   PDSCH CRC16 Avg. Latency: {:.2f} us\n",
               pdsch_crc_calculator_producer.get_avg_latency_us(crc_generator_poly::CRC16));
    fmt::print("                  Avg. Rate: {:.2f} Mbps\n",
               pdsch_crc_calculator_producer.get_avg_rate_Mbps(crc_generator_poly::CRC16));
    fmt::print("  PDSCH CRC24A Avg. Latency: {:.2f} us\n",
               pdsch_crc_calculator_producer.get_avg_latency_us(crc_generator_poly::CRC24A));
    fmt::print("                  Avg. Rate: {:.2f} Mbps\n",
               pdsch_crc_calculator_producer.get_avg_rate_Mbps(crc_generator_poly::CRC24A));
    fmt::print("  PDSCH CRC24B Avg. Latency: {:.2f} us\n",
               pdsch_crc_calculator_producer.get_avg_latency_us(crc_generator_poly::CRC24B));
    fmt::print("                  Avg. Rate: {:.2f} Mbps\n",
               pdsch_crc_calculator_producer.get_avg_rate_Mbps(crc_generator_poly::CRC24B));
    fmt::print("   PUSCH CRC16 Avg. Latency: {:.2f} us\n",
               pusch_crc_calculator_producer.get_avg_latency_us(crc_generator_poly::CRC16));
    fmt::print("                  Avg. Rate: {:.2f} Mbps\n",
               pusch_crc_calculator_producer.get_avg_rate_Mbps(crc_generator_poly::CRC16));
    fmt::print("  PUSCH CRC24A Avg. Latency: {:.2f} us\n",
               pusch_crc_calculator_producer.get_avg_latency_us(crc_generator_poly::CRC24A));
    fmt::print("                  Avg. Rate: {:.2f} Mbps\n",
               pusch_crc_calculator_producer.get_avg_rate_Mbps(crc_generator_poly::CRC24A));
    fmt::print("  PUSCH CRC24B Avg. Latency: {:.2f} us\n",
               pusch_crc_calculator_producer.get_avg_latency_us(crc_generator_poly::CRC24B));
    fmt::print("                  Avg. Rate: {:.2f} Mbps\n",
               pusch_crc_calculator_producer.get_avg_rate_Mbps(crc_generator_poly::CRC24B));
    fmt::print("\n");
    fmt::print("--- LDPC Decoder metrics (per codeblock) ---\n");
    fmt::print("     Avg. CB size: {:.2f} bits\n", ldpc_decoder_producer.get_avg_cb_size());
    fmt::print("     Avg. latency: {:.2f} us\n", ldpc_decoder_producer.get_avg_cb_latency_us());
    fmt::print("  Avg. iterations: {:.2f} iter\n", ldpc_decoder_producer.get_avg_nof_iterations());
    fmt::print("      Decode rate: {:.2f} Mbps\n", ldpc_decoder_producer.get_decode_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- PUSCH Channel estimator metrics ---\n");
    fmt::print("          Avg. latency: {:.2f} us\n", pusch_channel_estimator_producer.get_average_latency());
    fmt::print("  Avg. processing rate: {:.2f} MPRBps\n", pusch_channel_estimator_producer.get_processing_rate());
    fmt::print("\n");
    fmt::print("--- PUSCH channel equalizer metrics ---\n");
    fmt::print("  Processing rate - one layer: {:.2f} MREps\n",
               pusch_channel_equalizer_producer.get_average_rate_MRes(1));
    fmt::print("                   two layers: {:.2f} MREps\n",
               pusch_channel_equalizer_producer.get_average_rate_MRes(2));
    fmt::print("                 three layers: {:.2f} MREps\n",
               pusch_channel_equalizer_producer.get_average_rate_MRes(3));
    fmt::print("                  four layers: {:.2f} MREps\n",
               pusch_channel_equalizer_producer.get_average_rate_MRes(4));
    fmt::print("\n");
    fmt::print("--- Pseudo-random generator metrics ---\n");
    fmt::print("      Avg. PUSCH init time: {:.2f} us\n", pusch_scrambling_producer.get_average_init_time_us());
    fmt::print("   Avg. PUSCH advance rate: {:.2f} Mbps\n", pusch_scrambling_producer.get_advance_rate_Mbps());
    fmt::print("  Avg. PUSCH generate rate: {:.2f} Mbps\n", pusch_scrambling_producer.get_generate_rate_Mbps());
    fmt::print("      Avg. PDSCH init time: {:.2f} us\n", pdsch_scrambling_producer.get_average_init_time_us());
    fmt::print("   Avg. PDSCH advance rate: {:.2f} Mbps\n", pdsch_scrambling_producer.get_advance_rate_Mbps());
    fmt::print("  Avg. PDSCH generate rate: {:.2f} Mbps\n", pdsch_scrambling_producer.get_generate_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- PUSCH Processor metrics ---\n");
    fmt::print("           Avg. data latency: {:.2f} us\n", pusch_processor_producer.get_avg_data_latency_us());
    fmt::print("            Avg. UCI latency: {:.2f} us\n", pusch_processor_producer.get_avg_uci_latency_us());
    fmt::print("  Avg. processing PUSCH rate: {:.2f} Mbps\n", pusch_processor_producer.get_processing_rate_Mbps());
    fmt::print("                Decoder time: {:.2f} % (asynchronous)\n", ldpc_decoder_percent);
    fmt::print("      Channel estimator time: {:.2f} %\n", pusch_channel_estimator_percent);
    fmt::print("      Channel equalizer time: {:.2f} %\n", pusch_equalizer_percent);
    fmt::print("             Scrambling time: {:.2f} %\n", pusch_scrambling_percent);
    fmt::print("\n");
    fmt::print("--- PDSCH Layer mapping and precoding metrics ---\n");
    fmt::print("  Processing rate - one layer: {:.2f} MREps\n",
               pdsch_channel_precoder_producer.get_average_rate_MRes(1));
    fmt::print("                   two layers: {:.2f} MREps\n",
               pdsch_channel_precoder_producer.get_average_rate_MRes(2));
    fmt::print("                 three layers: {:.2f} MREps\n",
               pdsch_channel_precoder_producer.get_average_rate_MRes(3));
    fmt::print("                  four layers: {:.2f} MREps\n",
               pdsch_channel_precoder_producer.get_average_rate_MRes(4));
    fmt::print("\n");
  }

  upper_phy_metrics_notifiers& get_notifiers() { return *this; }

private:
  // See interface for documentation.
  crc_calculator_metric_notifier& get_pdsch_crc_calculator_notifier() override
  {
    return pdsch_crc_calculator_producer.get_notifier();
  }

  // See interface for documentation.
  crc_calculator_metric_notifier& get_pusch_crc_calculator_notifier() override
  {
    return pusch_crc_calculator_producer.get_notifier();
  }

  // See interface for documentation.
  ldpc_decoder_metric_notifier& get_ldpc_decoder_notifier() override { return ldpc_decoder_producer.get_notifier(); }

  // See interface for documentation.
  ldpc_encoder_metric_notifier& get_ldpc_encoder_notifier() override { return ldpc_encoder_producer.get_notifier(); }

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

  // See interface for documentation.
  pseudo_random_sequence_generator_metric_notifier& get_pusch_scrambling_notifier() override
  {
    return pusch_scrambling_producer.get_notifier();
  }

  // See interface for documentation.
  channel_equalizer_metric_notifier& get_pusch_channel_equalizer_notifier() override
  {
    return pusch_channel_equalizer_producer.get_notifier();
  }

  // See interface for documentation.
  pseudo_random_sequence_generator_metric_notifier& get_pdsch_scrambling_notifier() override
  {
    return pdsch_scrambling_producer.get_notifier();
  }

  // See interface for documentation.
  channel_precoder_metric_notifier& get_pdsch_channel_precoder_notifier() override
  {
    return pdsch_channel_precoder_producer.get_notifier();
  }

  /// PDSCH CRC calculator metric producer implementation.
  crc_calculator_metric_producer_impl pdsch_crc_calculator_producer;
  /// PUSCH CRC calculator metric producer implementation.
  crc_calculator_metric_producer_impl pusch_crc_calculator_producer;
  /// LDPC decoder metric producer implementation.
  ldpc_decoder_metric_producer_impl ldpc_decoder_producer;
  /// LDPC encoder metric producer implementation.
  ldpc_encoder_metric_producer_impl ldpc_encoder_producer;
  /// PUSCH channel estimator metric producer implementation.
  pusch_channel_estimator_metric_producer_impl pusch_channel_estimator_producer;
  /// PUSCH processor metric producer implementation.
  pusch_processor_metric_producer_impl pusch_processor_producer;
  /// PDSCH scrambling metric notifier.
  pseudo_random_sequence_metrics_producer_impl pdsch_scrambling_producer;
  /// PUSCH scrambling metric notifier.
  pseudo_random_sequence_metrics_producer_impl pusch_scrambling_producer;
  /// PUSCH channel equalizer metric notifier.
  channel_equalizer_metric_producer_impl pusch_channel_equalizer_producer;
  /// PDSCH channel precoder notifier.
  channel_precoder_metric_producer_impl pdsch_channel_precoder_producer;
}; // namespace srsran

} // namespace srsran
