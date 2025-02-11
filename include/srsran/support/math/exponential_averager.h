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

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Implements an exponential moving average accumulator, with simple moving average for faster initial
/// convergence. This accumulator is useful for tracking the average of a series of values, with a bias towards the most
/// recent values. The accumulator uses a simple moving average until a certain number of samples have been accumulated,
/// and then switches to exponential moving average.
template <typename T>
class exp_average_fast_start
{
public:
  exp_average_fast_start(T alpha_val_) : exp_average_fast_start(alpha_val_, 1.0 / alpha_val_) {}
  exp_average_fast_start(T alpha_val_, unsigned start_size) : alpha_value(alpha_val_), start_count_size(start_size)
  {
    srsran_assert(alpha_value < 1, "Alpha must be less than 1");
    srsran_assert(start_size > 0, "Start size must be greater than 0");
  }

  /// Push new sample.
  void push(T sample)
  {
    // Simple moving average until start_count_size is reached. This speeds up the convergence of the average value.
    if (count < start_count_size) {
      average += (sample - average) / (count + 1);
      ++count;
    }
    // Exponential moving average after start_count_size is reached.
    else {
      average = (1 - alpha_value) * average + alpha_value * sample;
    }
  }

  /// Push zeros \c n times.
  void push_zeros(unsigned n)
  {
    if (count < start_count_size) {
      const unsigned rem = std::min(start_count_size - count, n);
      for (unsigned i = 0; i != rem; ++i) {
        average -= average / (count + 1);
        ++count;
      }
      n -= rem;
    }

    if (n > 0) {
      average = std::pow(1 - alpha_value, n) * average;
    }
  }

  void reset()
  {
    count   = 0;
    average = 0;
  }

  /// Get the current average value.
  T get_average_value() const { return average; }

  /// Get the forgetting coefficient being used.
  T get_alpha() const { return alpha_value; }

  /// Whether the averager is in fast start stage or exponential average stage.
  bool is_exp_average_mode() const { return count >= start_count_size; }

private:
  T        average = 0;
  T        alpha_value;
  unsigned count = 0;
  unsigned start_count_size;
};

} // namespace srsran
