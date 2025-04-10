/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_block.h"

namespace srsran {

/// Struct to express a bitset of CRBs within a carrier.
struct crb_bitmap : bounded_bitset<MAX_NOF_PRBS> {
  using bounded_bitset::bounded_bitset;

  explicit crb_bitmap(const bounded_bitset& bitset) : bounded_bitset(bitset) {}

  crb_bitmap operator~() const noexcept
  {
    crb_bitmap ret(*this);
    ret.flip();
    return ret;
  }
};

inline crb_bitmap& operator|=(crb_bitmap& crb_bits, const crb_interval& grant)
{
  crb_bits.fill(grant.start(), grant.stop());
  return crb_bits;
}

/// Struct to express a bitset of PRBs within a BWP.
struct prb_bitmap : bounded_bitset<MAX_NOF_PRBS> {
  using bounded_bitset::bounded_bitset;

  explicit prb_bitmap(const bounded_bitset& bitset) : bounded_bitset(bitset) {}

  prb_bitmap operator~() const noexcept
  {
    prb_bitmap ret(*this);
    ret.flip();
    return ret;
  }
};

inline prb_bitmap& operator|=(prb_bitmap& prb_bits, const prb_interval& grant)
{
  prb_bits.fill(grant.start(), grant.stop());
  return prb_bits;
}

/// Struct to express a bitset of VRBs.
struct vrb_bitmap : bounded_bitset<MAX_NOF_PRBS> {
  using bounded_bitset::bounded_bitset;

  explicit vrb_bitmap(const bounded_bitset& bitset) : bounded_bitset(bitset) {}

  vrb_bitmap operator~() const noexcept
  {
    vrb_bitmap ret(*this);
    ret.flip();
    return ret;
  }
};

inline vrb_bitmap& operator|=(vrb_bitmap& vrb_bits, const vrb_interval& grant)
{
  vrb_bits.fill(grant.start(), grant.stop());
  return vrb_bits;
}

inline crb_bitmap prb_to_crb(const crb_interval bwp_crb_lims, const prb_bitmap& prb_bits)
{
  // TODO.
  return static_cast<crb_bitmap>(prb_bits);
}

} // namespace srsran

namespace fmt {

/// FMT formatter for crb_bitmaps.
template <>
struct formatter<srsran::crb_bitmap> : public formatter<srsran::bounded_bitset<srsran::MAX_NOF_PRBS>> {};

/// FMT formatter for prb_bitmaps.
template <>
struct formatter<srsran::prb_bitmap> : public formatter<srsran::bounded_bitset<srsran::MAX_NOF_PRBS>> {};

/// FMT formatter for vrb_bitmaps.
template <>
struct formatter<srsran::vrb_bitmap> : public formatter<srsran::bounded_bitset<srsran::MAX_NOF_PRBS>> {};

} // namespace fmt
