/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/binary.h"
#include "srsgnb/support/srsran_assert.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsgnb;

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
    srsran_assert(gold_z == z[i], "Failed");
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
    srsran_assert(gold_z == z[i], "Failed");
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
    srsran_assert(gold_z == z[i], "Failed");
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
