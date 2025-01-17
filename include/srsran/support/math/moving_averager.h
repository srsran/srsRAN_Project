/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include <cstdint>
#include <type_traits>
#include <vector>

namespace srsran {

namespace detail {

template <typename T>
class base_moving_average
{
public:
  base_moving_average() = default;
  base_moving_average(unsigned sliding_window_size) : sliding_window(sliding_window_size, 0) {}

  size_t size() const { return sliding_window.size(); }

protected:
  void push_sample(T sample)
  {
    sliding_window[write_pos++] = sample;
    if (write_pos >= sliding_window.size()) {
      write_pos = 0;
    }
  }

  void resize_window(unsigned sliding_window_size)
  {
    sliding_window.resize(sliding_window_size);
    clear_samples();
  }

  void clear_samples()
  {
    std::fill(sliding_window.begin(), sliding_window.end(), 0);
    write_pos = 0;
  }

  std::vector<T> sliding_window;

  unsigned write_pos = 0;
};

} // namespace detail

/// Implementation of a moving average using a sliding window.
template <typename T>
class moving_averager : public detail::base_moving_average<T>
{
  static_assert(std::is_integral_v<T>, "T must be integral to avoid loss of precision");

  using base_type = detail::base_moving_average<T>;

public:
  using base_type::base_type;

  void push(T sample)
  {
    sum_samples -= this->sliding_window[this->write_pos];
    sum_samples += sample;
    this->push_sample(sample);
  }

  double average() const { return sum_samples / static_cast<double>(this->size()); }

  void clear()
  {
    this->clear_samples();
    sum_samples = 0;
  }

  void resize(unsigned sliding_window_size)
  {
    if (sliding_window_size == this->size()) {
      return;
    }
    this->resize_window(sliding_window_size);
    sum_samples = 0;
  }

private:
  using sum_samples_type = std::conditional_t<std::is_unsigned_v<T>, uint64_t, int64_t>;

  sum_samples_type sum_samples = 0;
};

/// Specialization of moving_averager for the double case.
///
/// The main difference is that after N samples, where N is the size of the sliding window, the average is recalculated.
template <>
class moving_averager<double> : public detail::base_moving_average<double>
{
  using base_type = detail::base_moving_average<double>;

public:
  using base_type::base_type;

  void push(double sample)
  {
    sum_samples += sample - sliding_window[write_pos];
    push_sample(sample);
    if (write_pos == 0) {
      // Double operations suffer from loss of precision. So, the precision error can accummulate in \c sum_samples.
      // To avoid this, we recompute the average inside the sliding window every N number of samples, where the N is
      // the size of the sliding window.
      sum_samples = 0;
      for (double samp : sliding_window) {
        sum_samples += samp;
      }
    }
  }

  double average() const { return sum_samples / static_cast<double>(size()); }

  void clear()
  {
    this->clear_samples();
    sum_samples = 0;
  }

  void resize(unsigned sliding_window_size)
  {
    if (sliding_window_size == this->size()) {
      return;
    }
    this->resize_window(sliding_window_size);
    sum_samples = 0;
  }

private:
  double sum_samples = 0;
};

} // namespace srsran
