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

#include "srsgnb/rlc/rlc_config.h"
#include "fmt/format.h"
#include <mutex>

namespace srsgnb {

struct rlc_tm_rx_metrics {};

struct rlc_um_rx_metrics {
  uint32_t num_sdu_segments;      ///< Number of SDU segments RX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments Bytes
};

struct rlc_am_rx_metrics {
  uint32_t num_sdu_segments;      ///< Number of SDU segments RX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments bytes
  uint32_t num_ctrl_pdus;         ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;    ///< Number of control PDUs bytes
};

struct rlc_rx_metrics {
  // SDU metrics
  uint32_t num_sdus;      ///< Number of SDUs
  size_t   num_sdu_bytes; ///< Number of SDU bytes

  // PDU metrics
  uint32_t num_pdus;           ///< Number of PDUs
  size_t   num_pdu_bytes;      ///< Number of PDU bytes
  uint32_t num_lost_pdus;      ///< Number of dropped PDUs (reassembly timeout expiry or out of rx window)
  uint32_t num_malformed_pdus; ///< Number of malformed PDUs

  /// RLC mode of the entity
  rlc_mode mode;

  /// Mode-specific metrics
  ///
  /// The associated union member is indicated by \c mode.
  /// Contents of the other fields are undefined.
  union {
    rlc_tm_rx_metrics tm;
    rlc_um_rx_metrics um;
    rlc_am_rx_metrics am;
  } mode_specific;
};

class rlc_rx_metrics_interface
{
public:
  rlc_rx_metrics_interface()                                            = default;
  virtual ~rlc_rx_metrics_interface()                                   = default;
  rlc_rx_metrics_interface(const rlc_rx_metrics_interface&)             = delete;
  rlc_rx_metrics_interface& operator=(const rlc_rx_metrics_interface&)  = delete;
  rlc_rx_metrics_interface(const rlc_rx_metrics_interface&&)            = delete;
  rlc_rx_metrics_interface& operator=(const rlc_rx_metrics_interface&&) = delete;

  virtual rlc_rx_metrics get_metrics()           = 0;
  virtual rlc_rx_metrics get_and_reset_metrics() = 0;
  virtual void           reset_metrics()         = 0;
};
} // namespace srsgnb

namespace fmt {

// RLC RX metrics formatter
// TODO print mode-specific metrics
template <>
struct formatter<srsgnb::rlc_rx_metrics> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_rx_metrics m, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "num_pdus={} num_pdu_bytes={} num_lost_pdus={} num_malformed_pdus={} num_sdus={} num_sdu_bytes={}",
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_lost_pdus,
                     m.num_malformed_pdus,
                     m.num_sdus,
                     m.num_sdu_bytes);
  }
};
} // namespace fmt
