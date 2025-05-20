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

#include "srsran/ran/slot_point.h"

namespace srsran {

/// Slot point type that represents the numerology, HFN, SFN and slot index of the current slot.
class slot_point_extended : private slot_point
{
  /// Number of possible numerologies.
  static constexpr uint32_t NOF_NUMEROLOGIES = 5;

public:
  constexpr slot_point_extended() = default;
  constexpr slot_point_extended(subcarrier_spacing scs, uint32_t count)
  {
    uint32_t numerology = to_numerology_value(scs);
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", numerology);
    numerology_val = numerology;
    srsran_assert(count < NOF_HFNS * NOF_SFNS * get_nof_slots_per_subframe(scs), "Invalid slot count={} passed", count);
    count_val = count;
  }
  constexpr slot_point_extended(slot_point slot)
  {
    numerology_val = slot.numerology();
    count_val      = slot.count();
  }
  constexpr slot_point_extended(slot_point slot, uint32_t hfn)
  {
    srsran_assert(hfn < NOF_HFNS, "Invalid HFN={} passed", hfn);
    numerology_val = slot.numerology();
    count_val      = slot.count() + hfn * NOF_SFNS * NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(slot.scs());
  }
  constexpr slot_point_extended(subcarrier_spacing scs, uint32_t hfn, uint32_t sfn, uint32_t slot_frame_index) :
    slot_point_extended(scs,
                        (hfn * NOF_SFNS + sfn) * NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(scs) +
                            slot_frame_index)
  {
  }

  using slot_point::clear;
  using slot_point::count;
  using slot_point::nof_slots_per_frame;
  using slot_point::nof_slots_per_hyper_frame;
  using slot_point::nof_slots_per_subframe;
  using slot_point::numerology;
  using slot_point::scs;
  using slot_point::sfn;
  using slot_point::slot_index;
  using slot_point::subframe_index;
  using slot_point::system_slot;
  using slot_point::to_uint;
  using slot_point::valid;
  using slot_point::operator uint32_t;
  using slot_point::hrf_slot_index;
  using slot_point::is_odd_hrf;
  using slot_point::nof_hrf_slots;
  using slot_point::subframe_slot_index;

  uint32_t hfn() const { return count() / nof_slots_per_hyper_frame(); }

  /// The number of slots represented by all HFNs.
  constexpr uint32_t nof_slots_in_all_hfns() const { return nof_slots_per_frame() * NOF_SFNS * NOF_HFNS; }

  constexpr slot_point without_hfn() const
  {
    return valid() ? slot_point{this->numerology_val, this->count_val % nof_slots_per_hyper_frame()} : slot_point{};
  }

  constexpr bool operator==(const slot_point_extended& other) const
  {
    return other.count_val == count_val and other.numerology_val == numerology_val;
  }
  constexpr bool operator!=(const slot_point_extended& other) const { return not(*this == other); }
  constexpr bool operator<(slot_point_extended other) const
  {
    srsran_assert(numerology() == other.numerology(), "Comparing slots of different numerologies");
    int v = static_cast<int>(other.count_val) - static_cast<int>(count_val);
    if (v > 0) {
      return (v < static_cast<int>(nof_slots_in_all_hfns() / 2));
    }
    return (v < -static_cast<int>(nof_slots_in_all_hfns() / 2));
  }
  constexpr bool operator<=(slot_point_extended other) const { return (*this == other) or (*this < other); }
  constexpr bool operator>=(slot_point_extended other) const { return not(*this < other); }
  constexpr bool operator>(slot_point_extended other) const { return (*this != other) and *this >= other; }

  constexpr slot_difference operator-(const slot_point_extended& other) const
  {
    int      a = static_cast<int>(count_val) - static_cast<int>(other.count_val);
    uint32_t N = nof_slots_in_all_hfns();
    if (a >= static_cast<int>(N / 2)) {
      return a - N;
    }
    if (a < -static_cast<int>(N / 2)) {
      return a + N;
    }
    return a;
  }

  slot_point_extended& operator++()
  {
    count_val++;
    if (count_val == nof_slots_in_all_hfns()) {
      count_val = 0;
    }
    return *this;
  }
  slot_point_extended operator++(int)
  {
    slot_point_extended ret{*this};
    this->operator++();
    return ret;
  }
  template <typename Unsigned, std::enable_if_t<std::is_unsigned_v<Unsigned>, int> = 0>
  slot_point_extended& operator+=(Unsigned jump)
  {
    count_val = (count_val + jump) % nof_slots_in_all_hfns();
    return *this;
  }
  slot_point_extended& operator+=(int jump)
  {
    const int nof_slots = static_cast<int>(nof_slots_in_all_hfns());
    int       tmp       = (static_cast<int>(count_val) + jump) % nof_slots;
    count_val           = tmp + (tmp < 0 ? nof_slots : 0);
    return *this;
  }

  slot_point_extended& operator-=(uint32_t jump) { return this->operator+=(-static_cast<int>(jump)); }
  slot_point_extended& operator-=(int jump) { return this->operator+=(-jump); }

  constexpr bool is_in_interval(slot_point_extended begin, slot_point_extended end) const
  {
    return (*this >= begin and *this < end);
  }
};

template <typename Integer>
inline slot_point_extended operator+(slot_point_extended slot, Integer jump)
{
  static_assert(std::is_integral<Integer>::value, "Invalid integer type");
  slot += jump;
  return slot;
}
inline slot_point_extended operator+(uint32_t jump, slot_point_extended slot)
{
  slot += jump;
  return slot;
}
inline slot_point_extended operator+(int jump, slot_point_extended slot)
{
  slot += jump;
  return slot;
}
inline slot_point_extended operator-(slot_point_extended slot, uint32_t jump)
{
  slot -= jump;
  return slot;
}
inline slot_point_extended operator-(slot_point_extended slot, int jump)
{
  slot -= jump;
  return slot;
}

inline slot_point_extended max(slot_point_extended lhs, slot_point_extended rhs)
{
  return lhs > rhs ? lhs : rhs;
}

inline slot_point_extended min(slot_point_extended lhs, slot_point_extended rhs)
{
  return lhs < rhs ? lhs : rhs;
}

} // namespace srsran
