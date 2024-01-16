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
#include "srsran/srsvec/binary.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

template <typename T, unsigned RANGE>
void test_binary_xor(std::size_t N)
{
  std::uniform_int_distribution<T> dist(0, RANGE);

  srsvec::aligned_vec<T> x(N);
  for (T& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<T> y(N);
  for (T& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<T> z(N);

  srsvec::binary_xor(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    T gold_z = x[i] ^ y[i];
    TESTASSERT_EQ(gold_z, z[i]);
  }
}

template <typename T, unsigned RANGE>
void test_binary_and(std::size_t N)
{
  std::uniform_int_distribution<T> dist(0, RANGE);

  srsvec::aligned_vec<T> x(N);
  for (T& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<T> y(N);
  for (T& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<T> z(N);

  srsvec::binary_and(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    T gold_z = x[i] & y[i];
    TESTASSERT_EQ(gold_z, z[i]);
  }
}

template <typename T, unsigned RANGE>
void test_binary_or(std::size_t N)
{
  std::uniform_int_distribution<T> dist(0, RANGE);

  srsvec::aligned_vec<T> x(N);
  for (T& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<T> y(N);
  for (T& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<T> z(N);

  srsvec::binary_or(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    T gold_z = x[i] | y[i];
    TESTASSERT_EQ(gold_z, z[i]);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_binary_xor<uint8_t, UINT8_MAX>(N);
    test_binary_and<uint8_t, UINT8_MAX>(N);
    test_binary_or<uint8_t, UINT8_MAX>(N);
  }
}
