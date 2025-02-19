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
DECLARE_METRIC("bler", metric_bler, double, "");
DECLARE_METRIC("evm", metric_evm, double, "");
DECLARE_METRIC("sinr_db", metric_sinr, double, "dB");

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
DECLARE_METRIC_SET("fec", mset_fec_dl, metric_avg_throughput_mbps, metric_cpu_usage);

// Upper PHY Downlink metrics container.
DECLARE_METRIC_SET("dl",
                   mset_upper_phy_dl,
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
DECLARE_METRIC_SET("fec", mset_fec_ul, metric_avg_throughput_mbps, metric_cpu_usage);
// Algorithm efficiency
DECLARE_METRIC_SET("algo_efficiency", mset_algorithms, metric_bler, metric_evm, metric_sinr);
// Upper PHY Uplink metrics container.
DECLARE_METRIC_SET("ul",
                   mset_upper_phy_ul,
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

DECLARE_METRIC_SET("upper_phy", mset_upper_phy, mset_upper_phy_dl, mset_upper_phy_ul);
DECLARE_METRIC_SET("du_low", mset_du_low, mset_upper_phy);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_SET("du", mset_du, mset_du_low);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_du>;

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

  metric_context_t ctx("JSON Upper PHY Metrics");

  /// DL path.
  auto& upper_phy_dl = ctx.get<mset_du>().get<mset_du_low>().get<mset_upper_phy>().get<mset_upper_phy_dl>();

  // LDPC encoder.
  auto&       ldpc_encoder       = upper_phy_dl.get<mset_ldpc_encoder>();
  const auto& metric_enc         = odu_metrics.du.low.ldpc_metrics.encoder_metrics;
  double      ldpc_enc_cpu_usage = 100.0 * metric_enc.cpu_usage_us / metric_period_us;
  ldpc_encoder.write<metric_average_cb_size>(metric_enc.avg_cb_size);
  ldpc_encoder.write<metric_average_latency>(metric_enc.avg_cb_latency);
  ldpc_encoder.write<metric_min_latency>(metric_enc.min_cb_latency_us);
  ldpc_encoder.write<metric_max_latency>(metric_enc.max_cb_latency_us);
  ldpc_encoder.write<metric_avg_throughput_mbps>(metric_enc.encoding_rate_Mbps);
  ldpc_encoder.write<metric_cpu_usage>(ldpc_enc_cpu_usage);

  // LDPC rate matching.
  auto&       ldpc_rm           = upper_phy_dl.get<mset_ldpc_rate_matcher>();
  const auto& metric_rm         = odu_metrics.du.low.ldpc_metrics.rate_match_metrics;
  double      ldpc_rm_cpu_usage = 100.0 * metric_rm.cpu_usage_us / metric_period_us;
  ldpc_rm.write<metric_average_latency>(metric_rm.avg_cb_latency);
  ldpc_rm.write<metric_min_latency>(metric_rm.min_cb_latency_us);
  ldpc_rm.write<metric_max_latency>(metric_rm.max_cb_latency_us);
  ldpc_rm.write<metric_avg_throughput_mbps>(metric_rm.processing_rate_Mbps);
  ldpc_rm.write<metric_cpu_usage>(ldpc_rm_cpu_usage);

  // Scrambling.
  auto&       scrambling       = upper_phy_dl.get<mset_scrambling>();
  const auto& metric_scrmbl    = odu_metrics.du.low.pdsch_metrics.scrambling_metrics;
  double      scrmbl_cpu_usage = 100.0 * metric_scrmbl.cpu_usage_us / metric_period_us;
  scrambling.write<metric_cpu_usage>(scrmbl_cpu_usage);

  // Modulation mapper.
  auto&       mod_mapper       = upper_phy_dl.get<mset_modulation_map>();
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
  auto&       precoding         = upper_phy_dl.get<mset_precoding>();
  const auto& metric_precode    = odu_metrics.du.low.pdsch_metrics.precoding_metrics;
  const auto& metric_pdsch_proc = odu_metrics.du.low.pdsch_metrics.pdsch_proc_metrics;
  double      precode_cpu_usage = 100.0 * metric_precode.cpu_usage_us / metric_period_us;

  std::vector<double> throughputs             = {metric_precode.avg_rate_MRes_one_layer,
                                                 metric_precode.avg_rate_MRes_two_layers,
                                                 metric_precode.avg_rate_MRes_three_layers,
                                                 metric_precode.avg_rate_MRes_four_layers};
  double              avg_precoder_latency_us = static_cast<double>(metric_precode.total_latency_ns) /
                                   static_cast<double>(metric_pdsch_proc.tbs_processed) / 100.0;
  if (std::isnan(avg_precoder_latency_us) || std::isinf(avg_precoder_latency_us)) {
    avg_precoder_latency_us = 0.0;
  }
  precoding.write<mlist_precoder_throughputs>(throughputs);
  precoding.write<metric_average_latency>(avg_precoder_latency_us);
  precoding.write<metric_cpu_usage>(precode_cpu_usage);

  // DL FEC.
  const auto& metric_crc         = odu_metrics.du.low.pdsch_metrics.crc_metrics;
  double      total_cpu_usage_us = metric_enc.cpu_usage_us + metric_rm.cpu_usage_us + metric_crc.cpu_usage_us;
  double      dl_fec_cpu_usage   = 100.0 * total_cpu_usage_us / metric_period_us;
  auto&       fec_dl             = upper_phy_dl.get<mset_fec_dl>();
  fec_dl.write<metric_avg_throughput_mbps>(metric_enc.encoding_rate_Mbps);
  fec_dl.write<metric_cpu_usage>(dl_fec_cpu_usage);

  // L1 DL processing.
  double dmrs_cpu_usage = 100.0 * odu_metrics.du.low.pdsch_metrics.dmrs_metrics.cpu_usage_us / metric_period_us;
  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of L1 processing.
  double upper_phy_dl_cpu_usage = 100.0 * metric_pdsch_proc.cpu_usage_us / metric_period_us + dmrs_cpu_usage +
                                  dl_fec_cpu_usage + scrmbl_cpu_usage + mod_cpu_usage;
  upper_phy_dl.write<metric_avg_throughput_mbps>(metric_pdsch_proc.processing_rate_Mbps);
  upper_phy_dl.write<metric_cpu_usage>(upper_phy_dl_cpu_usage);

  /// UL path.
  auto& upper_phy_ul = ctx.get<mset_du>().get<mset_du_low>().get<mset_upper_phy>().get<mset_upper_phy_ul>();

  // LDPC decoder.
  auto&       ldpc_decoder       = upper_phy_ul.get<mset_ldpc_decoder>();
  const auto& metric_dec         = odu_metrics.du.low.ldpc_metrics.decoder_metrics;
  double      ldpc_dec_cpu_usage = 100.0 * metric_dec.cpu_usage_us / metric_period_us;
  ldpc_decoder.write<metric_average_cb_size>(metric_dec.avg_cb_size);
  ldpc_decoder.write<metric_average_latency>(metric_dec.avg_cb_latency_us);
  ldpc_decoder.write<metric_min_latency>(metric_dec.min_cb_latency_us);
  ldpc_decoder.write<metric_max_latency>(metric_dec.max_cb_latency_us);
  ldpc_decoder.write<metric_avg_throughput_mbps>(metric_dec.decoding_rate_Mbps);
  ldpc_decoder.write<metric_cpu_usage>(ldpc_dec_cpu_usage);

  // LDPC rate dematching.
  auto&       ldpc_rdm           = upper_phy_ul.get<mset_ldpc_rate_dematcher>();
  const auto& metric_rdm         = odu_metrics.du.low.ldpc_metrics.rate_dematch_metrics;
  double      ldpc_rdm_cpu_usage = 100.0 * metric_rdm.cpu_usage_us / metric_period_us;
  ldpc_rdm.write<metric_average_latency>(metric_rdm.avg_cb_latency);
  ldpc_rdm.write<metric_min_latency>(metric_rdm.min_cb_latency_us);
  ldpc_rdm.write<metric_max_latency>(metric_rdm.max_cb_latency_us);
  ldpc_rdm.write<metric_avg_throughput_mbps>(metric_rdm.processing_rate_Mbps);
  ldpc_rdm.write<metric_cpu_usage>(ldpc_rdm_cpu_usage);

  // Decrambling.
  auto&       descrambling       = upper_phy_ul.get<mset_descrambling>();
  const auto& metric_descrmbl    = odu_metrics.du.low.pusch_metrics.scrambling_metrics;
  double      descrmbl_cpu_usage = 100.0 * metric_descrmbl.cpu_usage_us / metric_period_us;
  descrambling.write<metric_cpu_usage>(descrmbl_cpu_usage);

  // Demodulation mapper.
  auto&       demod_mapper     = upper_phy_ul.get<mset_demodulation_map>();
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
  auto&       ch_estimator    = upper_phy_ul.get<mset_channel_estimation>();
  const auto& metric_chest    = odu_metrics.du.low.pusch_metrics.ch_estimator_metrics;
  double      chest_cpu_usage = 100.0 * metric_chest.cpu_usage_us / metric_period_us;
  ch_estimator.write<metric_average_latency>(metric_chest.avg_processing_latency);
  ch_estimator.write<metric_min_latency>(metric_chest.min_proc_latency_us);
  ch_estimator.write<metric_max_latency>(metric_chest.max_proc_latency_us);
  ch_estimator.write<metric_avg_throughput_mbps>(metric_chest.processing_rate_Mbps);
  ch_estimator.write<metric_cpu_usage>(chest_cpu_usage);

  // Transform precoder.
  auto&       xform_precoder       = upper_phy_ul.get<mset_transform_precoder>();
  const auto& metric_xform_precode = odu_metrics.du.low.pusch_metrics.xform_precoder_metrics;
  double      xform_precode_cpu    = 100.0 * metric_xform_precode.cpu_usage_us / metric_period_us;
  xform_precoder.write<metric_average_latency>(metric_xform_precode.avg_latency_us);
  xform_precoder.write<metric_cpu_usage>(xform_precode_cpu);

  // UL FEC.
  const auto& metric_crc_ul       = odu_metrics.du.low.pusch_metrics.crc_metrics;
  double      ul_fec_cpu_usage_us = metric_dec.cpu_usage_us + metric_rdm.cpu_usage_us + metric_crc_ul.cpu_usage_us;
  double      ul_fec_cpu_usage    = 100.0 * ul_fec_cpu_usage_us / metric_period_us;
  auto&       fec_ul              = upper_phy_ul.get<mset_fec_ul>();
  fec_ul.write<metric_avg_throughput_mbps>(metric_dec.decoding_rate_Mbps);
  fec_ul.write<metric_cpu_usage>(ul_fec_cpu_usage);

  // L1 UL processing.
  const auto& metric_pusch_proc = odu_metrics.du.low.pusch_metrics.pusch_proc_metrics;
  // Since FEC is running asynchronously in a separate thread pool, we add its CPU usage to the rest of L1 processing.
  double upper_phy_ul_cpu_usage = 100.0 * metric_pusch_proc.cpu_usage_us / metric_period_us + ul_fec_cpu_usage;

  upper_phy_ul.write<metric_avg_throughput_mbps>(metric_pusch_proc.processing_rate_Mbps);
  upper_phy_ul.write<metric_cpu_usage>(upper_phy_ul_cpu_usage);

  auto& algorithms = upper_phy_ul.get<mset_algorithms>();
  algorithms.write<metric_bler>(metric_pusch_proc.decoding_bler);
  algorithms.write<metric_evm>(metric_pusch_proc.evm);
  algorithms.write<metric_sinr>(metric_pusch_proc.sinr_dB);

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}

/// Size in bytes of the buffer used to prepare the metrics before logging.
static constexpr unsigned str_buffer_size = 4096;

/// Return the given value if it is not a Nan or Inf, otherwise returns 0.
static double validate_fp_value(double value)
{
  if (!std::isnan(value) && !std::isinf(value)) {
    return value;
  }
  return 0.0;
}

static void log_upper_phy_metrics_verbose(fmt::basic_memory_buffer<char, str_buffer_size>& buffer,
                                          const srs_du::o_du_low_metrics&                  du_lo)
{
  // The following variables are used to break-down PUSCH processing into smaller blocks.

  // Percent of total PUSCH processing time used by the channel estimation.
  double channel_estimator_percent = validate_fp_value(
      100.0 * static_cast<double>(du_lo.pusch_metrics.ch_estimator_metrics.total_processing_latency.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by port channel estimation.
  double port_channel_estimator_percent = validate_fp_value(
      100.0 * static_cast<double>(du_lo.pusch_metrics.port_ch_estimator_metrics.total_processing_latency.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by time alignment estimator.
  double ta_estimator_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.pusch_metrics.pusch_ta_est_metrics.total_proc_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by demodulator.
  double demodulator_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.pusch_metrics.pusch_demod_metrics.total_proc_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the channel equalizer.
  double channel_equalizer_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.pusch_metrics.ch_equalizer_metrics.total_proc_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by demodulation mapper.
  double demodulation_mapper_percent = validate_fp_value(
      100.0 * static_cast<double>(du_lo.pusch_metrics.demod_demapper_metrics.total_proc_time.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by EVM calculation.
  double evm_calc_percent = validate_fp_value(
      100.0 * static_cast<double>(du_lo.pusch_metrics.evm_calculator_metrics.total_proc_time.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the scrambling.
  double scrambling_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.pusch_metrics.scrambling_metrics.total_proc_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the transform precoding.
  double transform_precoding_percent = validate_fp_value(
      100.0 * static_cast<double>(du_lo.pusch_metrics.xform_precoder_metrics.total_proc_latency.count()) /
      static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by ULSCH demultiplexing.
  double ulsch_demux_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.pusch_metrics.ulsch_demux_metrics.total_proc_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  // Percent of total PUSCH processing time used by the LDPC decoder (asynchronous).
  double ldpc_decoding_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.ldpc_metrics.decoder_metrics.total_proc_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  double pusch_wait_time_percent =
      validate_fp_value(100.0 * static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_wait_time.count()) /
                        static_cast<double>(du_lo.pusch_metrics.pusch_proc_metrics.total_proc_time.count()));

  const auto& ldpc_encoder = du_lo.ldpc_metrics.encoder_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_cb_size={:.2f} bits, avg_latency={:.2f} us, max_latency={:.2f} us, "
                 "encode_rate={:.2f} Mbps\n",
                 "  LDPC Encoder:",
                 validate_fp_value(ldpc_encoder.avg_cb_size),
                 validate_fp_value(ldpc_encoder.avg_cb_latency),
                 validate_fp_value(ldpc_encoder.max_cb_latency_us),
                 validate_fp_value(ldpc_encoder.encoding_rate_Mbps));

  const auto& ldpc_rm = du_lo.ldpc_metrics.rate_match_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  LDPC Rate matcher:",
                 validate_fp_value(ldpc_rm.avg_cb_latency),
                 validate_fp_value(ldpc_rm.processing_rate_Mbps));

  const auto& ldpc_decoder = du_lo.ldpc_metrics.decoder_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_cb_size={:.2f} bits, avg_latency={:.2f} us, max_latency={:.2f} us, "
                 "avg_nof_iter={:.2f}, decode_rate={:.2f} Mbps\n",
                 "  LDPC Decoder:",
                 validate_fp_value(ldpc_decoder.avg_cb_size),
                 validate_fp_value(ldpc_decoder.avg_cb_latency_us),
                 validate_fp_value(ldpc_decoder.max_cb_latency_us),
                 validate_fp_value(ldpc_decoder.avg_nof_iterations),
                 validate_fp_value(ldpc_decoder.decoding_rate_Mbps));

  const auto& ldpc_rdm = du_lo.ldpc_metrics.rate_dematch_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, max_latency={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  LDPC Rate dematcher:",
                 validate_fp_value(ldpc_rdm.avg_cb_latency),
                 validate_fp_value(ldpc_rdm.max_cb_latency_us),
                 validate_fp_value(ldpc_rdm.processing_rate_Mbps));

  const auto& ch_est = du_lo.pusch_metrics.ch_estimator_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, proc_rate={:.2f} MPRBps\n",
                 "  PUSCH Channel estimator:",
                 validate_fp_value(ch_est.avg_processing_latency),
                 validate_fp_value(ch_est.processing_rate_Mbps));

  const auto& ch_eq = du_lo.pusch_metrics.ch_equalizer_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} proc_rate_per_nof_layers=[{:.2f}, {:.2f}, {:.2f}, {:.2f}] MREps\n",
                 "  PUSCH channel equalizer:",
                 validate_fp_value(ch_eq.avg_rate_MREps_one_layer),
                 validate_fp_value(ch_eq.avg_rate_MREps_two_layers),
                 validate_fp_value(ch_eq.avg_rate_MREps_three_layers),
                 validate_fp_value(ch_eq.avg_rate_MREps_four_layers));

  const auto& prg_pdsch = du_lo.pdsch_metrics.scrambling_metrics;
  const auto& prg_pusch = du_lo.pusch_metrics.scrambling_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_init_time={:.2f} us, avg_advance_rate={:.2f} Mbps, avg_gen_rate={:.2f} Mbps\n",
                 "  PUSCH PRG:",
                 validate_fp_value(prg_pusch.avg_init_time_us),
                 validate_fp_value(prg_pusch.advance_rate_Mbps),
                 validate_fp_value(prg_pusch.generate_rate_Mbps));
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_init_time={:.2f} us, avg_advance_rate={:.2f} Mbps, avg_gen_rate={:.2f} Mbps\n",
                 "  PDSCH PRG:",
                 validate_fp_value(prg_pdsch.avg_init_time_us),
                 validate_fp_value(prg_pdsch.advance_rate_Mbps),
                 validate_fp_value(prg_pdsch.generate_rate_Mbps));

  const auto& xform = du_lo.pusch_metrics.xform_precoder_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} proc_rate={:.2f} MREps, avg_latency={:.2f} us/symbol\n",
                 "  Transform precoding:",
                 validate_fp_value(xform.avg_rate_MREps),
                 validate_fp_value(xform.avg_latency_us));

  const auto& pusch_demod_map = du_lo.pusch_metrics.demod_demapper_metrics;
  const auto& pusch_evm       = du_lo.pusch_metrics.evm_calculator_metrics;
  const auto& pdsch_mod       = du_lo.pdsch_metrics.modulator_metrics;
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

  const auto& sch_demux = du_lo.pusch_metrics.ulsch_demux_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_init_time={:.2f} us, avg_finish_time={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  UL-SCH demux:",
                 validate_fp_value(sch_demux.avg_init_time_us),
                 validate_fp_value(sch_demux.avg_finish_time_us),
                 validate_fp_value(sch_demux.avg_rate_Mbps));

  const auto& pusch_proc = du_lo.pusch_metrics.pusch_proc_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_data_latency={:.2f} us, avg_uci_latency={:.2f} us, proc_rate={:.2f} Mbps, "
                 "decoder_time={:.2f}% (asynchronous)\n",
                 "  PUSCH Processor:",
                 validate_fp_value(pusch_proc.avg_data_latency_us),
                 validate_fp_value(pusch_proc.avg_uci_latency_us),
                 validate_fp_value(pusch_proc.processing_rate_Mbps),
                 validate_fp_value(ldpc_decoding_percent));

  const auto& p = du_lo.pdsch_metrics.precoding_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} proc_rate_per_nof_layers=[{:.2f}, {:.2f}, {:.2f}, {:.2f}] MREps\n",
                 "  PDSCH precoding:",
                 validate_fp_value(p.avg_rate_MRes_one_layer),
                 validate_fp_value(p.avg_rate_MRes_two_layers),
                 validate_fp_value(p.avg_rate_MRes_three_layers),
                 validate_fp_value(p.avg_rate_MRes_four_layers));

  const auto& pdsch_proc = du_lo.pdsch_metrics.pdsch_proc_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_latency={:.2f} us, avg_return_latency={:.2f} us, proc_rate={:.2f} Mbps\n",
                 "  PDSCH Processor:",
                 validate_fp_value(pdsch_proc.avg_latency_us),
                 validate_fp_value(pdsch_proc.avg_return_time_us),
                 validate_fp_value(pdsch_proc.processing_rate_Mbps));

  const auto& pdsch_dmrs = du_lo.pdsch_metrics.dmrs_metrics;
  fmt::format_to(std::back_inserter(buffer),
                 "{:<25} avg_generation_latency={:.2f} us\n",
                 "  PDSCH DM-RS:",
                 validate_fp_value(pdsch_dmrs.avg_latency_us));

  const auto& crc_pdsch = du_lo.pdsch_metrics.crc_metrics;
  const auto& crc_pusch = du_lo.pusch_metrics.crc_metrics;
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
}

