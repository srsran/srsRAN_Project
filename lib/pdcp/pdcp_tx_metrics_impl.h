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

#include "srsran/pdcp/pdcp_tx_metrics.h"
#include "srsran/support/resource_usage/resource_usage_utils.h"

namespace srsran {

class pdcp_tx_metrics
{
  pdcp_tx_metrics_container metrics = {};

public:
  void add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void add_discard_timouts(uint32_t num_discard_timeouts_) { metrics.num_discard_timeouts += num_discard_timeouts_; }

  pdcp_tx_metrics_container get_metrics()
  {
    pdcp_tx_metrics_container ret       = metrics;
    metrics.sum_crypto_used_cpu_time_us = sum_crypto_used_cpu_time_us;
    return ret;
  }

  pdcp_tx_metrics_container get_metrics_and_reset()
  {
    pdcp_tx_metrics_container ret = get_metrics();
    ret.counter++;
    reset_metrics();
    return ret;
  }

  void add_pdu_latency_ns(uint32_t sdu_latency_ns)
  {
    metrics.sum_pdu_latency_ns += sdu_latency_ns;

    unsigned bin_idx = sdu_latency_ns / (1000 * pdcp_tx_metrics_container::nof_usec_per_bin);

    bin_idx = std::min(bin_idx, pdcp_tx_metrics_container::pdu_latency_hist_bins - 1);
    metrics.pdu_latency_hist[bin_idx]++;

    metrics.max_pdu_latency_ns = std::max(sdu_latency_ns, metrics.max_pdu_latency_ns);
  }

  void add_cpu_usage_metrics(resource_usage_utils::measurements cpu_usage)
  {
    sum_crypto_used_cpu_time_us.fetch_add(cpu_usage.system_time.count() + cpu_usage.user_time.count(),
                                          std::memory_order_relaxed);
  }

  void reset_metrics()
  {
    metrics                     = {};
    sum_crypto_used_cpu_time_us = 0;
  }

private:
  std::atomic<uint32_t> sum_crypto_used_cpu_time_us = 0;
};
} // namespace srsran
