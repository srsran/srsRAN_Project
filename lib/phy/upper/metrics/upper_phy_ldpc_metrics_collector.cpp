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

#include "upper_phy_ldpc_metrics_collector.h"

using namespace srsran;

void upper_phy_ldpc_metrics_collector::collect_metrics(upper_phy_ldpc_metrics& metrics)
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
  metrics.encoder_metrics.avg_cb_latency_us  = ldpc_encoder_aggregator.get_avg_cb_latency_us();
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
