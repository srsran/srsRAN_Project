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

#include "srsgnb/adt/span.h"
#include <random>

namespace srsgnb {

template <size_t N>
std::array<bool, N> create_contiguous_array(unsigned number_of_ones)
{
  std::array<bool, N> ar = {};
  std::fill(ar.begin(), ar.begin() + number_of_ones, true);
  return ar;
}

template <size_t N>
std::array<bool, N> shuffle_array(const std::array<bool, N>& ar)
{
  static std::mt19937 rgen;
  std::array<bool, N> ar2 = ar;
  std::shuffle(ar2.begin(), ar2.end(), rgen);
  return ar;
}

} // namespace srsgnb