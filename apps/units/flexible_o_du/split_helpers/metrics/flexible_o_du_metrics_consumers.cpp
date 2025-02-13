/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "flexible_o_du_metrics_consumers.h"
#include "flexible_o_du_app_service_metrics.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

void flexible_o_du_metrics_consumer_dummy::handle_metric(const app_services::metrics_set& metric)
{
  // Implement me!
}

static void log_handle_du_low_metrics(srslog::basic_logger& logger, const srs_du::o_du_low_metrics& du_lo)
{
  /// Size in bytes of the buffer used to prepare the metrics before logging.
  static constexpr unsigned str_buffer_size = 4096;

  if (du_lo.metrics_period == std::chrono::microseconds()) {
    return;
  }

  // The following variables are used to break-down PUSCH processing into smaller blocks.

  // Percent of total PUSCH processing time used by the channel estimation.
  double channel_estimator_percent =
      100.0 * static_cast<double>(du_lo.pusch_metrics.ch_estimator_metrics.total_processing_latency.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by port channel estimation.
  double port_channel_estimator_percent =
      100.0 * static_cast<double>(du_lo.pusch_metrics.port_ch_estimator_metrics.total_processing_latency.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by time alignment estimator.
  double ta_estimator_percent = 100.0 *
                                static_cast<double>(du_lo.pusch_metrics.pusch_ta_est_metrics.total_proc_time.count()) /
                                static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by demodulator.
  double demodulator_percent = 100.0 *
                               static_cast<double>(du_lo.pusch_metrics.pusch_demod_metrics.total_proc_time.count()) /
                               static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by the channel equalizer.
  double channel_equalizer_percent =
      100.0 * static_cast<double>(du_lo.pusch_metrics.ch_equalizer_metrics.total_proc_time.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by demodulation mapper.
  double demodulation_mapper_percent =
      100.0 * static_cast<double>(du_lo.pusch_metrics.demod_demapper_metrics.total_proc_time.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by EVM calculation.
  double evm_calc_percent = 100.0 *
                            static_cast<double>(du_lo.pusch_metrics.evm_calculator_metrics.total_proc_time.count()) /
                            static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by the scrambling.
  double scrambling_percent = 100.0 *
                              static_cast<double>(du_lo.pusch_metrics.scrambling_metrics.total_proc_time.count()) /
                              static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by the transform precoding.
  double transform_precoding_percent =
      100.0 * static_cast<double>(du_lo.pusch_metrics.xform_precoder_metrics.total_proc_latency.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by ULSCH demultiplexing.
  double ulsch_demux_percent = 100.0 *
                               static_cast<double>(du_lo.pusch_metrics.ulsch_demux_metrics.total_proc_time.count()) /
                               static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  // Percent of total PUSCH processing time used by the LDPC decoder (asynchronous).
  double ldpc_decoding_percent = 100.0 *
                                 static_cast<double>(du_lo.ldpc_metrics.decoder_metrics.total_proc_time.count()) /
                                 static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  double pusch_wait_time_percent = 100.0 *
                                   static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_wait_time.count()) /
                                   static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count());

  fmt::basic_memory_buffer<char, str_buffer_size> buffer;

  fmt::format_to(std::back_inserter(buffer),
                 "\n--- DU low metrics accumulated over {}ms ---\n",
                 static_cast<double>(du_lo.metrics_period.count()) / 1000.0);

  const auto& ldpc_encoder = du_lo.ldpc_metrics.encoder_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- LDPC Encoder metrics (per codeblock) ---\n");
  fmt::format_to(std::back_inserter(buffer), "     Avg. CB size: {:.2f} bits\n", ldpc_encoder.avg_cb_size);
  fmt::format_to(std::back_inserter(buffer), "     Avg. latency: {:.2f} us\n", ldpc_encoder.avg_cb_latency);
  fmt::format_to(std::back_inserter(buffer), "      Encode rate: {:.2f} Mbps\n\n", ldpc_encoder.encoding_rate_Mbps);

  const auto& ldpc_rm = du_lo.ldpc_metrics.rate_match_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- LDPC Rate matcher (per codeblock) ---\n");
  fmt::format_to(std::back_inserter(buffer), "     Avg. latency: {:.2f} us\n", ldpc_rm.avg_cb_latency);
  fmt::format_to(std::back_inserter(buffer), "  Processing rate: {:.2f} Mbps\n\n", ldpc_rm.processing_rate_Mbps);

  const auto& ldpc_decoder = du_lo.ldpc_metrics.decoder_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- LDPC Decoder metrics (per codeblock) ---\n");
  fmt::format_to(std::back_inserter(buffer), "     Avg. CB size: {:.2f} bits\n", ldpc_decoder.avg_cb_size);
  fmt::format_to(std::back_inserter(buffer), "     Avg. latency: {:.2f} us\n", ldpc_decoder.avg_cb_latency_us);
  fmt::format_to(std::back_inserter(buffer), "     Min. latency: {:.2f} us\n", ldpc_decoder.min_cb_latency_us);
  fmt::format_to(std::back_inserter(buffer), "     Max. latency: {:.2f} us\n", ldpc_decoder.max_cb_latency_us);
  fmt::format_to(std::back_inserter(buffer), "  Avg. iterations: {:.2f} iter\n", ldpc_decoder.avg_nof_iterations);
  fmt::format_to(std::back_inserter(buffer), "      Decode rate: {:.2f} Mbps\n\n", ldpc_decoder.decoding_rate_Mbps);

  const auto& ldpc_rdm = du_lo.ldpc_metrics.rate_dematch_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- LDPC Rate dematcher (per codeblock) ---\n");
  fmt::format_to(std::back_inserter(buffer), "     Avg. latency: {:.2f} us\n", ldpc_rdm.avg_cb_latency);
  fmt::format_to(std::back_inserter(buffer), "  Processing rate: {:.2f} Mbps\n\n", ldpc_rdm.processing_rate_Mbps);

  const auto& crc_pdsch = du_lo.pdsch_metrics.crc_metrics;
  const auto& crc_pusch = du_lo.pusch_metrics.crc_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- CRC calculator ---\n");
  fmt::format_to(
      std::back_inserter(buffer), "   PDSCH CRC16 Avg. Latency: {:.2f} us\n", crc_pdsch.crc16_avg_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "                  Avg. Rate: {:.2f} Mbps\n", crc_pdsch.crc16_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "  PDSCH CRC24A Avg. Latency: {:.2f} us\n", crc_pdsch.crc24a_avg_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "                  Avg. Rate: {:.2f} Mbps\n", crc_pdsch.crc24a_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "  PDSCH CRC24B Avg. Latency: {:.2f} us\n", crc_pdsch.crc24b_avg_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "                  Avg. Rate: {:.2f} Mbps\n", crc_pdsch.crc24b_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "   PUSCH CRC16 Avg. Latency: {:.2f} us\n", crc_pusch.crc16_avg_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "                  Avg. Rate: {:.2f} Mbps\n", crc_pusch.crc16_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "  PUSCH CRC24A Avg. Latency: {:.2f} us\n", crc_pusch.crc24a_avg_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "                  Avg. Rate: {:.2f} Mbps\n", crc_pusch.crc24a_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "  PUSCH CRC24B Avg. Latency: {:.2f} us\n", crc_pusch.crc24b_avg_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "                  Avg. Rate: {:.2f} Mbps\n\n", crc_pusch.crc24b_avg_rate_Mbps);

  const auto& ch_est = du_lo.pusch_metrics.ch_estimator_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- PUSCH Channel estimator metrics ---\n");
  fmt::format_to(std::back_inserter(buffer), "          Avg. latency: {:.2f} us\n", ch_est.avg_processing_latency);
  fmt::format_to(std::back_inserter(buffer), "  Avg. processing rate: {:.2f} MPRBps\n\n", ch_est.processing_rate_Mbps);

  const auto& ch_eq = du_lo.pusch_metrics.ch_equalizer_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- PUSCH channel equalizer metrics ---\n");
  fmt::format_to(
      std::back_inserter(buffer), "  Processing rate - one layer: {:.2f} MREps\n", ch_eq.avg_rate_MREps_one_layer);
  fmt::format_to(
      std::back_inserter(buffer), "                   two layers: {:.2f} MREps\n", ch_eq.avg_rate_MREps_two_layers);
  fmt::format_to(
      std::back_inserter(buffer), "                 three layers: {:.2f} MREps\n", ch_eq.avg_rate_MREps_three_layers);
  fmt::format_to(
      std::back_inserter(buffer), "                  four layers: {:.2f} MREps\n\n", ch_eq.avg_rate_MREps_four_layers);

  const auto& prg_pdsch = du_lo.pdsch_metrics.scrambling_metrics;
  const auto& prg_pusch = du_lo.pusch_metrics.scrambling_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- Pseudo-random generator metrics ---\n");
  fmt::format_to(std::back_inserter(buffer), "      Avg. PUSCH init time: {:.2f} us\n", prg_pusch.avg_init_time_us);
  fmt::format_to(std::back_inserter(buffer), "   Avg. PUSCH advance rate: {:.2f} Mbps\n", prg_pusch.advance_rate_Mbps);
  fmt::format_to(std::back_inserter(buffer), "  Avg. PUSCH generate rate: {:.2f} Mbps\n", prg_pusch.generate_rate_Mbps);
  fmt::format_to(std::back_inserter(buffer), "      Avg. PDSCH init time: {:.2f} us\n", prg_pdsch.avg_init_time_us);
  fmt::format_to(std::back_inserter(buffer), "   Avg. PDSCH advance rate: {:.2f} Mbps\n", prg_pdsch.advance_rate_Mbps);
  fmt::format_to(std::back_inserter(buffer), "  Avg. PDSCH generate rate: {:.2f} Mbps\n", prg_pdsch.generate_rate_Mbps);
  fmt::format_to(std::back_inserter(buffer), "\n");

  const auto& xform = du_lo.pusch_metrics.xform_precoder_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- Transform precoding ---\n");
  fmt::format_to(std::back_inserter(buffer), " Avg. Transform rate: {:.2f} MREps\n", xform.avg_rate_MREps);
  fmt::format_to(std::back_inserter(buffer), " Avg. Transform latency: {:.2f} us/symbol\n\n", xform.avg_latency_us);

  const auto& pusch_demod_map = du_lo.pusch_metrics.demod_demapper_metrics;
  const auto& pusch_evm       = du_lo.pusch_metrics.evm_calculator_metrics;
  const auto& pdsch_mod       = du_lo.pdsch_metrics.modulator_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- Modulation, demodulation and EVM calculation ---\n");
  fmt::format_to(std::back_inserter(buffer), "      Modulation rate QPSK: {:.2f} Mbps\n", pdsch_mod.qpsk_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                    16-QAM: {:.2f} Mbps\n", pdsch_mod.qam16_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                    64-QAM: {:.2f} Mbps\n", pdsch_mod.qam64_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                   256-QAM: {:.2f} Mbps\n", pdsch_mod.qam256_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "    Demodulation rate QPSK: {:.2f} Mbps\n", pusch_demod_map.qpsk_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                    16-QAM: {:.2f} Mbps\n", pusch_demod_map.qam16_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                    64-QAM: {:.2f} Mbps\n", pusch_demod_map.qam64_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                   256-QAM: {:.2f} Mbps\n", pusch_demod_map.qam256_avg_rate_Mbps);
  fmt::format_to(std::back_inserter(buffer), "  EVM calculator rate QPSK: {:.2f} Mbps\n", pusch_evm.qpsk_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                    16-QAM: {:.2f} Mbps\n", pusch_evm.qam16_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                    64-QAM: {:.2f} Mbps\n", pusch_evm.qam64_avg_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                   256-QAM: {:.2f} Mbps\n\n", pusch_evm.qam256_avg_rate_Mbps);

  const auto& sch_demux = du_lo.pusch_metrics.ulsch_demux_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- UL-SCH demux ---\n");
  fmt::format_to(std::back_inserter(buffer), "        Avg. init time: {:.2f} us\n", sch_demux.avg_init_time_us);
  fmt::format_to(std::back_inserter(buffer), "      Avg. finish time: {:.2f} us\n", sch_demux.avg_finish_time_us);
  fmt::format_to(std::back_inserter(buffer), "  Avg. processing rate: {:.2f} Mbps\n\n", sch_demux.avg_rate_Mbps);

  const auto& pusch_proc = du_lo.pusch_metrics.pusch_proc_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- PUSCH Processor metrics ---\n");
  fmt::format_to(
      std::back_inserter(buffer), "           Avg. data latency: {:.2f} us\n", pusch_proc.avg_data_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "            Avg. UCI latency: {:.2f} us\n", pusch_proc.avg_uci_latency_us);
  fmt::format_to(
      std::back_inserter(buffer), "  Avg. processing PUSCH rate: {:.2f} Mbps\n", pusch_proc.processing_rate_Mbps);
  fmt::format_to(
      std::back_inserter(buffer), "                Decoder time: {:.2f} % (asynchronous)\n\n", ldpc_decoding_percent);

  fmt::format_to(std::back_inserter(buffer), "--- PUSCH Processor breakdown ---\n");
  fmt::format_to(std::back_inserter(buffer), "   |-> Channel estimator: {:.2f} %\n", channel_estimator_percent);
  fmt::format_to(std::back_inserter(buffer), "     |-> Ch. estimator: {:.2f} %\n", port_channel_estimator_percent);
  fmt::format_to(std::back_inserter(buffer), "       |-> TA estimator: {:.2f} %\n", ta_estimator_percent);
  fmt::format_to(std::back_inserter(buffer), "   |-> Demodulation: {:.2f} %\n", demodulator_percent);
  fmt::format_to(std::back_inserter(buffer), "     |---> Channel equalizer: {:.2f} %\n", channel_equalizer_percent);
  fmt::format_to(std::back_inserter(buffer), "     |-> Demodulation mapper: {:.2f} %\n", demodulation_mapper_percent);
  fmt::format_to(std::back_inserter(buffer), "     |-----> EVM calculation: {:.2f} %\n", evm_calc_percent);
  fmt::format_to(std::back_inserter(buffer), "     |----------> Scrambling: {:.2f} %\n", scrambling_percent);
  fmt::format_to(std::back_inserter(buffer), "     |-> Transform precoding: {:.2f} %\n", transform_precoding_percent);
  fmt::format_to(std::back_inserter(buffer), "   |-> UL-SCH Demux: {:.2f} %\n", ulsch_demux_percent);
  fmt::format_to(
      std::back_inserter(buffer), "   |------> Decoder: {:.2f} % (multi-threaded)\n", pusch_wait_time_percent);
  fmt::format_to(std::back_inserter(buffer), "\n");

  const auto& p = du_lo.pdsch_metrics.precoding_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- PDSCH Layer mapping and precoding metrics ---\n");
  fmt::format_to(
      std::back_inserter(buffer), "  Processing rate - one layer: {:.2f} MREps\n", p.avg_rate_MRes_one_layer);
  fmt::format_to(
      std::back_inserter(buffer), "                   two layers: {:.2f} MREps\n", p.avg_rate_MRes_two_layers);
  fmt::format_to(
      std::back_inserter(buffer), "                 three layers: {:.2f} MREps\n", p.avg_rate_MRes_three_layers);
  fmt::format_to(
      std::back_inserter(buffer), "                  four layers: {:.2f} MREps\n\n", p.avg_rate_MRes_four_layers);

  const auto& pdsch_proc = du_lo.pdsch_metrics.pdsch_proc_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- PDSCH Processor metrics ---\n");
  fmt::format_to(std::back_inserter(buffer), "  Avg. processing latency: {:.2f} us\n", pdsch_proc.avg_latency_us);
  fmt::format_to(std::back_inserter(buffer), "      Avg. return latency: {:.2f} us\n", pdsch_proc.avg_return_time_us);
  fmt::format_to(
      std::back_inserter(buffer), "     Avg. processing rate: {:.2f} Mbps\n\n", pdsch_proc.processing_rate_Mbps);

  const auto& pdsch_dmrs = du_lo.pdsch_metrics.dmrs_metrics;
  fmt::format_to(std::back_inserter(buffer), "--- DM-RS for PDSCH generator metrics ---\n");
  fmt::format_to(std::back_inserter(buffer), "  Avg. generation latency: {:.2f} us\n\n", pdsch_dmrs.avg_latency_us);

  // CPU consumption.
  double ldpc_enc_cpu_usage =
      100.0 * static_cast<double>(ldpc_encoder.cpu_usage_us) / static_cast<double>(du_lo.metrics_period.count());
  double ldpc_dec_cpu_usage =
      100.0 * static_cast<double>(ldpc_decoder.cpu_usage_us) / static_cast<double>(du_lo.metrics_period.count());

  double fec_cpu_usage =
      static_cast<double>(ldpc_encoder.cpu_usage_us + ldpc_decoder.cpu_usage_us + ldpc_rm.cpu_usage_us +
                          ldpc_rdm.cpu_usage_us + crc_pdsch.cpu_usage_us + crc_pusch.cpu_usage_us) /
      static_cast<double>(du_lo.metrics_period.count()) * 100.0;

  fmt::format_to(std::back_inserter(buffer), "--- CPU usage ---\n");
  fmt::format_to(std::back_inserter(buffer), "           FEC: {:.2f} %\n", fec_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "  LDPC encoder: {:.2f} %\n", ldpc_enc_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "  LDPC decoder: {:.2f} %\n", ldpc_dec_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "\n");

  // Flush buffer to the logger.
  logger.info("{}", to_c_str(buffer));
}

flexible_o_du_metrics_consumer_log::flexible_o_du_metrics_consumer_log(srslog::basic_logger& logger_) : logger(logger_)
{
}

void flexible_o_du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  log_handle_du_low_metrics(logger, odu_metrics.du.low);
}
