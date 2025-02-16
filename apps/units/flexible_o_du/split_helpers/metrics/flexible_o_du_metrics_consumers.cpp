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

namespace {

DECLARE_METRIC("average_latency_us", metric_average_latency, double, "us");
DECLARE_METRIC("min_latency_us", metric_min_latency, double, "us");
DECLARE_METRIC("max_latency_us", metric_max_latency, double, "us");
DECLARE_METRIC("average_throughput_Mbps", metric_avg_throughput_mbps, double, "Mbps");
DECLARE_METRIC("average_throughput_MREps", metric_avg_throughput_mreps, double, "MREps");
DECLARE_METRIC("average_cb_size_bits", metric_average_cb_size, double, "bits");
DECLARE_METRIC("qpsk_mod_throughput_Mbps", metric_qpsk_mod_avg_throughput, double, "Mbps");
DECLARE_METRIC("qam16_mod_throughput_Mbps", metric_qam16_mod_avg_throughput, double, "Mbps");
DECLARE_METRIC("qam64_mod_throughput_Mbps", metric_qam64_mod_avg_throughput, double, "Mbps");
DECLARE_METRIC("qam256_mod_throughput_Mbps", metric_qam256_mod_avg_throughput, double, "Mbps");
DECLARE_METRIC("cpu_usage_percent", metric_cpu_usage, double, "");
DECLARE_METRIC("BLER", metric_bler, double, "");
DECLARE_METRIC("EVM", metric_evm, double, "");
DECLARE_METRIC("SINR_dB", metric_sinr, double, "dB");

/// DL processing metrics.
// LDPC encoder metrics.
DECLARE_METRIC_SET("ldpc_encoder",
                   mset_ldpc_encoder,
                   metric_average_cb_size,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);
// LDPC rate matcher metrics.
DECLARE_METRIC_SET("ldpc_rate_matcher",
                   mset_ldpc_rate_matcher,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);
// Scrambling metrics.
DECLARE_METRIC_SET("scrambling", mset_scrambling, metric_cpu_usage);

// Modulation mapper metrics.
DECLARE_METRIC_SET("modulation_mapper",
                   mset_modulation_map,
                   metric_qpsk_mod_avg_throughput,
                   metric_qam16_mod_avg_throughput,
                   metric_qam64_mod_avg_throughput,
                   metric_qam256_mod_avg_throughput,
                   metric_cpu_usage);

// List of precoder throughput per number of layers.
DECLARE_METRIC("throughput_per_nof_layers_MREsps", mlist_precoder_throughputs, std::vector<double>, "");

// Precoder and layer mapping metrics.
DECLARE_METRIC_SET("precoding_layer_mapping",
                   mset_precoding,
                   metric_average_latency,
                   mlist_precoder_throughputs,
                   metric_cpu_usage);
// FEC metrics.
DECLARE_METRIC_SET("FEC", mset_fec_dl, metric_avg_throughput_mbps, metric_cpu_usage);

// L1 Downlink metrics container.
DECLARE_METRIC_SET("DL",
                   mset_dl_l1,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage,
                   mset_ldpc_encoder,
                   mset_ldpc_rate_matcher,
                   mset_scrambling,
                   mset_modulation_map,
                   mset_precoding,
                   mset_fec_dl);

/// UL processing metrics.
// LDPC decoder metrics.
DECLARE_METRIC_SET("ldpc_decoder",
                   mset_ldpc_decoder,
                   metric_average_cb_size,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);
// LDPC rate dematcher metrics.
DECLARE_METRIC_SET("ldpc_rate_dematcher",
                   mset_ldpc_rate_dematcher,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);
// Scrambling metrics.
DECLARE_METRIC_SET("descrambling", mset_descrambling, metric_cpu_usage);

// Modulation mapper metrics.
DECLARE_METRIC_SET("demodulation_mapper",
                   mset_demodulation_map,
                   metric_qpsk_mod_avg_throughput,
                   metric_qam16_mod_avg_throughput,
                   metric_qam64_mod_avg_throughput,
                   metric_qam256_mod_avg_throughput,
                   metric_cpu_usage);

// Channel estimation.
DECLARE_METRIC_SET("channel_estimation",
                   mset_channel_estimation,
                   metric_average_latency,
                   metric_min_latency,
                   metric_max_latency,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage);
// Transform precoder.
DECLARE_METRIC_SET("transform_precoder",
                   mset_transform_precoder,
                   metric_average_latency,
                   metric_avg_throughput_mreps,
                   metric_cpu_usage);
// FEC metrics.
DECLARE_METRIC_SET("FEC", mset_fec_ul, metric_avg_throughput_mbps, metric_cpu_usage);
// Algorithm efficiency
DECLARE_METRIC_SET("Algo_efficiency", mset_algorithms, metric_bler, metric_evm, metric_sinr);
// L1 Uplink metrics container.
DECLARE_METRIC_SET("UL",
                   mset_ul_l1,
                   metric_avg_throughput_mbps,
                   metric_cpu_usage,
                   mset_ldpc_decoder,
                   mset_ldpc_rate_dematcher,
                   mset_descrambling,
                   mset_demodulation_map,
                   mset_channel_estimation,
                   mset_transform_precoder,
                   mset_fec_ul,
                   mset_algorithms);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_SET("L1", mset_l1, mset_dl_l1, mset_ul_l1);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_l1>;

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void flexible_o_du_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  if (odu_metrics.du.low.metrics_period == std::chrono::microseconds()) {
    return;
  }
  double metric_period_us = static_cast<double>(odu_metrics.du.low.metrics_period.count());

