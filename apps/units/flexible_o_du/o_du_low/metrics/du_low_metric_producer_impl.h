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

#include "channel_equalizer_metrics_producer.h"
#include "channel_modulation_metrics_producer.h"
#include "channel_precoder_metrics_producer.h"
#include "crc_calculator_metrics_producer.h"
#include "ldpc_decoder_metrics_producer.h"
#include "ldpc_encoder_metrics_producer.h"
#include "ldpc_rate_dematcher_metrics_producer.h"
#include "ldpc_rate_matcher_metrics_producer.h"
#include "pdsch_dmrs_generator_metrics_producer.h"
#include "pdsch_processor_metrics_producer.h"
#include "port_channel_estimator_metrics_producer.h"
#include "pseudo_random_sequence_metrics_producer.h"
#include "pusch_channel_estimator_metrics_producer.h"
#include "pusch_demodulator_metrics_producer.h"
#include "pusch_processor_metrics_producer.h"
#include "time_alignment_estimator_metrics_producer.h"
#include "transform_precoder_metrics_producer.h"
#include "ulsch_demultiplex_metrics_producer.h"
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
    std::chrono::nanoseconds ldpc_decoder_total_time              = ldpc_decoder_producer.get_total_time();
    std::chrono::nanoseconds pusch_channel_estimator_total_time   = pusch_channel_estimator_producer.get_total_time();
    std::chrono::nanoseconds pusch_processor_total_time           = pusch_processor_producer.get_total_time();
    std::chrono::nanoseconds pusch_scrambling_total_time          = pusch_scrambling_producer.get_total_time();
    std::chrono::nanoseconds pusch_equalizer_total_time           = pusch_channel_equalizer_producer.get_total_time();
    std::chrono::nanoseconds pusch_demodulation_total_time        = pusch_demodulation_mapper_producer.get_total_time();
    std::chrono::nanoseconds pusch_evm_calc_total_time            = pusch_evm_calculator_producer.get_total_time();
    std::chrono::nanoseconds pusch_waiting_time                   = pusch_processor_producer.get_total_wait_time();
    std::chrono::nanoseconds ulsch_demux_total_time               = ulsch_demultiplex_producer.get_total_time();
    std::chrono::nanoseconds pusch_demodulator_total_time         = pusch_demodulator_producer.get_total_time();
    std::chrono::nanoseconds pusch_ta_estimator_total_time        = pusch_ta_estimator_producer.get_total_time();
    std::chrono::nanoseconds pusch_port_ch_estimator_total_time   = pusch_port_ch_estimator_producer.get_total_time();
    std::chrono::nanoseconds pusch_transform_precoding_total_time = pusch_transform_precoder_producer.get_total_time();

    double ldpc_decoder_percent = 100.0 * static_cast<double>(ldpc_decoder_total_time.count()) /
                                  static_cast<double>(pusch_processor_total_time.count());
    double pusch_channel_estimator_percent = 100.0 * static_cast<double>(pusch_channel_estimator_total_time.count()) /
                                             static_cast<double>(pusch_processor_total_time.count());
    double pusch_scrambling_percent = 100.0 * static_cast<double>(pusch_scrambling_total_time.count()) /
                                      static_cast<double>(pusch_processor_total_time.count());
    double pusch_equalizer_percent = 100.0 * static_cast<double>(pusch_equalizer_total_time.count()) /
                                     static_cast<double>(pusch_processor_total_time.count());
    double pusch_demodulation_percent = 100.0 * static_cast<double>(pusch_demodulation_total_time.count()) /
                                        static_cast<double>(pusch_processor_total_time.count());
    double pusch_evm_calc_percent = 100.0 * static_cast<double>(pusch_evm_calc_total_time.count()) /
                                    static_cast<double>(pusch_processor_total_time.count());
    double pusch_wait_time_percent = 100.0 * static_cast<double>(pusch_waiting_time.count()) /
                                     static_cast<double>(pusch_processor_total_time.count());
    double ulsch_demux_time_percent = 100.0 * static_cast<double>(ulsch_demux_total_time.count()) /
                                      static_cast<double>(pusch_processor_total_time.count());
    double pusch_demodulator_time_percent = 100.0 * static_cast<double>(pusch_demodulator_total_time.count()) /
                                            static_cast<double>(pusch_processor_total_time.count());
    double pusch_ta_estimator_time_percent = 100.0 * static_cast<double>(pusch_ta_estimator_total_time.count()) /
                                             static_cast<double>(pusch_processor_total_time.count());
    double pusch_port_ch_estimator_time_percent = 100.0 *
                                                  static_cast<double>(pusch_port_ch_estimator_total_time.count()) /
                                                  static_cast<double>(pusch_processor_total_time.count());
    double pusch_transform_precoding_time_percent = 100.0 *
                                                    static_cast<double>(pusch_transform_precoding_total_time.count()) /
                                                    static_cast<double>(pusch_processor_total_time.count());

    fmt::print("--- LDPC Encoder metrics (per codeblock) ---\n");
    fmt::print("     Avg. CB size: {:.2f} bits\n", ldpc_encoder_producer.get_avg_cb_size());
    fmt::print("     Avg. latency: {:.2f} us\n", ldpc_encoder_producer.get_avg_cb_latency_us());
    fmt::print("      Encode rate: {:.2f} Mbps\n", ldpc_encoder_producer.get_encode_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- LDPC Rate matcher (per codeblock) ---\n");
    fmt::print("     Avg. latency: {:.2f} us\n", ldpc_rate_matcher_producer.get_avg_cb_latency_us());
    fmt::print("  Processing rate: {:.2f} Mbps\n", ldpc_rate_matcher_producer.get_avg_rate_Mbps());
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
    fmt::print("--- LDPC Rate dematcher (per codeblock) ---\n");
    fmt::print("     Avg. latency: {:.2f} us\n", ldpc_rate_dematcher_producer.get_avg_cb_latency_us());
    fmt::print("  Processing rate: {:.2f} Mbps\n", ldpc_rate_dematcher_producer.get_avg_rate_Mbps());
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
    fmt::print("--- Transform precoding ---\n");
    fmt::print(" Avg. Transform rate: {:.2f} MREps\n", pusch_transform_precoder_producer.get_avg_rate_MREps());
    fmt::print(" Avg. Transform latency: {:.2f} us/symbol\n", pusch_transform_precoder_producer.get_avg_latency_us());
    fmt::print("\n");
    fmt::print("--- Modulation, demodulation and EVM calculation ---\n");
    fmt::print("      Modulation rate QPSK: {:.2f} Mbps\n",
               pdsch_modulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QPSK));
    fmt::print("                    16-QAM: {:.2f} Mbps\n",
               pdsch_modulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QAM16));
    fmt::print("                    64-QAM: {:.2f} Mbps\n",
               pdsch_modulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QAM64));
    fmt::print("                   256-QAM: {:.2f} Mbps\n",
               pdsch_modulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QAM256));
    fmt::print("    Demodulation rate QPSK: {:.2f} Mbps\n",
               pusch_demodulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QPSK));
    fmt::print("                    16-QAM: {:.2f} Mbps\n",
               pusch_demodulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QAM16));
    fmt::print("                    64-QAM: {:.2f} Mbps\n",
               pusch_demodulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QAM64));
    fmt::print("                   256-QAM: {:.2f} Mbps\n",
               pusch_demodulation_mapper_producer.get_avg_rate_Mbps(modulation_scheme::QAM256));
    fmt::print("  EVM calculator rate QPSK: {:.2f} Mbps\n",
               pusch_evm_calculator_producer.get_avg_rate_Mbps(modulation_scheme::QPSK));
    fmt::print("                    16-QAM: {:.2f} Mbps\n",
               pusch_evm_calculator_producer.get_avg_rate_Mbps(modulation_scheme::QAM16));
    fmt::print("                    64-QAM: {:.2f} Mbps\n",
               pusch_evm_calculator_producer.get_avg_rate_Mbps(modulation_scheme::QAM64));
    fmt::print("                   256-QAM: {:.2f} Mbps\n",
               pusch_evm_calculator_producer.get_avg_rate_Mbps(modulation_scheme::QAM256));
    fmt::print("\n");
    fmt::print("--- UL-SCH demux ---\n");
    fmt::print("        Avg. init time: {:.2f} us\n", ulsch_demultiplex_producer.get_average_init_time_us());
    fmt::print("      Avg. finish time: {:.2f} us\n", ulsch_demultiplex_producer.get_average_finish_time_us());
    fmt::print("  Avg. processing rate: {:.2f} Mbps\n", ulsch_demultiplex_producer.get_average_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- PUSCH Processor metrics ---\n");
    fmt::print("           Avg. data latency: {:.2f} us\n", pusch_processor_producer.get_avg_data_latency_us());
    fmt::print("            Avg. UCI latency: {:.2f} us\n", pusch_processor_producer.get_avg_uci_latency_us());
    fmt::print("  Avg. processing PUSCH rate: {:.2f} Mbps\n", pusch_processor_producer.get_processing_rate_Mbps());
    fmt::print("                Decoder time: {:.2f} % (asynchronous)\n", ldpc_decoder_percent);
    fmt::print("\n");
    fmt::print("--- PUSCH Processor breakdown ---\n");
    fmt::print("   |-> Channel estimator: {:.2f} %\n", pusch_channel_estimator_percent);
    fmt::print("     |-> Ch. estimator: {:.2f} %\n", pusch_port_ch_estimator_time_percent);
    fmt::print("       |-> TA estimator: {:.2f} %\n", pusch_ta_estimator_time_percent);
    fmt::print("   |-> Demodulation: {:.2f} %\n", pusch_demodulator_time_percent);
    fmt::print("     |---> Channel equalizer: {:.2f} %\n", pusch_equalizer_percent);
    fmt::print("     |-> Demodulation mapper: {:.2f} %\n", pusch_demodulation_percent);
    fmt::print("     |-----> EVM calculation: {:.2f} %\n", pusch_evm_calc_percent);
    fmt::print("     |----------> Scrambling: {:.2f} %\n", pusch_scrambling_percent);
    fmt::print("     |-> Transform precoding: {:.2f} %\n", pusch_transform_precoding_time_percent);
    fmt::print("   |-> UL-SCH Demux: {:.2f} %\n", ulsch_demux_time_percent);
    fmt::print("   |------> Decoder: {:.2f} % (multi-threaded)\n", pusch_wait_time_percent);
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
    fmt::print("--- PDSCH Processor metrics ---\n");
    fmt::print("  Avg. processing latency: {:.2f} us\n", pdsch_processor_producer.get_avg_latency_us());
    fmt::print("      Avg. return latency: {:.2f} us\n", pdsch_processor_producer.get_avg_return_time_us());
    fmt::print("     Avg. processing rate: {:.2f} Mbps\n", pdsch_processor_producer.get_process_rate_Mbps());
    fmt::print("\n");
    fmt::print("--- DM-RS for PDSCH generator metrics ---\n");
    fmt::print("  Avg. generation latency: {:.2f} us\n", pdsch_dmrs_generator_producer.get_avg_latency_us());
    fmt::print("\n");
  }

  // See interface for documentation.
  void on_new_report_period() override {}

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

  // See interface for documentation.
  common_channel_modulation_metric_notifier& get_pdsch_modulation_mapper_notifier() override
  {
    return pdsch_modulation_mapper_producer.get_notifier();
  }

  // See interface for documentation.
  common_channel_modulation_metric_notifier& get_pusch_demodulation_mapper_notifier() override
  {
    return pusch_demodulation_mapper_producer.get_notifier();
  }

  // See interface for documentation.
  common_channel_modulation_metric_notifier& get_pusch_evm_calculator_notifier() override
  {
    return pusch_evm_calculator_producer.get_notifier();
  }

  // See interface for documentation.
  ulsch_demultiplex_metric_notifier& get_ulsch_demultiplex_notifier() override
  {
    return ulsch_demultiplex_producer.get_notifier();
  }

  // See interface for documentation.
  pusch_demodulator_metric_notifier& get_pusch_demodulator_notifier() override
  {
    return pusch_demodulator_producer.get_notifier();
  }

  // See interface for documentation.
  time_alignment_estimator_metric_notifier& get_pusch_ta_estimator_notifier() override
  {
    return pusch_ta_estimator_producer.get_notifier();
  }

  // See interface for documentation.
  port_channel_estimator_metric_notifier& get_pusch_port_channel_estimator_notifier() override
  {
    return pusch_port_ch_estimator_producer.get_notifier();
  }

  // See interface for documentation.
  transform_precoder_metric_notifier& get_pusch_transform_precoder_notifier() override
  {
    return pusch_transform_precoder_producer.get_notifier();
  }

  // See interface for documentation.
  ldpc_rate_matcher_metric_notifier& get_ldpc_rate_matcher_notifier() override
  {
    return ldpc_rate_matcher_producer.get_notifier();
  }

  // See interface for documentation.
  ldpc_rate_dematcher_metric_notifier& get_ldpc_rate_dematcher_notifier() override
  {
    return ldpc_rate_dematcher_producer.get_notifier();
  }

  // See interface for documentation.
  pdsch_processor_metric_notifier& get_pdsch_processor_notifier() override
  {
    return pdsch_processor_producer.get_notifier();
  }

  // See interface for documentation.
  pdsch_dmrs_generator_metric_notifier& get_pdsch_dmrs_generator() override
  {
    return pdsch_dmrs_generator_producer.get_notifier();
  }

  /// PDSCH CRC calculator metric producer implementation.
  crc_calculator_metric_producer_impl pdsch_crc_calculator_producer;
  /// PUSCH CRC calculator metric producer implementation.
  crc_calculator_metric_producer_impl pusch_crc_calculator_producer;
  /// LDPC decoder metric producer implementation.
  ldpc_decoder_metric_producer_impl ldpc_decoder_producer;
  /// LDPC rate matcher producer implementation.
  ldpc_rate_matcher_metric_producer_impl ldpc_rate_matcher_producer;
  /// LDPC rate dematcher producer implementation.
  ldpc_rate_dematcher_metric_producer_impl ldpc_rate_dematcher_producer;
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
  /// PDSCH modulation mapper notifier.
  channel_modulation_metric_producer_impl pdsch_modulation_mapper_producer;
  /// PUSCH demodulation mapper notifier.
  channel_modulation_metric_producer_impl pusch_demodulation_mapper_producer;
  /// PUSCH EVM calculator notifier.
  channel_modulation_metric_producer_impl pusch_evm_calculator_producer;
  /// UL-SCH demultiplexer producer implementation.
  ulsch_demultiplex_metric_producer_impl ulsch_demultiplex_producer;
  /// PUSCH demodulator metric producer implementation.
  pusch_demodulator_metric_producer_impl pusch_demodulator_producer;
  /// Time alignment estimator metric producer.
  time_alignment_estimator_metric_producer_impl pusch_ta_estimator_producer;
  /// Port channel estimator metric producer.
  port_channel_estimator_metric_producer_impl pusch_port_ch_estimator_producer;
  /// PUSCH transform precoder metric producer.
  transform_precoder_metric_producer_impl pusch_transform_precoder_producer;
  /// PDSCH processor metric producer.
  pdsch_processor_metric_producer_impl pdsch_processor_producer;
  /// PDSCH DM-RS metric producer.
  pdsch_dmrs_generator_metric_producer_impl pdsch_dmrs_generator_producer;
}; // namespace srsran

} // namespace srsran
