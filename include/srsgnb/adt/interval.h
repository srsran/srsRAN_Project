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

#include "srsgnb/support/srsgnb_assert.h"
#include "fmt/format.h"
#include <cassert>
#include <string>
#include <type_traits>

namespace srsgnb {

/// Representation of an interval between two numeric-types with the math representation [start, stop).
template <typename T>
class interval
{
  static_assert(std::is_trivially_copyable<T>::value, "Expected to be trivially copyable");

public:
  using length_type = std::conditional_t<(sizeof(T) > sizeof(uint32_t)), uint64_t, uint32_t>;

  interval() : start_(T{}), stop_(T{}) {}

  template <typename U, typename V>
  interval(U start_point, V stop_point) : start_(start_point), stop_(stop_point)
  {
    static_assert(std::is_convertible<U, T>::value, "Invalid interval start point type");
    static_assert(std::is_convertible<V, T>::value, "Invalid interval stop point type");
    srsgnb_assert(start_ <= stop_, "Invalid interval [{}, {})", start_, stop_);
  }

  T start() const { return start_; }

  T stop() const { return stop_; }

  bool empty() const { return stop_ == start_; }

  /// Interval length
  length_type length() const { return stop_ - start_; }

  void set(T start_point, T stop_point)
  {
    srsgnb_assert(stop_point >= start_point, "interval::set called for invalid range points");
    start_ = start_point;
    stop_  = stop_point;
  }

  /// Increase the interval length, maintaining the same starting point.
  /// \param length_increment Length increment/decrement to apply to current interval size. In case
  /// length_increment < 0, the condition -length_increment <= length() must be met.
  void extend(T length_increment)
  {
    // Detect length overflows
    srsgnb_assert(std::is_unsigned<T>::value or (length_increment >= 0 or length() >= -length_increment),
                  "Resulting interval would be invalid");
    stop_ += length_increment;
  }

  /// Set the interval length, maintaining the same starting point.
  void resize(T new_length)
  {
    srsgnb_assert(std::is_unsigned<T>::value or new_length >= 0, "Interval width must be positive");
    stop_ = start_ + new_length;
  }

  /// Move interval by an offset.
  void displace_by(int offset)
  {
    srsgnb_assert(
        std::is_signed<T>::value or static_cast<int64_t>(start_) >= -offset,
        "Cannot have negative starting_points in case interval<T> underlying type is unsigned. Start={} < offset={}",
        start_,
        -offset);
    start_ += offset;
    stop_ += offset;
  }

  /// Move interval start to provided starting point.
  void displace_to(T start_point)
  {
    stop_  = start_point + length();
    start_ = start_point;
  }

  /// If this and other intervals overlap.
  bool overlaps(interval other) const
  {
    // Note: if one or both intervals are empty, this will always return false
    return start_ < other.stop_ and other.start_ < stop_;
  }

  /// If interval contains provided point.
  bool contains(T point) const { return start_ <= point and point < stop_; }

  /// If interval contains provided interval.
  bool contains(interval other) const { return start_ <= other.start_ and other.stop_ <= stop_; }

  /// Assign to this interval the intersection between this and the provided interval.
  interval<T>& intersect(const interval<T>& other)
  {
    if (not overlaps(other)) {
      *this = interval<T>{};
    } else {
      start_ = std::max(start(), other.start());
      stop_  = std::min(stop(), other.stop());
    }
    return *this;
  }

private:
  T start_;
  T stop_;
};

template <typename T>
bool operator==(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start() == rhs.start() and lhs.stop() == rhs.stop();
}

template <typename T>
bool operator!=(const interval<T>& lhs, const interval<T>& rhs)
{
  return not(lhs == rhs);
}

/// Compares interval starting points. In case the starting point is the same, the interval with shorter length
/// is the lowest.
template <typename T>
bool operator<(const interval<T>& lhs, const interval<T>& rhs)
{
  return lhs.start() < rhs.start() or (lhs.start() == rhs.start() and lhs.stop() < rhs.stop());
}

/// Make union of intervals. If intervals do not overlap, the empty interval is returned
template <typename T>
interval<T> operator|(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return {std::min(lhs.start(), rhs.start()), std::max(lhs.stop(), rhs.stop())};
}

/// Make intersection of intervals
template <typename T>
interval<T> operator&(const interval<T>& lhs, const interval<T>& rhs)
{
  if (not lhs.overlaps(rhs)) {
    return interval<T>{};
  }
  return interval<T>{std::max(lhs.start(), rhs.start()), std::min(lhs.stop(), rhs.stop())};
}

} // namespace srsgnb

namespace fmt {

/// Format intervals with the notation [start, stop)
template <typename T>
struct formatter<srsgnb::interval<T>> : public formatter<T> {
  template <typename FormatContext>
  auto format(const srsgnb::interval<T>& interv, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "[{}, {})", interv.start(), interv.stop());
  }
};

} // namespace fmt
