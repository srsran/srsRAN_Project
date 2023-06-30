/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Represents the combined concept of a \c slot_point and a symbol index inside a slot.
class slot_symbol_point
{
public:
  slot_symbol_point(slot_point slot_, unsigned symbol_index_, unsigned nof_symbols_) :
    nof_symbols(nof_symbols_), numerology(slot_.numerology()), count_val(slot_.count_val * nof_symbols_ + symbol_index_)
  {
    srsran_assert(symbol_index_ < nof_symbols_, "Invalid symbol index");
  }

  /// Slot point.
  slot_point get_slot() const { return slot_point(numerology, count_val / nof_symbols); }

  /// Symbol index in a slot. Value: (0..nof_symbols-1).
  unsigned get_symbol_index() const { return count_val % nof_symbols; }

  /// Number of symbols per slot.
  unsigned get_nof_symbols() const { return nof_symbols; }

  /// Numerology index (0..4).
  unsigned get_numerology() const { return numerology; }

  /// Implementation of the sum operator, where \c jump is represented in number of symbols.
  slot_symbol_point& operator+=(int jump)
  {
    const int nof_symbols_per_slot_wrap = NOF_SFNS * NOF_SUBFRAMES_PER_FRAME *
                                          get_nof_slots_per_subframe(to_subcarrier_spacing(numerology)) * nof_symbols;

    int tmp   = (static_cast<int>(count_val) + jump) % nof_symbols_per_slot_wrap;
    count_val = tmp + (tmp < 0 ? nof_symbols_per_slot_wrap : 0);
    return *this;
  }

  /// Implementation of the subtraction operator, where \c jump is represented in number of symbols.
  slot_symbol_point& operator-=(int jump) { return this->operator+=(-jump); }

private:
  uint32_t nof_symbols : 4;
  uint32_t numerology : 3;
  uint32_t count_val : 25;
};

inline slot_symbol_point operator+(slot_symbol_point symbol_point, int jump)
{
  symbol_point += jump;
  return symbol_point;
}

inline slot_symbol_point operator+(int jump, slot_symbol_point symbol_point)
{
  symbol_point += jump;
  return symbol_point;
}

inline slot_symbol_point operator-(slot_symbol_point symbol_point, int jump)
{
  symbol_point -= jump;
  return symbol_point;
}

inline slot_symbol_point operator-(int jump, slot_symbol_point symbol_point)
{
  symbol_point -= jump;
  return symbol_point;
}

} // namespace ofh
} // namespace srsran
