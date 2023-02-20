/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/srsgnb_test.h"
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