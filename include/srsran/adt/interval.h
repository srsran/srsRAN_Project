/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"
#include <cassert>
#include <string>
#include <type_traits>

namespace srsran {

/// Representation of an interval between two numeric-types with the math representation [start, stop).
template <typename T, bool RightClosed = false>
class interval
{
  static_assert(std::is_trivially_copyable<T>::value, "Expected to be trivially copyable");

public:
  /// Whether the interval is of real numbers.
  using is_real = std::is_floating_point<T>;
  /// Type used to represent the interval length.
  using length_type =
      std::conditional_t<is_real::value, T, std::conditional_t<(sizeof(T) > sizeof(uint32_t)), uint64_t, uint32_t>>;

  constexpr interval() : start_(T{}), stop_(T{}) {}

  template <typename U, typename V>
  constexpr interval(U start_point, V stop_point) : start_(start_point), stop_(stop_point)
  {
    static_assert(std::is_convertible<U, T>::value, "Invalid interval start point type");
    static_assert(std::is_convertible<V, T>::value, "Invalid interval stop point type");
    srsran_assert(start_ <= stop_, "Invalid interval [{}, {})", start_, stop_);
  }

  T start() const { return start_; }

  T stop() const { return stop_; }

  bool empty() const { return not RightClosed and stop_ == start_; }

  /// Interval length. e.g. [0, 1), [0, 1], [0 .. 1) have length 1. [0 .. 1] has length 2.
  length_type length() const
  {
    return static_cast<length_type>(stop_ - start_ + ((RightClosed and not is_real::value) ? 1 : 0));
  }

  void set(T start_point, T stop_point)
  {
    srsran_assert(stop_point >= start_point, "interval::set called for invalid range points");
    start_ = start_point;
    stop_  = stop_point;
  }

  /// Increase the interval length, maintaining the same starting point.
  /// \param length_increment Length increment/decrement to apply to current interval size. In case
  /// length_increment < 0, the condition -length_increment <= length() must be met.
  void extend(T length_increment)
  {
    // Detect length overflows
    srsran_assert(std::is_unsigned<T>::value or (length_increment >= 0 or length() >= -length_increment),
                  "Resulting interval would be invalid");
    stop_ += length_increment;
  }

  /// Set the interval length, maintaining the same starting point.
  void resize(length_type new_length) { stop_ = start_ + new_length; }

  /// Move interval by an offset.
  void displace_by(int offset)
  {
    srsran_assert(
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
    return RightClosed ? start_ <= other.stop_ and other.start_ <= stop_
                       : start_ < other.stop_ and other.start_ < stop_;
  }

  /// If interval contains provided point.
  bool contains(T point) const { return start_ <= point and (point < stop_ or (RightClosed and point == stop_)); }

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

} // namespace srsran

namespace fmt {

/// Format intervals with the notation [start, stop)
template <typename T, bool RightClosed>
struct formatter<srsran::interval<T, RightClosed>> : public formatter<T> {
  template <typename FormatContext>
  auto format(const srsran::interval<T, RightClosed>& interv, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "[{}{}{}{}",
                     interv.start(),
                     srsran::interval<T, RightClosed>::is_real::value ? ", " : "..",
                     interv.stop(),
                     RightClosed ? ']' : ')');
  }
};

} // namespace fmt
