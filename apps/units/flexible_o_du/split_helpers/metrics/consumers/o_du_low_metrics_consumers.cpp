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

#include "o_du_low_metrics_consumers.h"
#include "apps/helpers/metrics/helpers.h"
#include "apps/helpers/metrics/json_generators/o_du_low.h"
#include "srsran/du/du_low/o_du_low_metrics.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;
using namespace app_helpers;

/// Size in bytes of the buffer used to prepare the metrics before logging.
static constexpr unsigned str_buffer_size = 4096;

void o_du_low_metrics_consumer_json::handle_metric(const srs_du::o_du_low_metrics& metric)
{
  // Nothing to print.
  if (metric.du_lo_metrics.sector_metrics.empty()) {
    return;
  }

  log_chan("{}", app_helpers::json_generators::generate_string(metric, pci_sector_map, 2));
}

static void log_upper_phy_metrics_verbose(fmt::basic_memory_buffer<char, str_buffer_size>& buffer,
                                          const upper_phy_metrics&                         upper_metrics)
{
  // The following variables are used to break-down PUSCH processing into smaller blocks.

  // Percent of total PUSCH processing time used by the channel estimation.
  double channel_estimator_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.ch_estimator_metrics.total_processing_latency.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by port channel estimation.
  double port_channel_estimator_percent = validate_fp_value(
      100.0 *
      static_cast<double>(upper_metrics.pusch_metrics.port_ch_estimator_metrics.total_processing_latency.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by time alignment estimator.
  double ta_estimator_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.pusch_ta_est_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by demodulator.
  double demodulator_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.pusch_demod_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the channel equalizer.
  double channel_equalizer_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.ch_equalizer_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by demodulation mapper.
  double demodulation_mapper_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.demod_demapper_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by EVM calculation.
  double evm_calc_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.evm_calculator_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the scrambling.
  double scrambling_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.scrambling_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the transform precoding.
  double transform_precoding_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.xform_precoder_metrics.total_proc_latency.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by ULSCH demultiplexing.
  double ulsch_demux_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.ulsch_demux_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the LDPC decoder (asynchronous).
  double ldpc_decoding_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.ldpc_metrics.decoder_metrics.total_proc_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  double pusch_wait_time_percent = validate_fp_value(
      100.0 * static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_wait_time.count()) /
      static_cast<double>(upper_metrics.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));
  fmt::format_to(std::back_inserter(buffer), "\n");

  const auto& dl_processor = upper_metrics.dl_processor_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} max_latency={:.2f} us in slot={}\n",
                 "  DL processing:",
                 validate_fp_value(dl_processor.max_latency_us.first),
                 dl_processor.max_latency_us.second);

  const auto& ldpc_encoder = upper_metrics.ldpc_metrics.encoder_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_cb_size={:.2f} bits, avg_latency={:.2f} us, max_latency={:.2f} us, "
                 "encode_rate={:.2f} Mbps\n",
                 "  LDPC Encoder:",
                 validate_fp_value(ldpc_encoder.avg_cb_size),
                 validate_fp_value(ldpc_encoder.avg_cb_latency_us),
                 validate_fp_value(ldpc_encoder.max_cb_latency_us),
                 validate_fp_value(ldpc_encoder.encoding_rate_Mbps));

  const auto& ldpc_rm = upper_metrics.ldpc_metrics.rate_match_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  LDPC Rate matcher:",
                 validate_fp_value(ldpc_rm.avg_cb_latency),
                 validate_fp_value(ldpc_rm.processing_rate_Mbps));

  const auto& ldpc_decoder = upper_metrics.ldpc_metrics.decoder_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_cb_size={:.2f} bits, avg_latency={:.2f} us, max_latency={:.2f} us, "
                 "avg_nof_iter={:.2f}, decode_rate={:.2f} Mbps\n",
                 "  LDPC Decoder:",
                 validate_fp_value(ldpc_decoder.avg_cb_size),
                 validate_fp_value(ldpc_decoder.avg_cb_latency_us),
                 validate_fp_value(ldpc_decoder.max_cb_latency_us),
                 validate_fp_value(ldpc_decoder.avg_nof_iterations),
                 validate_fp_value(ldpc_decoder.decoding_rate_Mbps));

  const auto& ldpc_rdm = upper_metrics.ldpc_metrics.rate_dematch_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, max_latency={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  LDPC Rate dematcher:",
                 validate_fp_value(ldpc_rdm.avg_cb_latency),
                 validate_fp_value(ldpc_rdm.max_cb_latency_us),
                 validate_fp_value(ldpc_rdm.processing_rate_Mbps));

  const auto& ch_est = upper_metrics.pusch_metrics.ch_estimator_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, proc_rate={:.2f} MPRBps\n",
                 "  PUSCH chan. estimator:",
                 validate_fp_value(ch_est.avg_processing_latency),
                 validate_fp_value(ch_est.processing_rate_Mbps));

  const auto& ch_eq = upper_metrics.pusch_metrics.ch_equalizer_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} proc_rate_per_nof_layers=[{:.2f}, {:.2f}, {:.2f}, {:.2f}] MREps\n",
                 "  PUSCH chan. equalizer:",
                 validate_fp_value(ch_eq.avg_rate_MREps_one_layer),
                 validate_fp_value(ch_eq.avg_rate_MREps_two_layers),
                 validate_fp_value(ch_eq.avg_rate_MREps_three_layers),
                 validate_fp_value(ch_eq.avg_rate_MREps_four_layers));

  const auto& prg_pdsch = upper_metrics.pdsch_metrics.scrambling_metrics;
  const auto& prg_pusch = upper_metrics.pusch_metrics.scrambling_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_init_time={:.2f} us, avg_advance_rate={:.2f} Mbps, avg_gen_rate={:.2f} Mbps\n",
                 "  PUSCH scrambling:",
                 validate_fp_value(prg_pusch.avg_init_time_us),
                 validate_fp_value(prg_pusch.advance_rate_Mbps),
                 validate_fp_value(prg_pusch.generate_rate_Mbps));
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_init_time={:.2f} us, avg_advance_rate={:.2f} Mbps, avg_gen_rate={:.2f} Mbps\n",
                 "  PDSCH scrambling:",
                 validate_fp_value(prg_pdsch.avg_init_time_us),
                 validate_fp_value(prg_pdsch.advance_rate_Mbps),
                 validate_fp_value(prg_pdsch.generate_rate_Mbps));

  const auto& xform = upper_metrics.pusch_metrics.xform_precoder_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} proc_rate={:.2f} MREps, avg_latency={:.2f} us/symbol\n",
                 "  Transform precoding:",
                 validate_fp_value(xform.avg_rate_MREps),
                 validate_fp_value(xform.avg_latency_us));

  const auto& pusch_demod_map = upper_metrics.pusch_metrics.demod_demapper_metrics;
  const auto& pusch_evm       = upper_metrics.pusch_metrics.evm_calculator_metrics;
  const auto& pdsch_mod       = upper_metrics.pdsch_metrics.modulator_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} QPSK={:.2f} Mbps, 16QAM={:.2f} Mbps, 64QAM={:.2f} Mbps, 256QAM={:.2f} Mbps\n",
                 "  Modulation rates:",
                 validate_fp_value(pdsch_mod.qpsk_avg_rate_Mbps),
                 validate_fp_value(pdsch_mod.qam16_avg_rate_Mbps),
                 validate_fp_value(pdsch_mod.qam64_avg_rate_Mbps),
                 validate_fp_value(pdsch_mod.qam256_avg_rate_Mbps));
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} QPSK={:.2f} Mbps, 16QAM={:.2f} Mbps, 64QAM={:.2f} Mbps, 256QAM={:.2f} Mbps\n",
                 "  Demodulation rates:",
                 validate_fp_value(pusch_demod_map.qpsk_avg_rate_Mbps),
                 validate_fp_value(pusch_demod_map.qam16_avg_rate_Mbps),
                 validate_fp_value(pusch_demod_map.qam64_avg_rate_Mbps),
                 validate_fp_value(pusch_demod_map.qam256_avg_rate_Mbps));
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} QPSK={:.2f} Mbps, 16QAM={:.2f} Mbps, 64QAM={:.2f} Mbps, 256QAM={:.2f} Mbps\n",
                 "  EVM calc. rates:",
                 validate_fp_value(pusch_evm.qpsk_avg_rate_Mbps),
                 validate_fp_value(pusch_evm.qam16_avg_rate_Mbps),
                 validate_fp_value(pusch_evm.qam64_avg_rate_Mbps),
                 validate_fp_value(pusch_evm.qam256_avg_rate_Mbps));

  const auto& sch_demux = upper_metrics.pusch_metrics.ulsch_demux_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_init_time={:.2f} us, avg_finish_time={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  UL-SCH demux:",
                 validate_fp_value(sch_demux.avg_init_time_us),
                 validate_fp_value(sch_demux.avg_finish_time_us),
                 validate_fp_value(sch_demux.avg_rate_Mbps));

  const auto& pusch_proc = upper_metrics.pusch_metrics.pusch_proc_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_data_latency={:.2f} us, avg_uci_latency={:.2f} us, proc_rate={:.2f} Mbps, "
                 "decoder_time={:.2f}% (asynchronous)\n",
                 "  PUSCH Processor:",
                 validate_fp_value(pusch_proc.avg_data_latency_us),
                 validate_fp_value(pusch_proc.avg_uci_latency_us),
                 validate_fp_value(pusch_proc.processing_rate_Mbps),
                 validate_fp_value(ldpc_decoding_percent));

  const auto& p = upper_metrics.pdsch_metrics.precoding_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} proc_rate_per_nof_layers=[{:.2f}, {:.2f}, {:.2f}, {:.2f}] MREps\n",
                 "  PDSCH precoding:",
                 validate_fp_value(p.avg_rate_MRes_one_layer),
                 validate_fp_value(p.avg_rate_MRes_two_layers),
                 validate_fp_value(p.avg_rate_MRes_three_layers),
                 validate_fp_value(p.avg_rate_MRes_four_layers));

  const auto& pdsch_proc = upper_metrics.pdsch_metrics.pdsch_proc_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, max_latency={:.2f} us at slot={}, proc_rate={:.2f} Mbps\n",
                 "  PDSCH Processor:",
                 validate_fp_value(pdsch_proc.avg_latency_us),
                 validate_fp_value(pdsch_proc.max_latency_us.first),
                 pdsch_proc.max_latency_us.second,
                 validate_fp_value(pdsch_proc.processing_rate_Mbps));

  const auto& pdsch_dmrs = upper_metrics.pdsch_metrics.dmrs_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_generation_latency={:.2f} us\n",
                 "  PDSCH DM-RS:",
                 validate_fp_value(pdsch_dmrs.avg_latency_us));

  const auto& crc_pdsch = upper_metrics.pdsch_metrics.crc_metrics;
  const auto& crc_pusch = upper_metrics.pusch_metrics.crc_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "  PDSCH CRC calculator {{avg_latency, avg_rate}}: crc16={{{:.2f} us, {:.2f} Mbps}}, "
                 "crc24a={{{:.2f} us, {:.2f} Mbps}}, "
                 "crc24b={{{:.2f} us, {:.2f} Mbps}}\n",
                 validate_fp_value(crc_pdsch.crc16_avg_latency_us),
                 validate_fp_value(crc_pdsch.crc16_avg_rate_Mbps),
                 validate_fp_value(crc_pdsch.crc24a_avg_latency_us),
                 validate_fp_value(crc_pdsch.crc24a_avg_rate_Mbps),
                 validate_fp_value(crc_pdsch.crc24b_avg_latency_us),
                 validate_fp_value(crc_pdsch.crc24b_avg_rate_Mbps));
  fmt::format_to(std::back_inserter(buffer),
                 "  PUSCH CRC calculator {{avg_latency, avg_rate}}: crc16={{{:.2f} us, {:.2f} Mbps}}, "
                 "crc24a={{{:.2f} us, {:.2f} Mbps}}, "
                 "crc24b={{{:.2f} us, {:.2f} Mbps}}\n",
                 validate_fp_value(crc_pusch.crc16_avg_latency_us),
                 validate_fp_value(crc_pusch.crc16_avg_rate_Mbps),
                 validate_fp_value(crc_pusch.crc24a_avg_latency_us),
                 validate_fp_value(crc_pusch.crc24a_avg_rate_Mbps),
                 validate_fp_value(crc_pusch.crc24b_avg_latency_us),
                 validate_fp_value(crc_pusch.crc24b_avg_rate_Mbps));

  fmt::format_to(std::back_inserter(buffer), "  PUSCH Processor breakdown:\n");
  fmt::format_to(std::back_inserter(buffer), "     |-> Channel estimator: {:.2f} %\n", channel_estimator_percent);
  fmt::format_to(std::back_inserter(buffer), "       |-> Ch. estimator: {:.2f} %\n", port_channel_estimator_percent);
  fmt::format_to(std::back_inserter(buffer), "         |-> TA estimator: {:.2f} %\n", ta_estimator_percent);
  fmt::format_to(std::back_inserter(buffer), "     |-> Demodulation: {:.2f} %\n", demodulator_percent);
  fmt::format_to(std::back_inserter(buffer), "       |---> Channel equalizer: {:.2f} %\n", channel_equalizer_percent);
  fmt::format_to(std::back_inserter(buffer), "       |-> Demodulation mapper: {:.2f} %\n", demodulation_mapper_percent);
  fmt::format_to(std::back_inserter(buffer), "       |-----> EVM calculation: {:.2f} %\n", evm_calc_percent);
  fmt::format_to(std::back_inserter(buffer), "       |----------> Scrambling: {:.2f} %\n", scrambling_percent);
  fmt::format_to(std::back_inserter(buffer), "       |-> Transform precoding: {:.2f} %\n", transform_precoding_percent);
  fmt::format_to(std::back_inserter(buffer), "     |-> UL-SCH Demux: {:.2f} %\n", ulsch_demux_percent);
  fmt::format_to(
      std::back_inserter(buffer), "     |------> Decoder: {:.2f} % (multi-threaded)\n", pusch_wait_time_percent);

  // CPU consumption.
  double metric_period_us   = static_cast<double>(upper_metrics.metrics_period.count());
  double ldpc_enc_cpu_usage = 100.0 * ldpc_encoder.cpu_usage_us / metric_period_us;
  double ldpc_dec_cpu_usage = 100.0 * ldpc_decoder.cpu_usage_us / metric_period_us;
  double dl_fec_cpu_usage =
      100.0 * (ldpc_encoder.cpu_usage_us + ldpc_rm.cpu_usage_us + crc_pdsch.cpu_usage_us) / metric_period_us;
  double ul_fec_cpu_usage =
      100.0 * (ldpc_decoder.cpu_usage_us + ldpc_rdm.cpu_usage_us + crc_pusch.cpu_usage_us) / metric_period_us;
  double ldpc_rm_cpu_usage       = 100.0 * ldpc_rm.cpu_usage_us / metric_period_us;
  double ldpc_rdm_cpu_usage      = 100.0 * ldpc_rdm.cpu_usage_us / metric_period_us;
  double scramble_cpu_usage      = 100.0 * prg_pdsch.cpu_usage_us / metric_period_us;
  double descramble_cpu_usage    = 100.0 * prg_pusch.cpu_usage_us / metric_period_us;
  double pdsch_mod_map_cpu       = 100.0 * pdsch_mod.cpu_usage_us / metric_period_us;
  double pusch_demod_map_cpu     = 100.0 * pusch_demod_map.cpu_usage_us / metric_period_us;
  double pdsch_dmrs_cpu_usage    = 100.0 * pdsch_dmrs.cpu_usage_us / metric_period_us;
  double pdsch_precode_cpu_usage = 100.0 * p.cpu_usage_us / metric_period_us;
  double pusch_precode_cpu_usage = 100.0 * xform.cpu_usage_us / metric_period_us;
  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of upper phy
  // processing.
  double upper_phy_dl_cpu_usage = 100.0 * pdsch_proc.cpu_usage_us / metric_period_us + dl_fec_cpu_usage +
                                  pdsch_mod_map_cpu + scramble_cpu_usage + pdsch_dmrs_cpu_usage;
  double upper_phy_ul_cpu_usage = 100.0 * pusch_proc.cpu_usage_us / metric_period_us + ul_fec_cpu_usage;

  fmt::format_to(std::back_inserter(buffer),
                 "  CPU usage over {:.3f} ms:\n"
                 "    DL: upper_phy_dl={:.2f}%, fec={:.2f}%, ldpc_enc={:.2f}%, ldpc_rm={:.2f}%, "
                 "pdsch_scrambling={:.2f}%, mod_mapper={:.2f}%, dl_layer_map={:.2f}%\n"
                 "    UL: upper_phy_ul={:.2f}%, ul_fec={:.2f}%, ldpc_dec={:.2f}%, ldpc_rdm={:.2f}%, "
                 "pusch_scrambling={:.2f}%, demod_mapper={:.2f}%, ul_precoding={:.2f}%",
                 metric_period_us / 1000.0,
                 validate_fp_value(upper_phy_dl_cpu_usage),
                 validate_fp_value(dl_fec_cpu_usage),
                 validate_fp_value(ldpc_enc_cpu_usage),
                 validate_fp_value(ldpc_rm_cpu_usage),
                 validate_fp_value(scramble_cpu_usage),
                 validate_fp_value(pdsch_mod_map_cpu),
                 validate_fp_value(pdsch_precode_cpu_usage),
                 validate_fp_value(upper_phy_ul_cpu_usage),
                 validate_fp_value(ul_fec_cpu_usage),
                 validate_fp_value(ldpc_dec_cpu_usage),
                 validate_fp_value(ldpc_rdm_cpu_usage),
                 validate_fp_value(descramble_cpu_usage),
                 validate_fp_value(pusch_demod_map_cpu),
                 validate_fp_value(pusch_precode_cpu_usage));
}

