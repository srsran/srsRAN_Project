/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/rlc/rlc_tx_metrics.h"

namespace srsran {

struct rlc_tx_metrics_atomic_higher {
  std::atomic<uint32_t> num_sdus;             ///< Number of SDUs
  std::atomic<size_t>   num_sdu_bytes;        ///< Number of SDU bytes
  std::atomic<uint32_t> num_dropped_sdus;     ///< Number of dropped SDUs (due to full queue)
  std::atomic<uint32_t> num_discarded_sdus;   ///< Number of discarded SDUs (instructed from higher layer)
  std::atomic<uint32_t> num_discard_failures; ///< Number of failed SDU discards (instructed from higher layer)

  void reset()
  {
    num_sdus.store(0, std::memory_order_relaxed);
    num_sdu_bytes.store(0, std::memory_order_relaxed);
    num_dropped_sdus.store(0, std::memory_order_relaxed);
    num_discarded_sdus.store(0, std::memory_order_relaxed);
    num_discard_failures.store(0, std::memory_order_relaxed);
  }
};

struct rlc_tm_tx_metrics_atomic_lower {
  std::atomic<uint32_t> num_small_allocs; ///< Number of allocations that are too small to TX PDU

  void reset() { num_small_allocs.store(0, std::memory_order_relaxed); }
};

struct rlc_um_tx_metrics_atomic_lower {
  std::atomic<uint32_t> num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  std::atomic<uint32_t> num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation

  void reset()
  {
    num_pdus_with_segmentation.store(0, std::memory_order_relaxed);
    num_pdu_bytes_with_segmentation.store(0, std::memory_order_relaxed);
  }
};

struct rlc_am_tx_metrics_atomic_lower {
  std::atomic<uint32_t> num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  std::atomic<uint32_t> num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation
  std::atomic<uint32_t> num_retx_pdus;                   ///< Number of retransmitted PDUs
  std::atomic<uint32_t> num_retx_pdu_bytes;              ///< Number of retransmitted PDU bytes
  std::atomic<uint32_t> num_ctrl_pdus;                   ///< Number of control PDUs
  std::atomic<uint32_t> num_ctrl_pdu_bytes;              ///< Number of control PDUs bytes

  void reset()
  {
    num_pdus_with_segmentation.store(0, std::memory_order_relaxed);
    num_pdu_bytes_with_segmentation.store(0, std::memory_order_relaxed);
    num_retx_pdus.store(0, std::memory_order_relaxed);
    num_retx_pdu_bytes.store(0, std::memory_order_relaxed);
    num_ctrl_pdus.store(0, std::memory_order_relaxed);
    num_ctrl_pdu_bytes.store(0, std::memory_order_relaxed);
  }
};

struct rlc_tx_metrics_atomic_lower {
  std::atomic<uint32_t> num_pdus_no_segmentation;      ///< Number of transmitted PDUs without segmentation
  std::atomic<size_t>   num_pdu_bytes_no_segmentation; ///< Number of transmitted PDU bytes without segmentation

  /// RLC mode of the entity
  std::atomic<rlc_mode> mode;

  /// Mode-specific metrics
  ///
  /// The associated union member is indicated by \c mode.
  /// Contents of the other fields are undefined.
  union {
    rlc_tm_tx_metrics_atomic_lower tm;
    rlc_um_tx_metrics_atomic_lower um;
    rlc_am_tx_metrics_atomic_lower am;
  } mode_specific;

  void reset()
  {
    num_pdus_no_segmentation.store(0, std::memory_order_relaxed);
    num_pdu_bytes_no_segmentation.store(0, std::memory_order_relaxed);

    // reset mode-specific values
    switch (mode.load(std::memory_order_relaxed)) {
      case rlc_mode::tm:
        mode_specific.tm.reset();
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        mode_specific.um.reset();
        break;
      case rlc_mode::am:
        mode_specific.am.reset();
        break;
      default:
        // nothing to do here
        break;
    }
  }
};

class rlc_tx_metrics_container
{
  rlc_tx_metrics_atomic_higher metrics_hi = {};
  rlc_tx_metrics_atomic_lower  metrics_lo = {};
  bool                         enabled    = false;

public:
  rlc_tx_metrics_container(bool enabled_) : enabled(enabled_) {}

  void metrics_set_mode(rlc_mode mode)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.mode.store(mode, std::memory_order_relaxed);
  }

