/// \file
/// \brief Cyclic Prefix types and related slot size.

#ifndef SRSGNB_PHY_CYCLIC_PREFIX_H
#define SRSGNB_PHY_CYCLIC_PREFIX_H

#include <algorithm>

namespace srsgnb {

/// Cyclic Prefix types, as described in TS 38.211.
enum class cyclic_prefix { NORMAL = 0, EXTENDED };

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