  metric_context_t ctx("JSON L1 Metrics");

  /// DL path.
  auto& dl_l1 = ctx.get<mset_l1>().get<mset_dl_l1>();

  // LDPC encoder.
  auto&       ldpc_encoder       = dl_l1.get<mset_ldpc_encoder>();
  const auto& metric_enc         = odu_metrics.du.low.ldpc_metrics.encoder_metrics;
  double      ldpc_enc_cpu_usage = 100.0 * metric_enc.cpu_usage_us / metric_period_us;
  ldpc_encoder.write<metric_average_cb_size>(metric_enc.avg_cb_size);
  ldpc_encoder.write<metric_average_latency>(metric_enc.avg_cb_latency);
  ldpc_encoder.write<metric_min_latency>(metric_enc.min_cb_latency_us);
  ldpc_encoder.write<metric_max_latency>(metric_enc.max_cb_latency_us);
  ldpc_encoder.write<metric_avg_throughput_mbps>(metric_enc.encoding_rate_Mbps);
  ldpc_encoder.write<metric_cpu_usage>(ldpc_enc_cpu_usage);

  // LDPC rate matching.
  auto&       ldpc_rm           = dl_l1.get<mset_ldpc_rate_matcher>();
  const auto& metric_rm         = odu_metrics.du.low.ldpc_metrics.rate_match_metrics;
  double      ldpc_rm_cpu_usage = 100.0 * metric_rm.cpu_usage_us / metric_period_us;
  ldpc_rm.write<metric_average_latency>(metric_rm.avg_cb_latency);
  ldpc_rm.write<metric_min_latency>(metric_rm.min_cb_latency_us);
  ldpc_rm.write<metric_max_latency>(metric_rm.max_cb_latency_us);
  ldpc_rm.write<metric_avg_throughput_mbps>(metric_rm.processing_rate_Mbps);
  ldpc_rm.write<metric_cpu_usage>(ldpc_rm_cpu_usage);

  // Scrambling.
  auto&       scrambling       = dl_l1.get<mset_scrambling>();
  const auto& metric_scrmbl    = odu_metrics.du.low.pdsch_metrics.scrambling_metrics;
  double      scrmbl_cpu_usage = 100.0 * metric_scrmbl.cpu_usage_us / metric_period_us;
  scrambling.write<metric_cpu_usage>(scrmbl_cpu_usage);

  // Modulation mapper.
  auto&       mod_mapper       = dl_l1.get<mset_modulation_map>();
  const auto& metric_modul_map = odu_metrics.du.low.pdsch_metrics.modulator_metrics;
  double      mod_cpu_usage    = 100.0 * metric_modul_map.cpu_usage_us / metric_period_us;
  if (std::isnormal(metric_modul_map.qpsk_avg_rate_Mbps)) {
    mod_mapper.write<metric_qpsk_mod_avg_throughput>(metric_modul_map.qpsk_avg_rate_Mbps);
  }
  if (std::isnormal(metric_modul_map.qam16_avg_rate_Mbps)) {
    mod_mapper.write<metric_qam16_mod_avg_throughput>(metric_modul_map.qam16_avg_rate_Mbps);
  }
  if (std::isnormal(metric_modul_map.qam64_avg_rate_Mbps)) {
    mod_mapper.write<metric_qam64_mod_avg_throughput>(metric_modul_map.qam64_avg_rate_Mbps);
  }
  if (std::isnormal(metric_modul_map.qam256_avg_rate_Mbps)) {
    mod_mapper.write<metric_qam256_mod_avg_throughput>(metric_modul_map.qam256_avg_rate_Mbps);
  }
  mod_mapper.write<metric_cpu_usage>(mod_cpu_usage);

