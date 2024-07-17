/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/rlc/rlc_tx_metrics.h"

namespace srsran {

struct rlc_tx_metrics_higher {
  uint32_t num_sdus;             ///< Number of SDUs
  uint32_t num_of_pulled_sdus;   ///< Number of pulled SDUs
  size_t   num_sdu_bytes;        ///< Number of SDU bytes
  uint32_t num_dropped_sdus;     ///< Number of dropped SDUs (due to full queue)
  uint32_t num_discarded_sdus;   ///< Number of discarded SDUs (instructed from higher layer)
  uint32_t num_discard_failures; ///< Number of failed SDU discards (instructed from higher layer)
  uint32_t sum_sdu_latency_us;   ///< total SDU latency (in us)>

  void reset()
  {
    num_sdus             = {};
    num_of_pulled_sdus   = {};
    num_sdu_bytes        = {};
    num_dropped_sdus     = {};
    num_discarded_sdus   = {};
    num_discard_failures = {};
    sum_sdu_latency_us   = {};
  }
};

struct rlc_tm_tx_metrics_atomic_lower {
  uint32_t num_small_allocs; ///< Number of allocations that are too small to TX PDU

  void reset() { num_small_allocs = {}; }
};

struct rlc_um_tx_metrics_atomic_lower {
  uint32_t num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  uint32_t num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation

  void reset()
  {
    num_pdus_with_segmentation      = {};
    num_pdu_bytes_with_segmentation = {};
  }
};

struct rlc_am_tx_metrics_atomic_lower {
  uint32_t num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  uint32_t num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation
  uint32_t num_retx_pdus;                   ///< Number of retransmitted PDUs
  uint32_t num_retx_pdu_bytes;              ///< Number of retransmitted PDU bytes
  uint32_t num_ctrl_pdus;                   ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;              ///< Number of control PDUs bytes

  void reset()
  {
    num_pdus_with_segmentation      = {};
    num_pdu_bytes_with_segmentation = {};
    num_retx_pdus                   = {};
    num_retx_pdu_bytes              = {};
    num_ctrl_pdus                   = {};
    num_ctrl_pdu_bytes              = {};
  }
};

struct rlc_tx_metrics_atomic_lower {
  uint32_t num_pdus_no_segmentation;      ///< Number of transmitted PDUs without segmentation
  size_t   num_pdu_bytes_no_segmentation; ///< Number of transmitted PDU bytes without segmentation

  /// RLC mode of the entity
  rlc_mode mode;

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
    num_pdus_no_segmentation      = {};
    num_pdu_bytes_no_segmentation = {};

    // reset mode-specific values
    switch (mode) {
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
  rlc_tx_metrics_higher       metrics_hi = {};
  rlc_tx_metrics_atomic_lower metrics_lo = {};
  bool                        enabled    = false;

public:
  rlc_tx_metrics_container(bool enabled_) : enabled(enabled_) {}

  void metrics_set_mode(rlc_mode mode)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.mode = mode;
  }

  void metrics_add_sdus(uint32_t num_sdus, size_t num_sdu_bytes)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_sdus += num_sdus;
    metrics_hi.num_sdu_bytes += num_sdu_bytes;
  }

  void metrics_add_pulled_sdus(uint32_t num_sdus)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.num_of_pulled_sdus += num_sdus;
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

  void metrics_add_pdus_no_segmentation(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    metrics_lo.num_pdus_no_segmentation += num_pdus;
    metrics_lo.num_pdu_bytes_no_segmentation += num_pdu_bytes;
  }
  void metrics_add_sdu_latency_us(uint32_t sdu_latency)
  {
    if (not enabled) {
      return;
    }
    metrics_hi.sum_sdu_latency_us += sdu_latency;
  }

