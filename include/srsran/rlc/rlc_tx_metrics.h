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

#include "srsran/rlc/rlc_config.h"
#include "srsran/support/engineering_notation.h"
#include "srsran/support/format_utils.h"
#include "fmt/format.h"

namespace srsran {

struct rlc_tx_metrics_higher {
  uint32_t num_sdus;             ///< Number of SDUs
  size_t   num_sdu_bytes;        ///< Number of SDU bytes
  uint32_t num_dropped_sdus;     ///< Number of dropped SDUs (due to full queue)
  uint32_t num_discarded_sdus;   ///< Number of discarded SDUs (instructed from higher layer)
  uint32_t num_discard_failures; ///< Number of failed SDU discards (instructed from higher layer)

  uint32_t counter = 0; ///< Counter of amount of times we collected metrics.
                        ///  Useful to aggregate high and low metrics.

  rlc_tx_metrics_higher get()
  {
    rlc_tx_metrics_higher ret = *this;
    counter++;
    return ret;
  }

  void reset()
  {
    num_sdus             = {};
    num_sdu_bytes        = {};
    num_dropped_sdus     = {};
    num_discarded_sdus   = {};
    num_discard_failures = {};
    // do not reset counter
  }
};

struct rlc_tm_tx_metrics_lower {
  uint32_t num_small_allocs; ///< Number of allocations that are too small to TX PDU

  void reset() { num_small_allocs = {}; }
};

struct rlc_um_tx_metrics_lower {
  uint32_t num_pdus_with_segmentation;      ///< Number of transmitted PDUs with segmentation
  uint32_t num_pdu_bytes_with_segmentation; ///< Number of transmitted PDU bytes with segmentation

  void reset()
  {
    num_pdus_with_segmentation      = {};
    num_pdu_bytes_with_segmentation = {};
  }
};

struct rlc_am_tx_metrics_lower {
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

struct rlc_tx_metrics_lower {
  uint32_t num_pdus_no_segmentation;      ///< Number of transmitted PDUs without segmentation
  size_t   num_pdu_bytes_no_segmentation; ///< Number of transmitted PDU bytes without segmentation
  uint32_t num_of_pulled_sdus;            ///< Number of pulled SDUs
  uint32_t sum_sdu_latency_us;            ///< total SDU latency (in us)>

  /// RLC mode of the entity
  rlc_mode mode;

  /// Mode-specific metrics
  ///
  /// The associated union member is indicated by \c mode.
  /// Contents of the other fields are undefined.
  union {
    rlc_tm_tx_metrics_lower tm;
    rlc_um_tx_metrics_lower um;
    rlc_am_tx_metrics_lower am;
  } mode_specific;

  uint32_t counter = 0; ///< Counter of amount of times we collected metrics.
                        ///  Useful to aggregate high and low metrics.

  rlc_tx_metrics_lower get()
  {
    rlc_tx_metrics_lower ret = *this;
    counter++;
    return ret;
  }
  void reset()
  {
    num_pdus_no_segmentation      = {};
    num_pdu_bytes_no_segmentation = {};
    num_of_pulled_sdus            = {};
    sum_sdu_latency_us            = {};

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

struct rlc_tx_metrics {
  rlc_tx_metrics_higher tx_high;
  rlc_tx_metrics_lower  tx_low;
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

  virtual rlc_tx_metrics get_metrics()           = 0;
  virtual rlc_tx_metrics get_and_reset_metrics() = 0;
  virtual void           reset_metrics()         = 0;
};

inline std::string format_rlc_tx_metrics(timer_duration metrics_period, const rlc_tx_metrics& m)
{
  fmt::memory_buffer buffer;
  fmt::format_to(
      buffer,
      "num_sdus={} sdu_rate={}bps dropped_sdus={} discarded_sdus={} "
      "num_pdus_no_segm={} pdu_rate_no_segm={}bps",
      scaled_fmt_integer(m.tx_high.num_sdus, false),
      float_to_eng_string(static_cast<float>(m.tx_high.num_sdu_bytes) * 8 * 1000 / (metrics_period.count()), 1, false),
      scaled_fmt_integer(m.tx_high.num_dropped_sdus, false),
      scaled_fmt_integer(m.tx_high.num_discarded_sdus, false),
      scaled_fmt_integer(m.tx_low.num_pdus_no_segmentation, false),
      float_to_eng_string(
          static_cast<float>(m.tx_low.num_pdu_bytes_no_segmentation) * 8 * 1000 / metrics_period.count(), 1, false));

  if (m.tx_low.mode == rlc_mode::tm) {
    // No TM specific metrics for RX
  } else if ((m.tx_low.mode == rlc_mode::um_bidir || m.tx_low.mode == rlc_mode::um_unidir_dl)) {
    fmt::format_to(buffer,
                   " num_pdus_with_segm={} pdu_with_segm_rate={}bps",
                   m.tx_low.mode_specific.um.num_pdus_with_segmentation,
                   static_cast<float>(m.tx_low.mode_specific.um.num_pdu_bytes_with_segmentation) * 8 /
                       metrics_period.count());
  } else if (m.tx_low.mode == rlc_mode::am) {
    fmt::format_to(buffer,
                   " num_pdus_with_segm={} pdu_rate_with_segm={}bps num_retx={} "
                   "retx_rate={}bps ctrl_pdus={} ctrl_rate={}bps",
                   scaled_fmt_integer(m.tx_low.mode_specific.am.num_pdus_with_segmentation, false),
                   float_to_eng_string(static_cast<float>(m.tx_low.mode_specific.am.num_pdu_bytes_with_segmentation) *
                                           8 * 1000 / metrics_period.count(),
                                       1,
                                       false),
                   scaled_fmt_integer(m.tx_low.mode_specific.am.num_retx_pdus, false),
                   float_to_eng_string(static_cast<float>(m.tx_low.mode_specific.am.num_retx_pdu_bytes) * 8 * 1000 /
                                           metrics_period.count(),
                                       1,
                                       false),
                   scaled_fmt_integer(m.tx_low.mode_specific.am.num_ctrl_pdus, false),
                   float_to_eng_string(static_cast<float>(m.tx_low.mode_specific.am.num_ctrl_pdu_bytes) * 8 * 1000 /
                                           (double)metrics_period.count(),
                                       1,
                                       false));
  }
  return to_c_str(buffer);
}
} // namespace srsran

namespace fmt {

// RLC TX metrics formatter
// TODO print mode-specific metrics
template <>
struct formatter<srsran::rlc_tx_metrics> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_tx_metrics m, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "num_sdus={} num_sdu_bytes={} num_dropped_sdus={} num_discarded_sdus={} "
                     "num_discard_failures={} num_pdus_no_segm={} num_pdu_bytes_no_segm={}",
                     m.tx_high.num_sdus,
                     m.tx_high.num_sdu_bytes,
                     m.tx_high.num_dropped_sdus,
                     m.tx_high.num_discarded_sdus,
                     m.tx_high.num_discard_failures,
                     m.tx_low.num_pdus_no_segmentation,
                     m.tx_low.num_pdu_bytes_no_segmentation);
  }
};
} // namespace fmt
