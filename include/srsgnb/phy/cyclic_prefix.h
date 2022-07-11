/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Cyclic Prefix types and related slot size.

#pragma once

#include "srsgnb/support/math_utils.h"
#include <algorithm>

namespace srsgnb {

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

  /// Construct from another cyclic prefix type.
  constexpr cyclic_prefix(const cyclic_prefix& other) : value(other.value) {}

  /// Comparison to value.
  constexpr bool operator==(options opt) const { return value == opt; }

  /// Comparison to other instance.
  constexpr bool operator==(const cyclic_prefix& other) const { return value == other.value; }

  /// Get the string value.
  std::string to_string() const
  {
    if (value == NORMAL) {
      return "normal";
    }
    return "extended";
  }

  /// \brief Checks the validity of the DFT size for the current cyclic prefix.
  /// \param[in] numerology Indicates the subcarrier spacing numerology.
  /// \param[in] dft_size Indicates the DFT size.
  /// \return True if the given cyclic prefix type, numerology and DFT size combination results in a valid cyclic prefix
  /// length. Otherwise, false.
  bool is_valid(unsigned numerology, unsigned dft_size)
  {
    unsigned N_ref = 2048;
    unsigned kappa = 64;

    // Calculate cyclic prefix length.
    unsigned cp_len = 144 * kappa;
    if (value == EXTENDED) {
      cp_len = (512 * kappa);
    }

    // The cyclic prefix lengths must be divisible by the reference DFT size.
    return ((cp_len * dft_size) % (N_ref * kappa) == 0) && ((16 * dft_size * pow2(numerology)) % (N_ref) == 0);
  }

  /// \brief Calculates the cyclic prefix length in samples as per TS38.211 section 5.3.1.
  /// \param[in] symbol_idx Symbol index within the subframe.
  /// \param[in] numerology Indicates the subcarrier spacing numerology.
  /// \param[in] dft_size Indicates the DFT size.
  /// \return The number of samples comprising the cyclic prefix.
  constexpr unsigned get_length(unsigned symbol_idx, unsigned numerology, unsigned dft_size) const
  {
    unsigned N_ref = 2048;
    unsigned kappa = 64;

    unsigned cp_len = 144 * kappa;
    if (value == EXTENDED) {
      cp_len = 512 * kappa;
    } else if (symbol_idx == 0 || symbol_idx == 7 * pow2(numerology)) {
      cp_len += 16 * kappa * pow2(numerology);
    }

    return (cp_len * dft_size) / (N_ref * kappa);
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

} // namespace srsgnb

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsgnb::cyclic_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsgnb::cyclic_prefix cp, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", cp.to_string());
  }
};
} // namespace fmt
