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

#include "srsran/pdcp/pdcp_rx_metrics.h"
#include "srsran/support/srsran_assert.h"
#include <atomic>

namespace srsran {
class pdcp_rx_metrics
{
  pdcp_rx_metrics_container metrics = {};
  bool                      enabled = false;

public:
  pdcp_rx_metrics(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void add_data_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_data_pdus += num_pdus_;
    metrics.num_data_pdu_bytes += num_pdu_bytes_;
  }

  void add_lost_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_dropped_pdus += num_pdus_;
  }

  void add_dropped_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_dropped_pdus += num_pdus_;
  }

  void add_integrity_verified_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_integrity_verified_pdus += num_pdus_;
  }

  void add_integrity_failed_pdus(uint32_t num_pdus_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_integrity_failed_pdus += num_pdus_;
  }

  void add_t_reordering_timeouts(uint32_t num_timeouts_)
  {
    if (not enabled) {
      return;
    }
    metrics.num_t_reordering_timeouts += num_timeouts_;
  }

  void add_reordering_delay_us(uint32_t reordering_delay_us_)
  {
    if (not enabled) {
      return;
    }
    metrics.reordering_delay_us += reordering_delay_us_;
    metrics.reordering_counter++;
  }

  // Called from the security engine, possibly from many threads.
  void add_crypto_processing_latency(uint32_t crypto_processing_latency)
  {
    if (not enabled) {
      return;
    }
    sum_crypto_processing_latency_ns.fetch_add(crypto_processing_latency, std::memory_order_relaxed);
  }

  void add_sdu_latency_ns(uint32_t sdu_latency_ns)
  {
    if (not enabled) {
      return;
    }
    metrics.sum_sdu_latency_ns += sdu_latency_ns;

    unsigned bin_idx = sdu_latency_ns / (1000 * pdcp_rx_metrics_container::nof_usec_per_bin);

    bin_idx = std::min(bin_idx, pdcp_rx_metrics_container::sdu_latency_hist_bins - 1);
    metrics.sdu_latency_hist[bin_idx]++;

    metrics.min_sdu_latency_ns = std::min(metrics.min_sdu_latency_ns, std::optional<uint32_t>{sdu_latency_ns});
    metrics.max_sdu_latency_ns = std::max(metrics.max_sdu_latency_ns, std::optional<uint32_t>{sdu_latency_ns});
  }

  pdcp_rx_metrics_container get_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    pdcp_rx_metrics_container ret        = metrics.copy();
    ret.sum_crypto_processing_latency_ns = sum_crypto_processing_latency_ns.load(std::memory_order_relaxed);
    return ret;
  }

  pdcp_rx_metrics_container get_metrics_and_reset()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    pdcp_rx_metrics_container ret = get_metrics();
    reset_metrics();
    return ret;
  }

  void reset_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    unsigned counter = metrics.counter;
    metrics          = {};
    metrics.counter  = counter;

    sum_crypto_processing_latency_ns.store(0, std::memory_order_relaxed);
  }

private:
  std::atomic<uint32_t> sum_crypto_processing_latency_ns = 0;
};

} // namespace srsran
