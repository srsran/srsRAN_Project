
#ifndef SRSGNB_SLOT_POINT_H
#define SRSGNB_SLOT_POINT_H

#include "srsgnb/adt/interval.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

class slot_point
{
  /// Tag of numerology used: {0: 15 kHz, 1: 30 kHz, 2: 60 kHz, 3: 120 kHz, 4: 240 kHz}.
  uint32_t numerology_val : 3;
  /// Count value that encodes both SFN and slot index.
  uint32_t count_val : 29;

  static constexpr uint32_t NOF_NUMEROLOGIES        = 5;
  static constexpr uint16_t NOF_SFNS                = 1024;
  static constexpr uint8_t  NOF_SUBFRAMES_PER_FRAME = 10;

  /// Number of slots per hyperframe.
  uint32_t nof_slots_per_hf() const { return nof_slots_per_frame() * NOF_SFNS; }

public:
  slot_point() : numerology_val(NOF_NUMEROLOGIES), count_val(0) {}
  slot_point(uint8_t numerology, uint32_t count) : numerology_val(numerology), count_val(count)
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", (int)numerology);
    srsran_assert(count < nof_slots_per_hf(), "Invalid slot count={} passed", (int)count);
  }
  slot_point(uint8_t numerology, uint16_t sfn_val, uint8_t slot) :
    numerology_val(numerology), count_val(slot + sfn_val * nof_slots_per_frame())
  {
    srsran_assert(numerology < NOF_NUMEROLOGIES, "Invalid numerology idx={} passed", (int)numerology);
    srsran_assert(sfn_val < NOF_SFNS, "Invalid SFN={} provided", (int)sfn_val);
    srsran_assert(slot < nof_slots_per_frame(),
                  "Slot index={} exceeds maximum number of slots={}",
                  (int)slot,
                  (int)nof_slots_per_frame());
  }

  /// Determines whether slot_point was correctly initialized.
  bool valid() const { return numerology_val < NOF_NUMEROLOGIES; }

  /// Number of slots present in a subframe.
  uint8_t nof_slots_per_subframe() const { return 1U << numerology_val; }

  /// Number of slots present in a frame.
  uint8_t nof_slots_per_frame() const { return nof_slots_per_subframe() * NOF_SUBFRAMES_PER_FRAME; }

  /// Frame number.
  uint16_t sfn() const { return count_val / nof_slots_per_frame(); }

  /// Subframe index in a frame.
  uint16_t subframe_idx() const { return slot_idx() / nof_slots_per_subframe(); }

  /// Slot index.
  uint8_t slot_idx() const { return count_val % nof_slots_per_frame(); }

  /// Numberology index (0..4).
  uint8_t numerology_idx() const { return numerology_val; }

  /// Conversion of slot_point to integer.
  uint32_t to_uint() const { return count_val; }
  explicit operator uint32_t() const { return count_val; }

  void clear() { numerology_val = NOF_NUMEROLOGIES; }

  /// Comparison operators.
  bool operator==(const slot_point& other) const
  {
    return other.count_val == count_val and other.numerology_val == numerology_val;
  }
  bool operator!=(const slot_point& other) const { return not(*this == other); }
  bool operator<(const slot_point& other) const
  {
    srsran_assert(numerology_idx() == other.numerology_idx(), "Comparing slots of different numerologies");
    int a = static_cast<int>(other.count_val) - static_cast<int>(count_val);
    if (a > 0) {
      return (a < (int)nof_slots_per_hf() / 2);
    }
    return (a < -(int)nof_slots_per_hf() / 2);
  }
  bool operator<=(const slot_point& other) const { return (*this == other) or (*this < other); }
  bool operator>=(const slot_point& other) const { return not(*this < other); }
  bool operator>(const slot_point& other) const { return (*this != other) and *this >= other; }

  /// Subtraction of slot points.
  int32_t operator-(const slot_point& other) const
  {
    int a = static_cast<int>(count_val) - static_cast<int>(other.count_val);
    if (a >= (int)nof_slots_per_hf() / 2) {
      return a - nof_slots_per_hf();
    }
    if (a < -(int)nof_slots_per_hf() / 2) {
      return a + nof_slots_per_hf();
    }
    return a;
  }

  /// Sum of slot points by a jump value.
  slot_point& operator++()
  {
    count_val++;
    if (count_val == nof_slots_per_hf()) {
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
  slot_point& operator+=(uint32_t jump)
  {
    count_val = (count_val + jump) % nof_slots_per_hf();
    return *this;
  }

  /// Subtraction of slot point by a jump value.
  slot_point& operator-=(uint32_t jump)
  {
    int a = (static_cast<int>(count_val) - static_cast<int>(jump)) % static_cast<int>(nof_slots_per_hf());
    if (a < 0) {
      a += nof_slots_per_hf();
    }
    count_val = a;
    return *this;
  }

  /// Determines whether this fits within provided slot points.
  bool is_in_interval(slot_point begin, slot_point end) const { return (*this >= begin and *this < end); }
};

/// Sum and subtraction operations over slot_point types.
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

/// Get maximum of two slots.
inline slot_point max(slot_point lhs, slot_point rhs)
{
  return lhs > rhs ? lhs : rhs;
}
inline slot_point min(slot_point lhs, slot_point rhs)
{
  return lhs < rhs ? lhs : rhs;
}

/// Slot Interval [start, stop).
using slot_interval = interval<slot_point>;

} // namespace srsgnb

namespace fmt {
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