  // Precoding and layer mapping.
  auto&       precoding         = dl_l1.get<mset_precoding>();
  const auto& metric_precode    = odu_metrics.du.low.pdsch_metrics.precoding_metrics;
  const auto& metric_pdsch_proc = odu_metrics.du.low.pdsch_metrics.pdsch_proc_metrics;
  double      precode_cpu_usage = 100.0 * metric_precode.cpu_usage_us / metric_period_us;

  std::vector<double> throughputs             = {metric_precode.avg_rate_MRes_one_layer,
                                                 metric_precode.avg_rate_MRes_two_layers,
                                                 metric_precode.avg_rate_MRes_three_layers,
                                                 metric_precode.avg_rate_MRes_four_layers};
  double              avg_precoder_latency_us = static_cast<double>(metric_precode.total_latency_ns) /
                                   static_cast<double>(metric_pdsch_proc.tbs_processed) / 100.0;
  precoding.write<mlist_precoder_throughputs>(throughputs);
  precoding.write<metric_average_latency>(avg_precoder_latency_us);
  precoding.write<metric_cpu_usage>(precode_cpu_usage);

  // DL FEC.
  const auto& metric_crc         = odu_metrics.du.low.pdsch_metrics.crc_metrics;
  double      total_cpu_usage_us = metric_enc.cpu_usage_us + metric_rm.cpu_usage_us + metric_crc.cpu_usage_us;
  double      dl_fec_cpu_usage   = 100.0 * total_cpu_usage_us / metric_period_us;
  auto&       fec_dl             = dl_l1.get<mset_fec_dl>();
  fec_dl.write<metric_avg_throughput_mbps>(metric_enc.encoding_rate_Mbps);
  fec_dl.write<metric_cpu_usage>(dl_fec_cpu_usage);

  // L1 DL processing.
  double dmrs_cpu_usage = 100.0 * odu_metrics.du.low.pdsch_metrics.dmrs_metrics.cpu_usage_us / metric_period_us;
  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of L1 processing.
  double dl_l1_cpu_usage = 100.0 * metric_pdsch_proc.cpu_usage_us / metric_period_us + dmrs_cpu_usage +
                           dl_fec_cpu_usage + scrmbl_cpu_usage + mod_cpu_usage;
  dl_l1.write<metric_avg_throughput_mbps>(metric_pdsch_proc.processing_rate_Mbps);
  dl_l1.write<metric_cpu_usage>(dl_l1_cpu_usage);

  /// UL path.
  auto& ul_l1 = ctx.get<mset_l1>().get<mset_ul_l1>();

  // LDPC decoder.
  auto&       ldpc_decoder       = ul_l1.get<mset_ldpc_decoder>();
  const auto& metric_dec         = odu_metrics.du.low.ldpc_metrics.decoder_metrics;
  double      ldpc_dec_cpu_usage = 100.0 * metric_dec.cpu_usage_us / metric_period_us;
  ldpc_decoder.write<metric_average_cb_size>(metric_dec.avg_cb_size);
  ldpc_decoder.write<metric_average_latency>(metric_dec.avg_cb_latency_us);
  ldpc_decoder.write<metric_min_latency>(metric_dec.min_cb_latency_us);
  ldpc_decoder.write<metric_max_latency>(metric_dec.max_cb_latency_us);
  ldpc_decoder.write<metric_avg_throughput_mbps>(metric_dec.decoding_rate_Mbps);
  ldpc_decoder.write<metric_cpu_usage>(ldpc_dec_cpu_usage);

