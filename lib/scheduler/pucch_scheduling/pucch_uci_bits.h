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

#include "srsran/scheduler/result/pucch_format.h"
#include "fmt/format.h"

namespace srsran {

/// Contains the number of UCI bits (HARQ-ACK, SR and CSI) of a PUCCH grant.
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
  auto format(const srsran::pucch_uci_bits& uci_bits, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "uci_bits=[H={} S={} C={}]",
                     uci_bits.harq_ack_nof_bits,
                     fmt::underlying(uci_bits.sr_bits),
                     uci_bits.csi_part1_nof_bits);
  }
};

} // namespace fmt
