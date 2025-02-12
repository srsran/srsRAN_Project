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
  metrics.decoder_metrics.avg_cb_size        = ldpc_decoder_aggregator.get_avg_cb_size();
  metrics.decoder_metrics.avg_nof_iterations = ldpc_decoder_aggregator.get_avg_nof_iterations();
  metrics.decoder_metrics.decoding_rate_Mbps = ldpc_decoder_aggregator.get_decode_rate_Mbps();
  metrics.decoder_metrics.cpu_usage          = ldpc_decoder_aggregator.get_cpu_utilization();
  ldpc_decoder_aggregator.reset();

  // Collect metrics from the rest of LDPC aggregators...
}