  // LDPC rate dematching.
  auto&       ldpc_rdm           = ul_l1.get<mset_ldpc_rate_dematcher>();
  const auto& metric_rdm         = odu_metrics.du.low.ldpc_metrics.rate_dematch_metrics;
  double      ldpc_rdm_cpu_usage = 100.0 * metric_rdm.cpu_usage_us / metric_period_us;
  ldpc_rdm.write<metric_average_latency>(metric_rdm.avg_cb_latency);
  ldpc_rdm.write<metric_min_latency>(metric_rdm.min_cb_latency_us);
  ldpc_rdm.write<metric_max_latency>(metric_rdm.max_cb_latency_us);
  ldpc_rdm.write<metric_avg_throughput_mbps>(metric_rdm.processing_rate_Mbps);
  ldpc_rdm.write<metric_cpu_usage>(ldpc_rdm_cpu_usage);

  // Decrambling.
  auto&       descrambling       = ul_l1.get<mset_descrambling>();
  const auto& metric_descrmbl    = odu_metrics.du.low.pusch_metrics.scrambling_metrics;
  double      descrmbl_cpu_usage = 100.0 * metric_descrmbl.cpu_usage_us / metric_period_us;
  descrambling.write<metric_cpu_usage>(descrmbl_cpu_usage);

  // Demodulation mapper.
  auto&       demod_mapper     = ul_l1.get<mset_demodulation_map>();
  const auto& metric_demod_map = odu_metrics.du.low.pusch_metrics.demod_demapper_metrics;
  double      demod_cpu_usage  = 100.0 * metric_demod_map.cpu_usage_us / metric_period_us;
  if (std::isnormal(metric_demod_map.qpsk_avg_rate_Mbps)) {
    demod_mapper.write<metric_qpsk_mod_avg_throughput>(metric_demod_map.qpsk_avg_rate_Mbps);
  }
  if (std::isnormal(metric_demod_map.qam16_avg_rate_Mbps)) {
    demod_mapper.write<metric_qam16_mod_avg_throughput>(metric_demod_map.qam16_avg_rate_Mbps);
  }
  if (std::isnormal(metric_demod_map.qam64_avg_rate_Mbps)) {
    demod_mapper.write<metric_qam64_mod_avg_throughput>(metric_demod_map.qam64_avg_rate_Mbps);
  }
  if (std::isnormal(metric_demod_map.qam256_avg_rate_Mbps)) {
    demod_mapper.write<metric_qam256_mod_avg_throughput>(metric_demod_map.qam256_avg_rate_Mbps);
  }
  demod_mapper.write<metric_cpu_usage>(demod_cpu_usage);

  // Channel estimator.
  auto&       ch_estimator    = ul_l1.get<mset_channel_estimation>();
  const auto& metric_chest    = odu_metrics.du.low.pusch_metrics.ch_estimator_metrics;
  double      chest_cpu_usage = 100.0 * metric_chest.cpu_usage_us / metric_period_us;
  ch_estimator.write<metric_average_latency>(metric_chest.avg_processing_latency);
  ch_estimator.write<metric_min_latency>(metric_chest.min_proc_latency_us);
  ch_estimator.write<metric_max_latency>(metric_chest.max_proc_latency_us);
  ch_estimator.write<metric_avg_throughput_mbps>(metric_chest.processing_rate_Mbps);
  ch_estimator.write<metric_cpu_usage>(chest_cpu_usage);

  // Transform precoder.
  auto&       xform_precoder       = ul_l1.get<mset_transform_precoder>();
  const auto& metric_xform_precode = odu_metrics.du.low.pusch_metrics.xform_precoder_metrics;
  double      xform_precode_cpu    = 100.0 * metric_xform_precode.cpu_usage_us / metric_period_us;
  xform_precoder.write<metric_average_latency>(metric_xform_precode.avg_latency_us);
  xform_precoder.write<metric_cpu_usage>(xform_precode_cpu);

  // UL FEC.
  const auto& metric_crc_ul       = odu_metrics.du.low.pusch_metrics.crc_metrics;
  double      ul_fec_cpu_usage_us = metric_dec.cpu_usage_us + metric_rdm.cpu_usage_us + metric_crc_ul.cpu_usage_us;
  double      ul_fec_cpu_usage    = 100.0 * ul_fec_cpu_usage_us / metric_period_us;
  auto&       fec_ul              = ul_l1.get<mset_fec_ul>();
  fec_ul.write<metric_avg_throughput_mbps>(metric_dec.decoding_rate_Mbps);
  fec_ul.write<metric_cpu_usage>(ul_fec_cpu_usage);

