/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "srsran/support/srsran_assert.h"
#include <numeric>

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
  T    get_average_value() const { return count == 0 ? 0 : average; }
  T    get_alpha() const { return alpha_value; }
  bool is_exp_average_mode() const { return count >= start_count_size; }

private:
  T        average = 0;
  T        alpha_value;
  unsigned count = 0;
  unsigned start_count_size;
};