  void metrics_add_sdus(uint32_t num_sdus, size_t num_sdu_bytes)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_sdus.fetch_add(num_sdus, std::memory_order_relaxed);
    metrics_hi.num_sdu_bytes.fetch_add(num_sdu_bytes, std::memory_order_relaxed);
  }

  void metrics_add_lost_sdus(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_dropped_sdus.fetch_add(num_sdus, std::memory_order_relaxed);
  }

  void metrics_add_discard(uint32_t num_discarded_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_discarded_sdus.fetch_add(num_discarded_sdus, std::memory_order_relaxed);
  }

  void metrics_add_discard_failure(uint32_t num_discard_failures)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_discard_failures.fetch_add(num_discard_failures, std::memory_order_relaxed);
  }

  void metrics_add_pdus_no_segmentation(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.num_pdus_no_segmentation.fetch_add(num_pdus, std::memory_order_relaxed);
    metrics_lo.num_pdu_bytes_no_segmentation.fetch_add(num_pdu_bytes, std::memory_order_relaxed);
  }

  // TM specific metrics
  void metrics_add_small_alloc(uint32_t num_allocs)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode.load(std::memory_order_relaxed) == rlc_mode::tm, "Wrong mode for TM metrics.");
    metrics_lo.mode_specific.tm.num_small_allocs.fetch_add(num_allocs, std::memory_order_relaxed);
  }

  // UM specific metrics
  void metrics_add_pdus_with_segmentation_um(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    rlc_mode mode = metrics_lo.mode.load(std::memory_order_relaxed);
    srsran_assert(mode == rlc_mode::um_bidir || mode == rlc_mode::um_unidir_dl, "Wrong mode for UM metrics.");
    metrics_lo.mode_specific.um.num_pdus_with_segmentation.fetch_add(num_pdus, std::memory_order_relaxed);
    metrics_lo.mode_specific.um.num_pdu_bytes_with_segmentation.fetch_add(num_pdu_bytes, std::memory_order_relaxed);
  }

  // AM specific metrics
  void metrics_add_pdus_with_segmentation_am(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode.load(std::memory_order_relaxed) == rlc_mode::am, "Wrong mode for AM metrics.");
    metrics_lo.mode_specific.am.num_pdus_with_segmentation.fetch_add(num_pdus, std::memory_order_relaxed);
    metrics_lo.mode_specific.am.num_pdu_bytes_with_segmentation.fetch_add(num_pdu_bytes, std::memory_order_relaxed);
  }

  void metrics_add_retx_pdus(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode.load(std::memory_order_relaxed) == rlc_mode::am, "Wrong mode for AM metrics.");
    metrics_lo.mode_specific.am.num_retx_pdus.fetch_add(num_pdus, std::memory_order_relaxed);
    metrics_lo.mode_specific.am.num_retx_pdu_bytes.fetch_add(num_pdu_bytes, std::memory_order_relaxed);
  }

  void metrics_add_ctrl_pdus(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode.load(std::memory_order_relaxed) == rlc_mode::am, "Wrong mode for AM metrics.");
    metrics_lo.mode_specific.am.num_ctrl_pdus.fetch_add(num_pdus, std::memory_order_relaxed);
    metrics_lo.mode_specific.am.num_ctrl_pdu_bytes.fetch_add(num_pdu_bytes, std::memory_order_relaxed);
  }

  // Metrics getters and setters
  rlc_tx_metrics get_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    rlc_tx_metrics ret;

    // Metrics accessed from higher layer
    ret.num_sdus             = metrics_hi.num_sdus.load(std::memory_order_relaxed);
    ret.num_sdu_bytes        = metrics_hi.num_sdu_bytes.load(std::memory_order_relaxed);
    ret.num_dropped_sdus     = metrics_hi.num_dropped_sdus.load(std::memory_order_relaxed);
    ret.num_discarded_sdus   = metrics_hi.num_discarded_sdus.load(std::memory_order_relaxed);
    ret.num_discard_failures = metrics_hi.num_discard_failures.load(std::memory_order_relaxed);

    // Metrics accessed from lower layer
    ret.num_pdus_no_segmentation      = metrics_lo.num_pdus_no_segmentation.load(std::memory_order_relaxed);
    ret.num_pdu_bytes_no_segmentation = metrics_lo.num_pdu_bytes_no_segmentation.load(std::memory_order_relaxed);
    ret.mode                          = metrics_lo.mode.load(std::memory_order_relaxed);
    switch (ret.mode) {
      case rlc_mode::tm:
        ret.mode_specific.tm.num_small_allocs =
            metrics_lo.mode_specific.tm.num_small_allocs.load(std::memory_order_relaxed);
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        ret.mode_specific.um.num_pdus_with_segmentation =
            metrics_lo.mode_specific.um.num_pdus_with_segmentation.load(std::memory_order_relaxed);
        ret.mode_specific.um.num_pdu_bytes_with_segmentation =
            metrics_lo.mode_specific.um.num_pdu_bytes_with_segmentation.load(std::memory_order_relaxed);
        break;
      case rlc_mode::am:
        ret.mode_specific.am.num_pdus_with_segmentation =
            metrics_lo.mode_specific.am.num_pdus_with_segmentation.load(std::memory_order_relaxed);
        ret.mode_specific.am.num_pdu_bytes_with_segmentation =
            metrics_lo.mode_specific.am.num_pdu_bytes_with_segmentation.load(std::memory_order_relaxed);
        ret.mode_specific.am.num_retx_pdus = metrics_lo.mode_specific.am.num_retx_pdus.load(std::memory_order_relaxed);
        ret.mode_specific.am.num_retx_pdu_bytes =
            metrics_lo.mode_specific.am.num_retx_pdu_bytes.load(std::memory_order_relaxed);
        ret.mode_specific.am.num_ctrl_pdus = metrics_lo.mode_specific.am.num_ctrl_pdus.load(std::memory_order_relaxed);
        ret.mode_specific.am.num_ctrl_pdu_bytes =
            metrics_lo.mode_specific.am.num_ctrl_pdu_bytes.load(std::memory_order_relaxed);
        break;
      default:
        // nothing to do here
        break;
    }

    return ret;
  }

  void reset_metrics()
  {
    srsran_assert(enabled, "Trying to reset metrics, but metrics are disabled.");
    if (not enabled) {
      return;
    }
    metrics_hi.reset();
    metrics_lo.reset();
  }

  rlc_tx_metrics get_and_reset_metrics()
  {
    srsran_assert(enabled, "Trying to get metrics, but metrics are disabled.");
    if (not enabled) {
      return {};
    }
    rlc_tx_metrics ret = get_metrics();
    reset_metrics();
    return ret;
  }
};
} // namespace srsran
