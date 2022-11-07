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
#include "srsgnb/srslog/srslog.h"
#include <mutex>

namespace srsgnb {

struct rlc_tm_bearer_tx_metrics_container {
  uint32_t num_small_allocs; ///< Number of allocations that are too small to TX PDU
};

struct rlc_um_bearer_tx_metrics_container {
  uint32_t num_sdu_segments;      ///< Number of SDU segments TX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments Bytes
};

struct rlc_am_bearer_tx_metrics_container {
  uint32_t num_retx_pdus;         ///< Number of RETX'ed PDUs
  uint32_t num_sdu_segments;      ///< Number of SDU segments TX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments bytes
  uint32_t num_ctrl_pdus;         ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;    ///< Number of control PDUs bytes
};

struct rlc_bearer_tx_metrics_container {
  // SDU metrics
  uint32_t num_sdus;         ///< Number of SDUs
  size_t   num_sdu_bytes;    ///< Number of SDU bytes
  uint32_t num_dropped_sdus; ///< Number of dropped SDUs (due to full queue)

  // PDU metrics
  uint32_t num_pdus;      ///< Number of PDUs
  size_t   num_pdu_bytes; ///< Number of PDU bytes

  rlc_mode mode;
  union {
    rlc_tm_bearer_tx_metrics_container tm;
    rlc_um_bearer_tx_metrics_container um;
    rlc_am_bearer_tx_metrics_container am;
  } mode_specific;
};

struct rlc_tm_bearer_rx_metrics_container {};

struct rlc_um_bearer_rx_metrics_container {
  uint32_t num_sdu_segments;      ///< Number of SDU segments TX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments Bytes
};

struct rlc_am_bearer_rx_metrics_container {
  uint32_t num_sdu_segments;      ///< Number of SDU segments TX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments bytes
  uint32_t num_ctrl_pdus;         ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;    ///< Number of control PDUs bytes
};

class rlc_tx_metrics_interface
{
public:
  rlc_tx_metrics_interface()                                            = default;
  virtual ~rlc_tx_metrics_interface()                                   = default;
  rlc_tx_metrics_interface(const rlc_tx_metrics_interface&)             = delete;
  rlc_tx_metrics_interface& operator=(const rlc_tx_metrics_interface&)  = delete;
  rlc_tx_metrics_interface(const rlc_tx_metrics_interface&&)            = delete;
  rlc_tx_metrics_interface& operator=(const rlc_tx_metrics_interface&&) = delete;

  virtual rlc_bearer_tx_metrics_container get_metrics()           = 0;
  virtual rlc_bearer_tx_metrics_container get_and_reset_metrics() = 0;
  virtual void                            reset_metrics()         = 0;
};

struct rlc_bearer_rx_metrics_container {
  // SDU metrics
  uint32_t num_sdus;      ///< Number of SDUs
  size_t   num_sdu_bytes; ///< Number of SDU bytes

  // PDU metrics
  uint32_t num_pdus;           ///< Number of PDUs
  size_t   num_pdu_bytes;      ///< Number of PDU bytes
  uint32_t num_lost_pdus;      ///< Number of dropped PDUs (reassembly timeout expiry or out of rx window)
  uint32_t num_malformed_pdus; ///< Number of malformed PDUs

  rlc_mode mode;
  union {
    rlc_tm_bearer_rx_metrics_container tm;
    rlc_um_bearer_rx_metrics_container um;
    rlc_am_bearer_rx_metrics_container am;
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

  virtual rlc_bearer_rx_metrics_container get_metrics()           = 0;
  virtual rlc_bearer_rx_metrics_container get_and_reset_metrics() = 0;
  virtual void                            reset_metrics()         = 0;
};

struct rlc_bearer_metrics_container {
  rlc_bearer_tx_metrics_container tx;
  rlc_bearer_rx_metrics_container rx;
};
} // namespace srsgnb

namespace fmt {
// RLC TX metrics formatter
template <>
struct formatter<srsgnb::rlc_bearer_tx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_bearer_tx_metrics_container m, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "num_sdus={}, num_sdu_bytes={}, num_dropped_sdus={}, num_pdus={}, num_pdu_bytes={}",
                     m.num_sdus,
                     m.num_sdu_bytes,
                     m.num_dropped_sdus,
                     m.num_pdus,
                     m.num_pdu_bytes);
  }
};

// RLC RX metrics formatter
template <>
struct formatter<srsgnb::rlc_bearer_rx_metrics_container> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_bearer_rx_metrics_container m, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "num_pdus={}, num_pdu_bytes={}, num_dropped_pdus={}, num_sdus={}, num_sdu_bytes={}",
                     m.num_pdus,
                     m.num_pdu_bytes,
                     m.num_lost_pdus,
                     m.num_malformed_pdus,
                     m.num_sdus,
                     m.num_sdu_bytes);
  }
};
} // namespace fmt
