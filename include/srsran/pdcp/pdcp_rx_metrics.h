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
};

/// This interface will allow the relevant entities to query PDCP RX
/// metrics (and possibly reset them).
class pdcp_rx_metrics_interface
{
public:
  pdcp_rx_metrics_interface()                                             = default;
  virtual ~pdcp_rx_metrics_interface()                                    = default;
  pdcp_rx_metrics_interface(const pdcp_rx_metrics_interface&)             = delete;
  pdcp_rx_metrics_interface& operator=(const pdcp_rx_metrics_interface&)  = delete;
  pdcp_rx_metrics_interface(const pdcp_rx_metrics_interface&&)            = delete;
  pdcp_rx_metrics_interface& operator=(const pdcp_rx_metrics_interface&&) = delete;

  /// Metrics interfaces.
  virtual pdcp_rx_metrics_container get_metrics()           = 0;
  virtual pdcp_rx_metrics_container get_metrics_and_reset() = 0;
  virtual void                      reset_metrics()         = 0;
};
} // namespace srsran

namespace fmt {
// PDCP RX metrics formatter
template <>
struct formatter<srsran::pdcp_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::pdcp_rx_metrics_container m, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
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
