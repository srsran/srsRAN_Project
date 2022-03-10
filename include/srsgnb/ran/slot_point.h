
#ifndef SRSGNB_SLOT_POINT_H
#define SRSGNB_SLOT_POINT_H

#include "srsgnb/adt/interval.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// Number of subframers per frame. This constant doesn't depend on the numerology used
const uint32_t NOF_SUBFRAMES_PER_FRAME = 10;

/// Represents the numerology, SFN and slot index of the current slot.
/// It provides several functionalities compared to a simple integer type, namely:
/// - automatic wrap-around of SFN and slot_index on increment/decrement
/// - comparison of slot_points accounts for wrap-around. For instance slot point with SFN=1023 and slot index=9 is
///   lower than slot point with SFN=0 and slot index=0.
/// - it provides several helper methods to convert slot point to SFN, slot_index and frame number
/// - it can encode an invalid slot_point state
class slot_point
{
  /// Number of possible numerologies.
  static constexpr uint32_t NOF_NUMEROLOGIES = 5;

  /// Number of possible SFNs.
  static constexpr uint16_t NOF_SFNS = 1024;

  /// Number of slots per system frame.
  uint32_t nof_slots_per_system_frame() const { return nof_slots_per_frame() * NOF_SFNS; }

public:
  /// Default constructor. Sets slot_point in invalid state.
  constexpr slot_point() : numerology_val(NOF_NUMEROLOGIES), count_val(0) {}

  /// Takes a numerology and total count value.
  slot_point(uint32_t numerology, uint32_t count) : numerology_val(numerology), count_val(count)
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", numerology);
    srsran_assert(count < nof_slots_per_system_frame(), "Invalid slot count={} passed", count);
  }

  /// Takes a numerology, SFN and slot index.
  slot_point(uint32_t numerology, uint32_t sfn_val, uint32_t slot_idx_) :
    numerology_val(numerology), count_val(slot_idx_ + sfn_val * nof_slots_per_frame())
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", numerology);
    srsran_assert(sfn_val < NOF_SFNS, "Invalid SFN={} provided", sfn_val);
    srsran_assert(slot_idx_ < nof_slots_per_frame(),
                  "Slot index={} exceeds maximum number of slots={}",
                  slot_idx_,
                  nof_slots_per_frame());
  }

  /// Determines whether slot_point was correctly initialized.
  bool valid() const { return numerology_val < NOF_NUMEROLOGIES; }

  /// Number of slots present in a subframe.
  uint8_t nof_slots_per_subframe() const { return 1U << static_cast<uint32_t>(numerology_val); }

  /// Number of slots present in a frame.
  uint8_t nof_slots_per_frame() const { return nof_slots_per_subframe() * NOF_SUBFRAMES_PER_FRAME; }

  /// Frame number. Value: (0..1023).
  uint16_t sfn() const { return static_cast<uint32_t>(count_val) / nof_slots_per_frame(); }

  /// Subframe index in a frame. Value: (0..9).
  uint16_t subframe_idx() const { return slot_idx() / nof_slots_per_subframe(); }

  /// Slot index. Value: SCS=15kHz: (0..9), SCS=30kHz: (0..19), SCS=60kHz: (0..39), SCS=120kHz: (0..79), SCS=240kHz:
  /// (0..159)
  uint8_t slot_idx() const { return static_cast<uint32_t>(count_val) % nof_slots_per_frame(); }

  /// Numerology index (0..4).
  uint8_t numerology_idx() const { return numerology_val; }

  /// Conversion of slot_point to system slot.
  uint32_t system_slot() const { return count_val; }
  uint32_t to_uint() const { return count_val; }

  /// Cast of slot point to system slot.
  explicit operator uint32_t() const { return count_val; }

  /// Sets slot_point to invalid state.
  void clear() { numerology_val = NOF_NUMEROLOGIES; }

  /// Checks if slot is in second half of the radio frame.
  bool is_odd_half_radio_frame() const { return subframe_idx() >= (NOF_SUBFRAMES_PER_FRAME / 2); }

  /// Equality comparison of two slot_point objects. Two slot points are equal if their numerology, SFN and slot index
  /// have the same value.
  bool operator==(const slot_point& other) const
  {
    return other.count_val == count_val and other.numerology_val == numerology_val;
  }

  /// Inequality comparison of two slot_point objects
  bool operator!=(const slot_point& other) const { return not(*this == other); }

  /// Checks if "lhs" slot point is lower than "rhs". It assumes that both "lhs" and "rhs" use the same numerology
  /// This comparison accounts for the wrap-around of both slot index and SFNs. The ambiguity range of the comparison
  /// is equal to half of the total number of slots in a hyperframe.
  bool operator<(const slot_point& other) const
  {
    srsran_assert(numerology_idx() == other.numerology_idx(), "Comparing slots of different numerologies");
    int v = static_cast<int>(other.count_val) - static_cast<int>(count_val);
    if (v > 0) {
      return (v < (int)nof_slots_per_system_frame() / 2);
    }
    return (v < -(int)nof_slots_per_system_frame() / 2);
  }

  /// Other lower/higher comparisons that build on top of operator== and operator<.
  bool operator<=(const slot_point& other) const { return (*this == other) or (*this < other); }
  bool operator>=(const slot_point& other) const { return not(*this < other); }
  bool operator>(const slot_point& other) const { return (*this != other) and *this >= other; }

  /// Implements the subtraction of two slot points.
  /// @return a signed integer that is positive if "lhs" is higher than "rhs", zero if equal, and negative otherwise.
  int32_t operator-(const slot_point& other) const
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
  slot_point& operator+=(uint32_t jump)
  {
    count_val = (count_val + jump) % nof_slots_per_system_frame();
    return *this;
  }

  /// Subtraction of slot point by an integer jump value.
  slot_point& operator-=(uint32_t jump)
  {
    int a = (static_cast<int>(count_val) - static_cast<int>(jump)) % static_cast<int>(nof_slots_per_system_frame());
    if (a < 0) {
      a += nof_slots_per_system_frame();
    }
    count_val = a;
    return *this;
  }

  /// Determines whether this slot_point fits within provided slot point range.
  bool is_in_interval(slot_point begin, slot_point end) const { return (*this >= begin and *this < end); }

  /// Tag for numerology used: {0: 15 kHz, 1: 30 kHz, 2: 60 kHz, 3: 120 kHz, 4: 240 kHz}.
  uint32_t numerology_val : 3;

  /// Count value that encodes both SFN (0..1023) and slot index (0..9/19/39/79/159).
  uint32_t count_val : 29;
};

/// Sum and subtraction operations between slot_point types.
inline slot_point operator+(slot_point slot, uint32_t jump)
{
  slot += jump;
  return slot;
}
inline slot_point operator+(uint32_t jump, slot_point slot)
{
  slot += jump;
  return slot;
}
inline slot_point operator-(slot_point slot, uint32_t jump)
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

} // namespace srsgnb

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsgnb::slot_point> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsgnb::slot_point slot, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", slot.to_uint());
  }
};
} // namespace fmt

#endif // SRSGNB_SLOT_POINT_H
