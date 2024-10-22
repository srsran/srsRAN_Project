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

#include "fmt/format.h"

/*
 * This file will hold the interfaces and structures for the
 * PDCP RX entity metrics collection. This also includes formatting
 * helpers for printing the metrics.
 */
namespace srsran {

/// This struct will hold relevant metrics for the PDCP RX
struct pdcp_rx_metrics_container {
  uint32_t num_pdus;
  uint32_t num_pdu_bytes;
  uint32_t num_dropped_pdus;
  uint32_t num_sdus;
  uint32_t num_sdu_bytes;
  uint32_t num_integrity_verified_pdus;
  uint32_t num_integrity_failed_pdus;
  uint32_t num_t_reordering_timeouts;
  unsigned counter;
};
} // namespace srsran

namespace fmt {
// PDCP RX metrics formatter
template <>
struct formatter<srsran::pdcp_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rx_metrics_container m, FormatContext& ctx)
  {
    return format_to(ctx.out(),
                     "num_sdus={} num_sdu_bytes={} num_dropped_pdus={} num_pdus={} num_pdu_bytes={} "
                     "num_integrity_verified_pdus={} num_integrity_failed_pdus={} num_t_reordering_timeouts={}",
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_dropped_pdus,
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_integrity_verified_pdus,
                     m.num_integrity_failed_pdus,
                     m.num_t_reordering_timeouts);
  }
};
} // namespace fmt
