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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {

/// \brief Represents a symbol mask within a slot.
///
/// Inherits from \c bounded_bitset<MAX_NSYMB_PER_SLOT>.
class symbol_slot_mask : public bounded_bitset<MAX_NSYMB_PER_SLOT>
{
public:
  /// Default constructor - It creates a mask containing \c MAX_NSYMB_PER_SLOT bits.
  constexpr symbol_slot_mask(unsigned nsymb = MAX_NSYMB_PER_SLOT) : bounded_bitset(nsymb) {}

  /// Copy constructor from another \c bounded_bitset<MAX_NSYMB_PER_SLOT>.
  symbol_slot_mask(const bounded_bitset& other) : bounded_bitset(other) {}

  /// Constructor from an initializer list.
  symbol_slot_mask(const std::initializer_list<const bool>& list) : bounded_bitset(list) {}
};

/// \brief Represents a RE mask within a PRB.
///
/// Inherits from \c bounded_bitset<NRE>.
class re_prb_mask : public bounded_bitset<NRE>
{
public:
  /// Default constructor - It creates a mask containing \c NRE bits.
  constexpr re_prb_mask() : bounded_bitset(NRE) {}

  /// Copy constructor from another \c bounded_bitset<NRE>.
  re_prb_mask(const bounded_bitset& other) : bounded_bitset(other) {}

  /// Constructor from an initializer list.
  re_prb_mask(const std::initializer_list<const bool>& list) : bounded_bitset(list) {}
};

} // namespace srsran

namespace fmt {

/// \brief Custom formatter for symbol_slot_mask.
template <>
struct formatter<srsran::symbol_slot_mask> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::symbol_slot_mask& mask, FormatContext& ctx) const
  {
    fmt::format_to(ctx.out(), "{:br}", static_cast<srsran::bounded_bitset<srsran::MAX_NSYMB_PER_SLOT>>(mask));
    return ctx.out();
  }
};

/// \brief Custom formatter for re_prb_mask.
template <>
struct formatter<srsran::re_prb_mask> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::re_prb_mask& mask, FormatContext& ctx) const
  {
    fmt::format_to(ctx.out(), "{:br}", static_cast<srsran::bounded_bitset<srsran::NRE>>(mask));
    return ctx.out();
  }
};

} // namespace fmt
