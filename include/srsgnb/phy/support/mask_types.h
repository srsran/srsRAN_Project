/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/cyclic_prefix.h"

namespace srsgnb {

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

} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for symbol_slot_mask.
template <>
struct formatter<srsgnb::symbol_slot_mask> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::symbol_slot_mask& mask, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    fmt::format_to(ctx.out(), "{:br}", static_cast<srsgnb::bounded_bitset<srsgnb::MAX_NSYMB_PER_SLOT>>(mask));
    return ctx.out();
  }
};

/// \brief Custom formatter for re_prb_mask.
template <>
struct formatter<srsgnb::re_prb_mask> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::re_prb_mask& mask, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    fmt::format_to(ctx.out(), "{:br}", static_cast<srsgnb::bounded_bitset<srsgnb::NRE>>(mask));
    return ctx.out();
  }
};

} // namespace fmt