  // TM specific metrics
  void metrics_add_small_alloc(uint32_t num_allocs)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode == rlc_mode::tm, "Wrong mode for TM metrics.");
    metrics_lo.mode_specific.tm.num_small_allocs += num_allocs;
  }

  // UM specific metrics
  void metrics_add_pdus_with_segmentation_um(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    rlc_mode mode = metrics_lo.mode;
    srsran_assert(mode == rlc_mode::um_bidir || mode == rlc_mode::um_unidir_dl, "Wrong mode for UM metrics.");
    metrics_lo.mode_specific.um.num_pdus_with_segmentation += num_pdus;
    metrics_lo.mode_specific.um.num_pdu_bytes_with_segmentation += num_pdu_bytes;
  }

  // AM specific metrics
  void metrics_add_pdus_with_segmentation_am(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode == rlc_mode::am, "Wrong mode for AM metrics.");
    metrics_lo.mode_specific.am.num_pdus_with_segmentation += num_pdus;
    metrics_lo.mode_specific.am.num_pdu_bytes_with_segmentation += num_pdu_bytes;
  }

  void metrics_add_retx_pdus(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode == rlc_mode::am, "Wrong mode for AM metrics.");
    metrics_lo.mode_specific.am.num_retx_pdus += num_pdus;
    metrics_lo.mode_specific.am.num_retx_pdu_bytes += num_pdu_bytes;
  }

  void metrics_add_ctrl_pdus(uint32_t num_pdus, size_t num_pdu_bytes)
  {
    if (not enabled) {
      return;
    }
    srsran_assert(metrics_lo.mode == rlc_mode::am, "Wrong mode for AM metrics.");
    metrics_lo.mode_specific.am.num_ctrl_pdus += num_pdus;
    metrics_lo.mode_specific.am.num_ctrl_pdu_bytes += num_pdu_bytes;
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
    ret.num_sdus             = metrics_hi.num_sdus;
    ret.num_of_pulled_sdus   = metrics_hi.num_of_pulled_sdus;
    ret.num_sdu_bytes        = metrics_hi.num_sdu_bytes;
    ret.num_dropped_sdus     = metrics_hi.num_dropped_sdus;
    ret.num_discarded_sdus   = metrics_hi.num_discarded_sdus;
    ret.num_discard_failures = metrics_hi.num_discard_failures;
    ret.sum_sdu_latency_us   = metrics_hi.sum_sdu_latency_us;

    // Metrics accessed from lower layer
    ret.num_pdus_no_segmentation      = metrics_lo.num_pdus_no_segmentation;
    ret.num_pdu_bytes_no_segmentation = metrics_lo.num_pdu_bytes_no_segmentation;
    ret.mode                          = metrics_lo.mode;
    switch (ret.mode) {
      case rlc_mode::tm:
        ret.mode_specific.tm.num_small_allocs = metrics_lo.mode_specific.tm.num_small_allocs;
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        ret.mode_specific.um.num_pdus_with_segmentation = metrics_lo.mode_specific.um.num_pdus_with_segmentation;
        ret.mode_specific.um.num_pdu_bytes_with_segmentation =
            metrics_lo.mode_specific.um.num_pdu_bytes_with_segmentation;
        break;
      case rlc_mode::am:
        ret.mode_specific.am.num_pdus_with_segmentation = metrics_lo.mode_specific.am.num_pdus_with_segmentation;
        ret.mode_specific.am.num_pdu_bytes_with_segmentation =
            metrics_lo.mode_specific.am.num_pdu_bytes_with_segmentation;
        ret.mode_specific.am.num_retx_pdus      = metrics_lo.mode_specific.am.num_retx_pdus;
        ret.mode_specific.am.num_retx_pdu_bytes = metrics_lo.mode_specific.am.num_retx_pdu_bytes;
        ret.mode_specific.am.num_ctrl_pdus      = metrics_lo.mode_specific.am.num_ctrl_pdus;
        ret.mode_specific.am.num_ctrl_pdu_bytes = metrics_lo.mode_specific.am.num_ctrl_pdu_bytes;
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