static void log_handle_du_low_metrics(srslog::basic_logger& logger, const srs_du::o_du_low_metrics& du_lo, bool verbose)
{
  if (du_lo.metrics_period == std::chrono::microseconds()) {
    return;
  }

  fmt::basic_memory_buffer<char, str_buffer_size> buffer;
  fmt::format_to(std::back_inserter(buffer), "Upper PHY metrics:\n");

  // Verbose logging.
  if (verbose) {
    log_upper_phy_metrics_verbose(buffer, du_lo);
  }

  const auto& ldpc_encoder    = du_lo.ldpc_metrics.encoder_metrics;
  const auto& ldpc_decoder    = du_lo.ldpc_metrics.decoder_metrics;
  const auto& ldpc_rm         = du_lo.ldpc_metrics.rate_match_metrics;
  const auto& ldpc_rdm        = du_lo.ldpc_metrics.rate_dematch_metrics;
  const auto& crc_pdsch       = du_lo.pdsch_metrics.crc_metrics;
  const auto& crc_pusch       = du_lo.pusch_metrics.crc_metrics;
  const auto& prg_pdsch       = du_lo.pdsch_metrics.scrambling_metrics;
  const auto& prg_pusch       = du_lo.pusch_metrics.scrambling_metrics;
  const auto& pusch_demod_map = du_lo.pusch_metrics.demod_demapper_metrics;
  const auto& pdsch_mod       = du_lo.pdsch_metrics.modulator_metrics;
  const auto& pdsch_dmrs      = du_lo.pdsch_metrics.dmrs_metrics;
  const auto& p               = du_lo.pdsch_metrics.precoding_metrics;
  const auto& xform           = du_lo.pusch_metrics.xform_precoder_metrics;
  const auto& pdsch_proc      = du_lo.pdsch_metrics.pdsch_proc_metrics;
  const auto& pusch_proc      = du_lo.pusch_metrics.pusch_proc_metrics;

  // CPU consumption.
  double metric_period_us   = static_cast<double>(du_lo.metrics_period.count());
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

  // Flush buffer to the logger.
  logger.info("{}", to_c_str(buffer));
}

void flexible_o_du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const flexible_o_du_metrics& odu_metrics =
      static_cast<const flexible_o_du_app_service_metrics_impl&>(metric).get_metrics();

  log_handle_du_low_metrics(logger, odu_metrics.du.low, verbose);
}
