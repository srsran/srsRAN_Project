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

#include "srsran/rlc/rlc_config.h"
#include "srsran/support/engineering_notation.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "fmt/format.h"

namespace srsran {

struct rlc_tm_rx_metrics {
  void reset() {}
};

struct rlc_um_rx_metrics {
  uint32_t num_sdu_segments;      ///< Number of SDU segments RX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments Bytes
  void     reset()
  {
    num_sdu_segments      = {};
    num_sdu_segment_bytes = {};
  }
};

struct rlc_am_rx_metrics {
  uint32_t num_sdu_segments;      ///< Number of SDU segments RX'ed
  uint32_t num_sdu_segment_bytes; ///< Number of SDU segments bytes
  uint32_t num_ctrl_pdus;         ///< Number of control PDUs
  uint32_t num_ctrl_pdu_bytes;    ///< Number of control PDUs bytes
  void     reset()
  {
    num_sdu_segments      = {};
    num_sdu_segment_bytes = {};
    num_ctrl_pdus         = {};
    num_ctrl_pdu_bytes    = {};
  }
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
  uint32_t sdu_latency_us;     ///< total SDU latency (in us)>

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

  uint32_t counter = 0; ///< Counter of amount of times we collected metrics.
                        ///  Useful to aggregate high and low metrics.

  rlc_rx_metrics get()
  {
    rlc_rx_metrics ret = *this;
    counter++;
    return ret;
  }

  void reset()
  {
    // SDU metrics
    num_sdus           = {};
    num_sdu_bytes      = {};
    num_pdus           = {};
    num_pdu_bytes      = {};
    num_lost_pdus      = {};
    num_malformed_pdus = {};
    sdu_latency_us     = {};

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
    // do not reset mode or counter
  }
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

inline void format_rlc_rx_metrics(fmt::memory_buffer& buffer, timer_duration metrics_period, const rlc_rx_metrics& m)
{
  fmt::format_to(
      std::back_inserter(buffer),
      "num_sdus={} sdu_rate={}bps num_pdus={} pdu_rate={}bps",
      scaled_fmt_integer(m.num_sdus, false),
      float_to_eng_string(static_cast<float>(m.num_sdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
      scaled_fmt_integer(m.num_pdus, false),
      float_to_eng_string(static_cast<double>(m.num_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false));

  // No TM specific metrics for RX
  if ((m.mode == rlc_mode::um_bidir || m.mode == rlc_mode::um_unidir_ul)) {
    // Format UM specific metrics for RX
    fmt::format_to(std::back_inserter(buffer),
                   " num_sdu_segments={} sdu_segmments_rate={}bps",
                   scaled_fmt_integer(m.mode_specific.um.num_sdu_segments, false),
                   float_to_eng_string(static_cast<float>(m.mode_specific.um.num_sdu_segment_bytes) * 8 * 1000 /
                                           metrics_period.count(),
                                       1,
                                       false));
  } else if (m.mode == rlc_mode::am) {
    fmt::format_to(
        std::back_inserter(buffer),
        " num_sdu_segments={} sdu_segmments_rate={}bps ctrl_pdus={} ctrl_rate={}bps",
        scaled_fmt_integer(m.mode_specific.am.num_sdu_segments, false),
        float_to_eng_string(
            static_cast<float>(m.mode_specific.am.num_sdu_segment_bytes) * 8 * 1000 / metrics_period.count(), 1, false),
        scaled_fmt_integer(m.mode_specific.am.num_ctrl_pdus, false),
        float_to_eng_string(
            static_cast<float>(m.mode_specific.am.num_ctrl_pdu_bytes) * 8 * 1000 / metrics_period.count(), 1, false));
  }
}

} // namespace srsran

namespace fmt {

// RLC RX metrics formatter
// TODO print mode-specific metrics
template <>
struct formatter<srsran::rlc_rx_metrics> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::rlc_rx_metrics& m, FormatContext& ctx) const
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
