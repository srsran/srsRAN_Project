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

#include "srsran/adt/interval.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Number of subframers per frame. This constant doesn't depend on the numerology used.
constexpr uint32_t NOF_SUBFRAMES_PER_FRAME = 10;

/// Number of system Frame Number values.
constexpr size_t NOF_SFNS = 1024;

/// Duration of a subframe in milliseconds.
constexpr uint32_t SUBFRAME_DURATION_MSEC = 1;

/// Represents the difference between two slot points.
using slot_difference = int;

/// Represents the numerology, SFN and slot index of the current slot.
/// It provides several functionalities compared to a simple integer type, namely:
/// - automatic wrap-around of SFN and slot_index on increment/decrement.
/// - comparison of slot_points accounts for wrap-around. For instance slot point with SFN=1023 and slot index=9 is
///   lower than slot point with SFN=0 and slot index=0.
/// - it provides several helper methods to convert slot point to SFN, slot_index and frame number.
/// - it can encode an invalid slot_point state.
class slot_point
{
  /// Number of possible numerologies.
  static constexpr uint32_t NOF_NUMEROLOGIES = 5;

public:
  /// Default constructor. Sets slot_point in invalid state.
  constexpr slot_point() : numerology_val(NOF_NUMEROLOGIES), count_val(0) {}

