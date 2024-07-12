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

#include "srsran/adt/expected.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>
#include <cstdlib>
#include <string>

namespace srsran {

/// \brief 36-bit identifying an NR Cell Id as specified in subclause 9.3.1.7 of 3GPP TS 38.413.
/// \remark The leftmost (22-32) bits of the NR Cell Identity correspond to the gNB ID and remaining (4-14) bits for
/// Sector ID.
class nr_cell_identity
{
  constexpr nr_cell_identity(uint64_t val_) : val(val_) {}

public:
  static constexpr nr_cell_identity min() { return nr_cell_identity{0x0}; }
  static constexpr nr_cell_identity max() { return nr_cell_identity{((uint64_t)1U << 36U) - 1U}; }

  nr_cell_identity() : val(0) {}

  static expected<nr_cell_identity> create(uint64_t val)
  {
    if (val > max().val) {
      return make_unexpected(default_error_t{});
    }
    return nr_cell_identity{val};
  }

  static expected<nr_cell_identity> create(gnb_id_t gnb_id, uint16_t sector_id)
  {
    if (gnb_id.bit_length < 22 or gnb_id.bit_length > 32) {
      // invalid bit length.
      return make_unexpected(default_error_t{});
    }
    if (sector_id >= (1U << (36U - gnb_id.bit_length))) {
      // invalid sector id.
      return make_unexpected(default_error_t{});
    }
    return nr_cell_identity{(uint64_t)gnb_id.id << (36U - gnb_id.bit_length) | sector_id};
  }

  static expected<nr_cell_identity> parse_hex(const std::string& hex_str)
  {
    const unsigned digits = nof_bits() / 4;
    if (hex_str.size() > digits) {
      return make_unexpected(default_error_t{});
    }
    char*    p;
    uint64_t n = std::strtoul(hex_str.c_str(), &p, 16);
    if (*p != 0) {
      return make_unexpected(default_error_t{});
    }
    return nr_cell_identity{n};
  }

  uint64_t value() const { return val; }

  /// Extract Sector ID from NR Cell Identity.
  uint16_t sector_id(unsigned nof_sector_id_bits) const
  {
    srsran_assert(nof_sector_id_bits >= 4 and nof_sector_id_bits <= 14, "Invalid number of Sector Id bits");
    return val & ((1U << nof_sector_id_bits) - 1U);
  }

  /// Extract gNB-DU ID from NR Cell Identity.
  gnb_id_t gnb_id(unsigned nof_gnb_id_bits) const
  {
    srsran_assert(nof_gnb_id_bits >= 22 and nof_gnb_id_bits <= 32, "Invalid number of gNB-DU ID bits");
    return gnb_id_t{static_cast<uint32_t>(val >> (36U - nof_gnb_id_bits)), static_cast<uint8_t>(nof_gnb_id_bits)};
  }

  static size_t nof_bits() { return 36; }

  bool operator==(const nr_cell_identity& nci) const { return val == nci.val; }
  bool operator!=(const nr_cell_identity& nci) const { return !(*this == nci); }
  bool operator<(const nr_cell_identity& nci) const { return val < nci.val; }
  bool operator<=(const nr_cell_identity& nci) const { return val <= nci.val; }
  bool operator>(const nr_cell_identity& nci) const { return val > nci.val; }
  bool operator>=(const nr_cell_identity& nci) const { return val >= nci.val; }

private:
  uint64_t val;
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::nr_cell_identity> : formatter<uint64_t> {
  template <typename FormatContext>
  auto format(const srsran::nr_cell_identity& val, FormatContext& ctx)
  {
    return formatter<uint64_t>::format(val.value(), ctx);
  }
};

} // namespace fmt