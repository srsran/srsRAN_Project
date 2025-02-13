/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_ldpc_metrics_collector.h"

using namespace srsran;
using namespace srs_du;

void du_low_ldpc_metrics_collector::collect_metrics(o_du_low_ldpc_metrics& metrics)
{
  // Collect metrics from the LDPC decoder metrics aggregator.
  metrics.decoder_metrics.avg_cb_latency_us  = ldpc_decoder_aggregator.get_avg_cb_latency_us();
  metrics.decoder_metrics.min_cb_latency_us  = ldpc_decoder_aggregator.get_min_cb_latency_us();
  metrics.decoder_metrics.max_cb_latency_us  = ldpc_decoder_aggregator.get_max_cb_latency_us();
  metrics.decoder_metrics.avg_cb_size        = ldpc_decoder_aggregator.get_avg_cb_size();
  metrics.decoder_metrics.avg_nof_iterations = ldpc_decoder_aggregator.get_avg_nof_iterations();
  metrics.decoder_metrics.decoding_rate_Mbps = ldpc_decoder_aggregator.get_decode_rate_Mbps();
  metrics.decoder_metrics.total_proc_time    = ldpc_decoder_aggregator.get_total_time();
  metrics.decoder_metrics.cpu_usage_us       = ldpc_decoder_aggregator.get_cpu_usage_us();
  ldpc_decoder_aggregator.reset();

  // Collect metrics from the LDPC encoder metrics aggregator.
  metrics.encoder_metrics.avg_cb_latency     = ldpc_encoder_aggregator.get_avg_cb_latency_us();
  metrics.encoder_metrics.min_cb_latency_us  = ldpc_encoder_aggregator.get_min_cb_latency_us();
  metrics.encoder_metrics.max_cb_latency_us  = ldpc_encoder_aggregator.get_max_cb_latency_us();
  metrics.encoder_metrics.avg_cb_size        = ldpc_encoder_aggregator.get_avg_cb_size();
  metrics.encoder_metrics.encoding_rate_Mbps = ldpc_encoder_aggregator.get_encode_rate_Mbps();
  metrics.encoder_metrics.cpu_usage_us       = ldpc_encoder_aggregator.get_cpu_usage_us();
  ldpc_encoder_aggregator.reset();

  // Collect metrics from the LDPC rate matcher metrics aggregator.
  metrics.rate_match_metrics.avg_cb_latency       = ldpc_rate_matcher_aggregator.get_avg_cb_latency_us();
  metrics.rate_match_metrics.min_cb_latency_us    = ldpc_rate_matcher_aggregator.get_min_cb_latency_us();
  metrics.rate_match_metrics.max_cb_latency_us    = ldpc_rate_matcher_aggregator.get_max_cb_latency_us();
  metrics.rate_match_metrics.processing_rate_Mbps = ldpc_rate_matcher_aggregator.get_avg_rate_Mbps();
  metrics.rate_match_metrics.cpu_usage_us         = ldpc_rate_matcher_aggregator.get_cpu_usage_us();
  ldpc_rate_matcher_aggregator.reset();

  // Collect metrics from the LDPC rate dematcher metrics aggregator.
  metrics.rate_dematch_metrics.avg_cb_latency       = ldpc_rate_dematcher_aggregator.get_avg_cb_latency_us();
  metrics.rate_dematch_metrics.min_cb_latency_us    = ldpc_rate_dematcher_aggregator.get_min_cb_latency_us();
  metrics.rate_dematch_metrics.max_cb_latency_us    = ldpc_rate_dematcher_aggregator.get_max_cb_latency_us();
  metrics.rate_dematch_metrics.processing_rate_Mbps = ldpc_rate_dematcher_aggregator.get_avg_rate_Mbps();
  metrics.rate_dematch_metrics.cpu_usage_us         = ldpc_rate_dematcher_aggregator.get_cpu_usage_us();
  ldpc_rate_dematcher_aggregator.reset();
}
