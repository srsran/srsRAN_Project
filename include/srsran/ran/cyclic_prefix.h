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

/// \file
/// \brief Cyclic Prefix types and related slot size.

#pragma once

#include "srsran/ran/phy_time_unit.h"
#include "srsran/support/math_utils.h"
#include <algorithm>

namespace srsran {

/// Describes a cyclic prefix configuration with its helper methods.
class cyclic_prefix
{
public:
  /// Cyclic Prefix types, as described in TS 38.211.
  enum options {
    /// Indicates normal cyclic prefix length.
    NORMAL = 0,
    /// Indicates extended cyclic prefix length.
    EXTENDED
  };

  /// Indicates the cyclic prefix type.
  options value = NORMAL;

  /// Default constructor as normal.
  constexpr cyclic_prefix() = default;

  /// Construct from value. It allows implicit conversion from a cyclic prefix option.
  constexpr cyclic_prefix(options opt) : value(opt) {}

  /// Implicit conversion to enumerate.
  constexpr operator options() const { return value; }

  /// Get the string value.
  std::string to_string() const
  {
    if (value == NORMAL) {
      return "normal";
    }
    return "extended";
  }

  /// \brief Checks the validity of the DFT size and subcarrier spacing for the current cyclic prefix.
  ///
  /// A cyclic prefix is valid if it results in an integer number of samples. Othwerwise, it is not valid.
  ///
  /// \param[in] scs      Subcarrier spacing.
  /// \param[in] dft_size DFT size.
  /// \return True if the cyclic prefix is valid for the given SCS and DFT size.
  bool is_valid(subcarrier_spacing scs, unsigned dft_size) const
  {
    // All units are in multiples of the constant kappa.
    unsigned cp_len = 144U >> to_numerology_value(scs);
    if (value == EXTENDED) {
      cp_len = 512U >> to_numerology_value(scs);
    }

    unsigned sampling_rate_Hz = to_sampling_rate_Hz<unsigned>(scs, dft_size);

    // The cyclic prefix lengths must result in an integer number of samples.
    return phy_time_unit::from_units_of_kappa(cp_len).is_sample_accurate(sampling_rate_Hz) &&
           phy_time_unit::from_units_of_kappa(16).is_sample_accurate(sampling_rate_Hz);
  }

  /// \brief Calculates the cyclic prefix length in samples as per TS38.211 Section 5.3.1.
  /// \param[in] symbol_idx Symbol index within the subframe.
  /// \param[in] scs        Subcarrier spacing numerology.
  /// \return The cyclic prefix length in PHY time units.
  constexpr phy_time_unit get_length(unsigned symbol_idx, subcarrier_spacing scs) const
  {
    // All units are in multiples of the constant kappa.
    unsigned cp_len = 144U >> to_numerology_value(scs);
    if (value == EXTENDED) {
      cp_len = 512U >> to_numerology_value(scs);
    } else if (symbol_idx == 0 || symbol_idx == 7 * pow2(to_numerology_value(scs))) {
      cp_len += 16;
    }

    return phy_time_unit::from_units_of_kappa(cp_len);
  }
};

/// Defines the number of OFDM symbols per slot, according to TS 38.211 Tables 4.3.2-1 and 4.3.2-2.
inline constexpr unsigned get_nsymb_per_slot(const cyclic_prefix cp)
{
  if (cp == cyclic_prefix::EXTENDED) {
    return 12;
  }
  return 14;
}

/// Returns the maximum number of OFDM symbols per slot.
static constexpr unsigned MAX_NSYMB_PER_SLOT =
    std::max(get_nsymb_per_slot(cyclic_prefix::NORMAL), get_nsymb_per_slot(cyclic_prefix::EXTENDED));

} // namespace srsran

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsran::cyclic_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsran::cyclic_prefix cp, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", cp.to_string());
  }
};
} // namespace fmt
