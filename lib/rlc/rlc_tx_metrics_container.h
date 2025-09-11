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

#include "srsran/rlc/rlc_mode.h"
#include "srsran/rlc/rlc_tx_metrics.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

class rlc_tx_metrics_high_container
{
  rlc_tx_metrics_higher metrics_hi = {};
  bool                  enabled    = false;

public:
  rlc_tx_metrics_high_container(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void metrics_add_sdus(uint32_t num_sdus, size_t num_sdu_bytes)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_sdus += num_sdus;
    metrics_hi.num_sdu_bytes += num_sdu_bytes;
  }

  void metrics_add_lost_sdus(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_dropped_sdus += num_sdus;
  }

  void metrics_add_discard(uint32_t num_discarded_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_discarded_sdus += num_discarded_sdus;
  }

  void metrics_add_discard_failure(uint32_t num_discard_failures)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_discard_failures += num_discard_failures;
  }

  // Metrics getters and setters
  rlc_tx_metrics_higher get_hi_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    return metrics_hi.get();
  }

  void reset_metrics()
  {
    srsran_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    metrics_hi.reset(); // do not reset the counter
  }

  rlc_tx_metrics_higher get_and_reset_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {false};
    }
    rlc_tx_metrics_higher ret = get_hi_metrics();
    reset_metrics();
    return ret;
  }
};

class rlc_tx_metrics_low_container
{
  rlc_tx_metrics_lower metrics_lo = {};
  bool                 enabled    = false;

public:
  rlc_tx_metrics_low_container(bool enabled_) : enabled(enabled_) {}

  bool is_enabled() const { return enabled; }

  void metrics_set_mode(rlc_mode mode)
  {
    if (not enabled) {
      return;
    }
    switch (mode) {
      case rlc_mode::tm:
        metrics_lo.mode_specific = rlc_tm_tx_metrics_lower{};
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
      case rlc_mode::um_unidir_ul:
        metrics_lo.mode_specific = rlc_um_tx_metrics_lower{};
        break;
      case rlc_mode::am:
        metrics_lo.mode_specific = rlc_am_tx_metrics_lower{};
        break;
      default:
        break;
    }
  }

  void metrics_add_pdus_no_segmentation(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.num_pdus_no_segmentation += num_pdus;
    metrics_lo.num_pdu_bytes_no_segmentation += num_pdu_bytes;
  }

  void metrics_add_pulled_sdus(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.num_of_pulled_sdus += num_sdus;
  }

  void metrics_add_sdu_latency_us(uint32_t sdu_latency)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.sum_sdu_latency_us += sdu_latency;
  }

  void metrics_add_pdu_latency_ns(uint32_t pdu_latency)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.sum_pdu_latency_ns += pdu_latency;

    unsigned bin_idx = pdu_latency / (1000 * rlc_tx_metrics_lower::nof_usec_per_bin);

    bin_idx = std::min(bin_idx, rlc_tx_metrics_lower::pdu_latency_hist_bins - 1);
    metrics_lo.pdu_latency_hist_ns[bin_idx]++;

    metrics_lo.max_pdu_latency_ns = std::max(pdu_latency, metrics_lo.max_pdu_latency_ns);
  }

  // TM specific metrics
  void metrics_add_small_alloc(uint32_t num_allocs)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_tm_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for TM metrics.");
    std::get<rlc_tm_tx_metrics_lower>(metrics_lo.mode_specific).num_small_allocs += num_allocs;
  }

  // UM specific metrics
  void metrics_add_pdus_with_segmentation_um(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_um_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for UM metrics.");
    auto& um = std::get<rlc_um_tx_metrics_lower>(metrics_lo.mode_specific);
    um.num_pdus_with_segmentation += num_pdus;
    um.num_pdu_bytes_with_segmentation += num_pdu_bytes;
  }

  // AM specific metrics
  void metrics_add_pdus_with_segmentation_am(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific);
    am.num_pdus_with_segmentation += num_pdus;
    am.num_pdu_bytes_with_segmentation += num_pdu_bytes;
  }

  void metrics_add_retx_pdus(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific);
    am.num_retx_pdus += num_pdus;
    am.num_retx_pdu_bytes += num_pdu_bytes;
  }

  void metrics_add_ctrl_pdus(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific);
    am.num_ctrl_pdus += num_pdus;
    am.num_ctrl_pdu_bytes += num_pdu_bytes;
  }

  void metrics_add_ack_latency_ms(uint32_t ack_latency_ms)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific);
    am.sum_ack_latency_ms += ack_latency_ms;
    am.num_ack_latency_meas++;

    am.min_ack_latency_ms = am.min_ack_latency_ms ? std::min(*am.min_ack_latency_ms, ack_latency_ms) : ack_latency_ms;
    am.max_ack_latency_ms = std::max(am.max_ack_latency_ms, std::optional<uint32_t>{ack_latency_ms});
  }

  void metrics_add_handle_status_latency_us(uint32_t processed_acks,
                                            uint32_t processed_nacks,
                                            uint32_t handle_status_latency_us)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific);
    am.sum_handle_status_latency_us += handle_status_latency_us;
    am.num_handle_status_latency_meas++;

    am.min_handle_status_latency_us = am.min_handle_status_latency_us
                                          ? std::min(*am.min_handle_status_latency_us, handle_status_latency_us)
                                          : handle_status_latency_us;
    if (am.max_handle_status_latency_us < handle_status_latency_us) {
      am.max_handle_status_latency_us = handle_status_latency_us;
      am.max_processed_acks           = processed_acks;
      am.max_processed_nacks          = processed_nacks;
    }
  }

  void metrics_add_t_poll_retransmission_expiry_latency_us(uint32_t t_poll_latency_us)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(std::holds_alternative<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific),
                  "Wrong mode for AM metrics.");
    auto& am = std::get<rlc_am_tx_metrics_lower>(metrics_lo.mode_specific);
    am.poll_latency.sum_latency_us += t_poll_latency_us;
    am.poll_latency.num_latency_meas++;

    am.poll_latency.min_latency_us = am.poll_latency.min_latency_us
                                         ? std::min(*am.poll_latency.min_latency_us, t_poll_latency_us)
                                         : t_poll_latency_us;
    if (am.poll_latency.max_latency_us < t_poll_latency_us) {
      am.poll_latency.max_latency_us = t_poll_latency_us;
    }
  }

  // Metrics getters and setters
  rlc_tx_metrics_lower get_low_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    rlc_tx_metrics_lower ret;

    // Metrics accessed from higher layer
    return metrics_lo.get();
  }

  void reset_metrics()
  {
    srsran_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    metrics_lo.reset();
  }

  rlc_tx_metrics_lower get_and_reset_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    rlc_tx_metrics_lower ret = get_low_metrics();
    reset_metrics();
    return ret;
  }
};
} // namespace srsran
