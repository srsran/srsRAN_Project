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

#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_test.h"
#include <random>

using namespace srsran;

template <typename T>
void test_resize(std::size_t N)
{
  // Allocate
  srsvec::aligned_vec<T> x(N);

  // Resize
  x.resize(2 * N);
}

template <typename T>
void test_zero(std::size_t N)
{
  // Allocate
  srsvec::aligned_vec<T> x(N);

  // Zero
  srsvec::zero(x);

  // Verify copy
  for (size_t i = 0; i != N; i++) {
    TESTASSERT_EQ(x[i], 0);
  }
}

template <typename T>
void test_copy(std::size_t N)
{
  // Allocate
  srsvec::aligned_vec<T> x(N);

  // Fill vector
  for (std::size_t i = 0; i != N; i++) {
    x[i] = i;
  }

  // Allocate and resize
  srsvec::aligned_vec<T> y(N);

  // Copy
  srsvec::copy(y, x);

  // Verify copy
  for (size_t i = 0; i != N; i++) {
    TESTASSERT_EQ(x[i], y[i]);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_resize<float>(N);
    test_resize<int16_t>(N);
    test_resize<int8_t>(N);

    test_zero<float>(N);
    test_zero<int16_t>(N);
    test_zero<int8_t>(N);

    test_copy<float>(N);
    test_copy<int16_t>(N);
    test_copy<int8_t>(N);
  }
}