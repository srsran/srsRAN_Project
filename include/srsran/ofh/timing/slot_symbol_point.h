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

  /// Takes a numerology, total count value and number of symbols.
  slot_symbol_point(uint32_t numerology_, uint32_t count_, unsigned nof_symbols_) :
    nof_symbols(nof_symbols_), numerology(numerology_), count_val(count_)
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", unsigned(numerology));
  }

  /// Slot point.
  slot_point get_slot() const
  {
    return numerology < NOF_NUMEROLOGIES ? slot_point(numerology, count_val / nof_symbols) : slot_point();
  }

  /// Symbol index in a slot. Value: (0..nof_symbols-1).
  unsigned get_symbol_index() const { return count_val % nof_symbols; }

  /// Number of symbols per slot.
  unsigned get_nof_symbols() const { return nof_symbols; }

  /// Numerology index (0..4).
  unsigned get_numerology() const { return numerology; }

  /// System slot.
  uint32_t system_slot() const { return count_val; }

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

  /// Equality comparison of two slot_symbol_point objects. Two slot symbol points are equal if their numerology, SFN
  /// slot index, symbol index and number of symbols have the same value.
  bool operator==(const slot_symbol_point& other) const
  {
    return other.count_val == count_val && other.numerology == numerology && other.nof_symbols == nof_symbols;
  }

  /// Inequality comparison of two slot_symbol_point objects.
  bool operator!=(const slot_symbol_point& other) const { return !(*this == other); }

  /// Checks if "lhs" slot symbol point is lower than "rhs". It assumes that both "lhs" and "rhs" use the same
  /// numerology and number of symbols. This comparison accounts for the wrap-around of both slot index and SFNs. The
  /// ambiguity range of the comparison is equal to half of the total number of slot symbol points in a hyperframe.
  bool operator<(const slot_symbol_point& other) const
  {
    srsran_assert(numerology == other.numerology, "Comparing slots symbol point of different numerologies");
    srsran_assert(get_nof_symbols() == other.get_nof_symbols(),
                  "Comparing slots symbol point with different number of symbols");

    const int nof_symbols_per_slot_wrap = NOF_SFNS * NOF_SUBFRAMES_PER_FRAME *
                                          get_nof_slots_per_subframe(to_subcarrier_spacing(numerology)) * nof_symbols;

    int v = static_cast<int>(other.count_val) - static_cast<int>(count_val);
    if (v > 0) {
      return (v < nof_symbols_per_slot_wrap / 2);
    }
    return (v < -nof_symbols_per_slot_wrap / 2);
  }

  /// Other lower/higher comparisons that build on top of operator== and operator<.
  bool operator<=(const slot_symbol_point& other) const { return (*this == other) || (*this < other); }
  bool operator>=(const slot_symbol_point& other) const { return !(*this < other); }
  bool operator>(const slot_symbol_point& other) const { return (*this != other) && *this >= other; }

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
