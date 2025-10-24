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

#include "srsran/pdcp/pdcp_tx_metrics.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class pdcp_tx_metrics
{
  pdcp_tx_metrics_container metrics = {};
  bool                      enabled = false;

public:
  pdcp_tx_metrics(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void add_lost_sdus(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics.num_dropped_sdus += num_sdus;
  }

  void add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void add_discard_timouts(uint32_t num_discard_timeouts_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_discard_timeouts += num_discard_timeouts_;
  }

  pdcp_tx_metrics_container get_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    pdcp_tx_metrics_container ret        = metrics.copy();
    ret.sum_crypto_processing_latency_ns = sum_crypto_processing_latency_ns.load(std::memory_order_relaxed);
    return ret;
  }

  pdcp_tx_metrics_container get_metrics_and_reset()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    pdcp_tx_metrics_container ret = get_metrics();
    reset_metrics();
    return ret;
  }

  void add_pdu_latency_ns(uint32_t sdu_latency_ns)
  {
    if (not enabled) {
      return;
    }
    metrics.sum_pdu_latency_ns += sdu_latency_ns;

    unsigned bin_idx = sdu_latency_ns / (1000 * pdcp_tx_metrics_container::nof_usec_per_bin);

    bin_idx = std::min(bin_idx, pdcp_tx_metrics_container::pdu_latency_hist_bins - 1);
    metrics.pdu_latency_hist[bin_idx]++;

    metrics.min_pdu_latency_ns =
        metrics.min_pdu_latency_ns ? std::min(*metrics.min_pdu_latency_ns, sdu_latency_ns) : sdu_latency_ns;
    metrics.max_pdu_latency_ns = std::max(metrics.max_pdu_latency_ns, std::optional<uint32_t>{sdu_latency_ns});
  }

  void add_crypto_processing_latency(uint32_t crypto_processing_latency)
  {
    if (not enabled) {
      return;
    }
    sum_crypto_processing_latency_ns.fetch_add(crypto_processing_latency, std::memory_order_relaxed);
  }

  void reset_metrics()
  {
    srsran_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    // Ugly way to reset all metrics but not the counter.
    unsigned counter = metrics.counter;
    metrics          = {};
    metrics.counter  = counter;

    sum_crypto_processing_latency_ns.store(0, std::memory_order_relaxed);
  }

private:
  std::atomic<uint32_t> sum_crypto_processing_latency_ns = 0;
};
} // namespace srsran