void o_du_low_metrics_consumer_log::handle_metric(const srs_du::o_du_low_metrics& metric)
{
  // No metrics to print.
  if (metric.du_lo_metrics.sector_metrics.empty()) {
    return;
  }

  for (const auto& upper_metrics : metric.du_lo_metrics.sector_metrics) {
    fmt::basic_memory_buffer<char, str_buffer_size> buffer;
    fmt::format_to(std::back_inserter(buffer), "Upper PHY sector#{} metrics: ", upper_metrics.sector_id);

    fmt::format_to(std::back_inserter(buffer),
                   "PHY metrics: "
                   "dl_processing_max_latency={:.1f}us "
                   "dl_processing_max_slot={} "
                   "ul_processing_max_latency={:.1f}us "
                   "ul_processing_max_slot={} "
                   "ldpc_encoder_avg_latency={:.1f}us "
                   "ldpc_encoder_max_latency={:.1f}us "
                   "ldpc_decoder_avg_latency={:.1f}us "
                   "ldpc_decoder_max_latency={:.1f}us "
                   "ldpc_decoder_avg_nof_iter={:.1f}",
                   validate_fp_value(upper_metrics.dl_processor_metrics.max_latency_us.first),
                   upper_metrics.dl_processor_metrics.max_latency_us.second,
                   validate_fp_value(upper_metrics.pusch_metrics.pusch_proc_metrics.max_data_latency_us.first),
                   upper_metrics.pusch_metrics.pusch_proc_metrics.max_data_latency_us.second,
                   validate_fp_value(upper_metrics.ldpc_metrics.encoder_metrics.avg_cb_latency_us),
                   validate_fp_value(upper_metrics.ldpc_metrics.encoder_metrics.max_cb_latency_us),
                   validate_fp_value(upper_metrics.ldpc_metrics.decoder_metrics.avg_cb_latency_us),
                   validate_fp_value(upper_metrics.ldpc_metrics.decoder_metrics.max_cb_latency_us),
                   validate_fp_value(upper_metrics.ldpc_metrics.decoder_metrics.avg_nof_iterations));

    // Verbose logging.
    if (verbose) {
      log_upper_phy_metrics_verbose(buffer, upper_metrics);
    }

    // Flush buffer to the logger.
    log_chan("{}", to_c_str(buffer));
  }
}
