/// \file
/// \brief Cyclic Prefix types and related slot size.

#ifndef SRSGNB_PHY_CYCLIC_PREFIX_H
#define SRSGNB_PHY_CYCLIC_PREFIX_H

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
  /// Construct from value.
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
  /// Calculates the cyclic prefix length in samples as per TS38.211 section 5.3.1.
  constexpr unsigned get_length(unsigned symbol_idx, unsigned numerology, unsigned dft_size) const
  {
    unsigned N_ref = 2048;
    unsigned kappa = 64;

    unsigned cp_len = (144 * kappa) / pow2(numerology);
    if (value == EXTENDED) {
      cp_len = (512 * kappa) / pow2(numerology);
    } else if (symbol_idx == 0 || symbol_idx == 7 * pow2(numerology)) {
      cp_len += 16 * kappa;
    }

    return (cp_len * dft_size) / N_ref;
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

#endif // SRSGNB_PHY_CYCLIC_PREFIX_H
