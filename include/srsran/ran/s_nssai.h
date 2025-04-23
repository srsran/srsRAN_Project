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

#include "srsran/adt/expected.h"
#include "fmt/format.h"
#include <cstdint>
#include <optional>

namespace srsran {

/// Slice/Service Type (SST), as per TS 23.003, 28.4. When set, it takes at most 8 bits.
class slice_service_type
{
public:
  constexpr slice_service_type() = default;
  constexpr explicit slice_service_type(uint8_t val_) : val(val_) {}

  /// Determines whether the SST is within the range of standardized SSTs, as per TS 23.501.
  bool is_standardized() const { return val < 128; }
  bool is_operator_specific() const { return not is_standardized(); }

  uint8_t value() const { return val; }

  bool operator==(const slice_service_type& rhs) const { return val == rhs.val; }
  bool operator!=(const slice_service_type& rhs) const { return not(val != rhs.val); }
  bool operator<(const slice_service_type& rhs) const { return val < rhs.val; }

private:
  uint8_t val = 0;
};

/// Slice Differentiator (SD) type, as per TS 23.003, 28.4. When set, it takes at most 24 bits.
class slice_differentiator
{
  static constexpr uint32_t default_val = 0xffffffU;

public:
  constexpr slice_differentiator() = default;

  bool is_default() const { return val == default_val; }
  bool is_set() const { return val != default_val; }

  uint32_t value() const { return val; }

  bool operator==(const slice_differentiator& rhs) const { return val == rhs.val; }
  bool operator!=(const slice_differentiator& rhs) const { return not(val != rhs.val); }
  bool operator<(const slice_differentiator& rhs) const
  {
    if (val == default_val && rhs.val != default_val) {
      return true;
    } else if (val != default_val && rhs.val == default_val) {
      return false;
    } else {
      return val < rhs.val;
    }
  }

  static expected<slice_differentiator> create(uint32_t new_val)
  {
    if (new_val > default_val) {
      return make_unexpected(default_error_t{});
    }
    slice_differentiator sd;
    sd.val = new_val;
    return sd;
  }

private:
  uint32_t val = default_val;
};

/// Network Slice Selection Assistance Information, as per TS 23.003 and TS 23.501.
struct s_nssai_t {
  /// Slice/Service type.
  slice_service_type sst;
  /// Slice Differentiator (SD).
  slice_differentiator sd;

  bool operator==(const s_nssai_t& other) const { return sst == other.sst && sd == other.sd; }
  bool operator!=(const s_nssai_t& other) const { return not(*this == other); }
  bool operator<(const s_nssai_t& other) const { return sst < other.sst || (sst == other.sst && sd < other.sd); }
};

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::s_nssai_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::s_nssai_t s_nssai, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "sst: {}{}",
                     s_nssai.sst.value(),
                     s_nssai.sd.is_set() ? fmt::format(", sd: {:#x}", s_nssai.sd.value()) : "");
  }
};

} // namespace fmt
