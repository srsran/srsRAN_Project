/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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
 * PDCP TX entity metrics collection. This also includes formatting
 * helpers for printing the metrics.
 */
namespace srsgnb {

/// This struct will hold relevant metrics for the PDCP TX
struct pdcp_tx_metrics_container {
  uint32_t num_sdus;
  uint32_t num_sdu_bytes;
  uint32_t num_pdus;
  uint32_t num_pdu_bytes;
  uint32_t num_discard_timeouts;
};

/// This interface will allow the relevant entities to query PDCP RX
/// metrics (and possibly reset them).
class pdcp_tx_metrics_interface
{
public:
  pdcp_tx_metrics_interface()                                             = default;
  virtual ~pdcp_tx_metrics_interface()                                    = default;
  pdcp_tx_metrics_interface(const pdcp_tx_metrics_interface&)             = delete;
  pdcp_tx_metrics_interface& operator=(const pdcp_tx_metrics_interface&)  = delete;
  pdcp_tx_metrics_interface(const pdcp_tx_metrics_interface&&)            = delete;
  pdcp_tx_metrics_interface& operator=(const pdcp_tx_metrics_interface&&) = delete;

  /// Metrics interfaces.
  virtual pdcp_tx_metrics_container get_metrics()           = 0;
  virtual pdcp_tx_metrics_container get_metrics_and_reset() = 0;
  virtual void                      reset_metrics()         = 0;
};

} // namespace srsgnb

namespace fmt {
// PDCP RX metrics formatter
template <>
struct formatter<srsgnb::pdcp_tx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::pdcp_tx_metrics_container m, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "num_sdus={}, num_sdu_bytes={}, num_pdus={}, num_pdu_bytes={}, num_discard_timeouts={}",
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_discard_timeouts);
  }
};
} // namespace fmt
