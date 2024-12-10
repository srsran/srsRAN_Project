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

#include "result/pucch_format.h"
#include "fmt/format.h"

namespace srsran {

/// Contains the number of UCI HARQ-ACK and CSI information bits of a removed PUCCH grant.
struct pucch_uci_bits {
  /// Number of HARQ-ACK info bits that should have been reported in the removed PUCCH grant.
  unsigned harq_ack_nof_bits{0};
  /// Number of SR info bits that should have been reported in the removed PUCCH grant.
  sr_nof_bits sr_bits{sr_nof_bits::no_sr};
  /// Number of CSI Part 1 info bits that should have been reported in the removed PUCCH grant.
  unsigned csi_part1_nof_bits{0};
  // TODO: add extra bits for CSI Part 2.

  [[nodiscard]] unsigned get_total_bits() const
  {
    return harq_ack_nof_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_nof_bits;
  }
};

} // namespace srsran

// Formatters.
namespace fmt {

template <>
struct formatter<srsran::pucch_uci_bits> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_uci_bits& uci_bits, FormatContext& ctx)
  {
    return format_to(ctx.out(),
                     "csi_bits=[H={} S={} C={}]",
                     uci_bits.harq_ack_nof_bits,
                     uci_bits.sr_bits,
                     uci_bits.csi_part1_nof_bits);
  }
};

} // namespace fmt