  /// Takes a numerology and total count value.
  constexpr slot_point(uint32_t numerology, uint32_t count) : numerology_val(numerology), count_val(count)
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", numerology);
    srsran_assert(count < nof_slots_per_system_frame(), "Invalid slot count={} passed", count);
  }

  /// Takes a numerology, SFN and slot index in radio frame.
  constexpr slot_point(uint32_t numerology, uint32_t sfn_val, uint32_t slot_radio_frame_idx_) :
    numerology_val(numerology), count_val(slot_radio_frame_idx_ + sfn_val * nof_slots_per_frame())
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", numerology);
    srsran_assert(sfn_val < NOF_SFNS, "Invalid SFN={} provided", sfn_val);
    srsran_assert(slot_radio_frame_idx_ < nof_slots_per_frame(),
                  "Slot index={} exceeds maximum number of slots={}",
                  slot_radio_frame_idx_,
                  nof_slots_per_frame());
  }

  /// Takes a subcarrier spacing, SFN and slot index in radio frame.
  constexpr slot_point(subcarrier_spacing scs, uint32_t sfn_val, uint32_t slot_radio_frame_idx_) :
    numerology_val(to_numerology_value(scs)), count_val(slot_radio_frame_idx_ + sfn_val * nof_slots_per_frame())
  {
    srsran_assert(
        to_numerology_value(scs) < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", to_numerology_value(scs));
    srsran_assert(sfn_val < NOF_SFNS, "Invalid SFN={} provided", sfn_val);
    srsran_assert(slot_radio_frame_idx_ < nof_slots_per_frame(),
                  "Slot index={} exceeds maximum number of slots={}",
                  slot_radio_frame_idx_,
                  nof_slots_per_frame());
  }

  /// Takes a numerology, SFN, subframe index and slot index within subframe.
  constexpr slot_point(uint32_t numerology, uint32_t sfn_val, uint32_t subframe_index_, uint32_t slot_subframe_index_) :
    numerology_val(numerology),
    count_val(slot_subframe_index_ + subframe_index_ * nof_slots_per_subframe() + sfn_val * nof_slots_per_frame())
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", numerology);
    srsran_assert(sfn_val < NOF_SFNS, "Invalid SFN={} provided", sfn_val);
    srsran_assert(subframe_index_ < NOF_SUBFRAMES_PER_FRAME, "Invalid Subframe index={} provided", subframe_index_);
    srsran_assert(slot_subframe_index_ < nof_slots_per_subframe(),
                  "Slot index={} exceeds maximum number of slots in subframe={}",
                  slot_subframe_index_,
                  nof_slots_per_subframe());
  }

  /// Sets slot_point to invalid state.
  void clear() { numerology_val = NOF_NUMEROLOGIES; }

  /// Determines whether slot_point was correctly initialized.
  constexpr bool valid() const { return numerology_val < NOF_NUMEROLOGIES; }

  /// Numerology index (0..4).
  constexpr uint32_t numerology() const { return numerology_val; }

  /// Slot index in radio frame.
  /// Value: SCS=15kHz: (0..9), SCS=30kHz: (0..19), SCS=60kHz: (0..39), SCS=120kHz: (0..79), SCS=240kHz: (0..159).
  constexpr uint32_t slot_index() const { return static_cast<uint32_t>(count_val) % nof_slots_per_frame(); }

  /// Radio Frame Number. Value: (0..1023).
  constexpr uint32_t sfn() const { return static_cast<uint32_t>(count_val) / nof_slots_per_frame(); }

  /// Number of slots per system frame.
  constexpr uint32_t nof_slots_per_system_frame() const { return nof_slots_per_frame() * NOF_SFNS; }

  /// Number of slots present in a subframe. Depends on numerology.
  constexpr uint32_t nof_slots_per_subframe() const { return 1U << static_cast<uint32_t>(numerology_val); }

  /// Number of slots present in a frame. Depends on numerology.
  constexpr uint32_t nof_slots_per_frame() const { return nof_slots_per_subframe() * NOF_SUBFRAMES_PER_FRAME; }

  /// Subframe index in a radio frame. Value: (0..9).
  constexpr uint32_t subframe_index() const { return slot_index() / nof_slots_per_subframe(); }

  /// Conversion of slot_point to system slot.
  constexpr uint32_t system_slot() const { return count_val; }
  constexpr uint32_t to_uint() const { return count_val; }
  constexpr explicit operator uint32_t() const { return count_val; }

  /// Get Half Radio Frame size in number of slots.
  constexpr uint32_t nof_hrf_slots() const { return nof_slots_per_frame() / 2; }

  /// Checks if slot is in second half of the radio frame.
  constexpr bool is_odd_hrf() const { return subframe_index() >= (NOF_SUBFRAMES_PER_FRAME / 2); }

  /// Get MOD(slot index, Nof slots in Half Frame).
  constexpr uint32_t hrf_slot_index() const { return static_cast<uint32_t>(count_val) % nof_hrf_slots(); }

  /// Get MOD(slot index, Nof slots in a subframe).
  constexpr uint32_t subframe_slot_index() const { return static_cast<uint32_t>(count_val) % nof_slots_per_subframe(); }

  /// Equality comparison of two slot_point objects. Two slot points are equal if their numerology, SFN and slot index
  /// have the same value.
  constexpr bool operator==(const slot_point& other) const
  {
    return other.count_val == count_val and other.numerology_val == numerology_val;
  }

  /// Inequality comparison of two slot_point objects.
  constexpr bool operator!=(const slot_point& other) const { return not(*this == other); }

  /// Checks if "lhs" slot point is lower than "rhs". It assumes that both "lhs" and "rhs" use the same numerology
  /// This comparison accounts for the wrap-around of both slot index and SFNs. The ambiguity range of the comparison
  /// is equal to half of the total number of slots in a hyperframe.
  constexpr bool operator<(const slot_point& other) const
  {
    srsran_assert(numerology() == other.numerology(), "Comparing slots of different numerologies");
    int v = static_cast<int>(other.count_val) - static_cast<int>(count_val);
    if (v > 0) {
      return (v < (int)nof_slots_per_system_frame() / 2);
    }
    return (v < -(int)nof_slots_per_system_frame() / 2);
  }

  /// Other lower/higher comparisons that build on top of operator== and operator<.
  constexpr bool operator<=(const slot_point& other) const { return (*this == other) or (*this < other); }
  constexpr bool operator>=(const slot_point& other) const { return not(*this < other); }
  constexpr bool operator>(const slot_point& other) const { return (*this != other) and *this >= other; }

  /// Implements the subtraction of two slot points.
  /// @return a signed integer that is positive if "lhs" is higher than "rhs", zero if equal, and negative otherwise.
  constexpr slot_difference operator-(const slot_point& other) const
  {
    int a = static_cast<int>(count_val) - static_cast<int>(other.count_val);
    if (a >= (int)nof_slots_per_system_frame() / 2) {
      return a - nof_slots_per_system_frame();
    }
    if (a < -(int)nof_slots_per_system_frame() / 2) {
      return a + nof_slots_per_system_frame();
    }
    return a;
  }

  /// Increment slot_point. It accounts for slot index and SFN wrap-around.
  slot_point& operator++()
  {
    count_val++;
    if (count_val == nof_slots_per_system_frame()) {
      count_val = 0;
    }
    return *this;
  }
  slot_point operator++(int)
  {
    slot_point ret{*this};
    this->     operator++();
    return ret;
  }

  /// Sum of slot_point by an integer jump value.
  template <typename Unsigned, std::enable_if_t<std::is_unsigned<Unsigned>::value, int> = 0>
  slot_point& operator+=(Unsigned jump)
  {
    count_val = (count_val + jump) % nof_slots_per_system_frame();
    return *this;
  }
  slot_point& operator+=(int jump)
  {
    const int nof_slots = static_cast<int>(nof_slots_per_system_frame());
    int       tmp       = (static_cast<int>(count_val) + jump) % nof_slots;
    count_val           = tmp + (tmp < 0 ? nof_slots : 0);
    return *this;
  }

  /// Subtraction of slot point by an integer jump value.
  slot_point& operator-=(uint32_t jump) { return this->operator+=(-static_cast<int>(jump)); }
  slot_point& operator-=(int jump) { return this->operator+=(-jump); }

  /// Determines whether this slot_point fits within provided slot point range.
  constexpr bool is_in_interval(slot_point begin, slot_point end) const { return (*this >= begin and *this < end); }

  /// Tag for numerology used: {0: 15 kHz, 1: 30 kHz, 2: 60 kHz, 3: 120 kHz, 4: 240 kHz}.
  uint32_t numerology_val : 3;

  /// Count value that encodes both SFN (0..1023) and slot index (0..9/19/39/79/159).
  uint32_t count_val : 29;
};