  // L1 UL processing.
  const auto& metric_pusch_proc = odu_metrics.du.low.pusch_metrics.pusch_proc_metrics;
  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of L1 processing.
  double ul_l1_cpu_usage = 100.0 * metric_pusch_proc.cpu_usage_us / metric_period_us + ul_fec_cpu_usage;

  ul_l1.write<metric_avg_throughput_mbps>(metric_pusch_proc.processing_rate_Mbps);
  ul_l1.write<metric_cpu_usage>(ul_l1_cpu_usage);

  auto& algorithms = ul_l1.get<mset_algorithms>();
  algorithms.write<metric_bler>(metric_pusch_proc.decoding_bler);
  algorithms.write<metric_evm>(metric_pusch_proc.evm);
  algorithms.write<metric_sinr>(metric_pusch_proc.sinr_dB);

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
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
  double metric_period_us   = static_cast<double>(du_lo.metrics_period.count());
  double ldpc_enc_cpu_usage = 100.0 * ldpc_encoder.cpu_usage_us / metric_period_us;
  double ldpc_dec_cpu_usage = 100.0 * ldpc_decoder.cpu_usage_us / metric_period_us;
  double dl_fec_cpu_usage =
      ldpc_encoder.cpu_usage_us + ldpc_rm.cpu_usage_us + crc_pdsch.cpu_usage_us / metric_period_us;
  double ul_fec_cpu_usage =
      ldpc_decoder.cpu_usage_us + ldpc_rdm.cpu_usage_us + crc_pusch.cpu_usage_us / metric_period_us;
  double ldpc_rm_cpu_usage       = ldpc_rm.cpu_usage_us / metric_period_us;
  double ldpc_rdm_cpu_usage      = ldpc_rdm.cpu_usage_us / metric_period_us;
  double scramble_cpu_usage      = prg_pdsch.cpu_usage_us / metric_period_us;
  double descramble_cpu_usage    = prg_pusch.cpu_usage_us / metric_period_us;
  double pdsch_mod_map_cpu       = pdsch_mod.cpu_usage_us / metric_period_us;
  double pusch_demod_map_cpu     = pusch_demod_map.cpu_usage_us / metric_period_us;
  double pdsch_precode_cpu_usage = p.cpu_usage_us / metric_period_us;
  double pusch_precode_cpu_usage = xform.cpu_usage_us / metric_period_us;
  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of L1 processing.
  double dl_l1_cpu_usage =
      100.0 * pdsch_proc.cpu_usage_us / metric_period_us + dl_fec_cpu_usage + pdsch_mod_map_cpu + scramble_cpu_usage;
  double ul_l1_cpu_usage = 100.0 * pusch_proc.cpu_usage_us / metric_period_us + ul_fec_cpu_usage;

  fmt::format_to(std::back_inserter(buffer), "-------- CPU usage -------\n");
  fmt::format_to(std::back_inserter(buffer), "           DL FEC: {:.2f} %\n", dl_fec_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "     LDPC encoder: {:.2f} %\n", ldpc_enc_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "          LDPC RM: {:.2f} %\n", ldpc_rm_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "           UL FEC: {:.2f} %\n", ul_fec_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "     LDPC decoder: {:.2f} %\n", ldpc_dec_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "         LDPC RDM: {:.2f} %\n", ldpc_rdm_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), " PDSCH scrambling: {:.2f} %\n", scramble_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), " PUSCH scrambling: {:.2f} %\n", descramble_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "    Modul. mapper: {:.2f} %\n", pdsch_mod_map_cpu);
  fmt::format_to(std::back_inserter(buffer), "  Demodul. mapper: {:.2f} %\n", pusch_demod_map_cpu);
  fmt::format_to(std::back_inserter(buffer), "     DL layer map: {:.2f} %\n", pdsch_precode_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "     UL precoding: {:.2f} %\n", pusch_precode_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "            L1 DL: {:.2f} %\n", dl_l1_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "            L1 UL: {:.2f} %\n", ul_l1_cpu_usage);
  fmt::format_to(std::back_inserter(buffer), "\n");

  // Flush buffer to the logger.
  logger.info("{}", to_c_str(buffer));
}

void flexible_o_du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  log_handle_du_low_metrics(logger, odu_metrics.du.low);
}