/// Sum and subtraction operations between slot_point types.
template <typename Integer>
inline slot_point operator+(slot_point slot, Integer jump)
{
  static_assert(std::is_integral<Integer>::value, "Invalid integer type");
  slot += jump;
  return slot;
}
inline slot_point operator+(uint32_t jump, slot_point slot)
{
  slot += jump;
  return slot;
}
inline slot_point operator+(int jump, slot_point slot)
{
  slot += jump;
  return slot;
}
inline slot_point operator-(slot_point slot, uint32_t jump)
{
  slot -= jump;
  return slot;
}
inline slot_point operator-(slot_point slot, int jump)
{
  slot -= jump;
  return slot;
}

/// Get maximum of two slot points.
inline slot_point max(slot_point lhs, slot_point rhs)
{
  return lhs > rhs ? lhs : rhs;
}

/// Get minimum of two slot points.
inline slot_point min(slot_point lhs, slot_point rhs)
{
  return lhs < rhs ? lhs : rhs;
}

/// Slot Interval [start, stop).
using slot_interval = interval<slot_point>;

/// Convert a slot point to another numerology. This function applies a "floor" operation when the new numerology is
/// lower than the old one. E.g. Given two numerologies mu1 and mu2, where m1 < m2, this conversion function "f" will
/// always ensure that, for a slot t1 with m1 numerology, the condition "f(f(t1, mu2), m1) <= t1" is satisfied.
inline slot_point set_slot_numerology(slot_point sl, unsigned new_numerology)
{
  unsigned old_numerology = sl.numerology();
  if (old_numerology > new_numerology) {
    return slot_point{new_numerology, sl.to_uint() >> (old_numerology - new_numerology)};
  }
  if (old_numerology < new_numerology) {
    return slot_point{new_numerology, sl.to_uint() << (new_numerology - old_numerology)};
  }
  return sl;
}

} // namespace srsran

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsran::slot_point> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsran::slot_point slot, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}.{}", slot.sfn(), slot.slot_index());
  }
};
} // namespace fmt
