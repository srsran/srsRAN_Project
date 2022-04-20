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

#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR    = 1e-6;
static const float  ASSERT_FLOAT_MAX_ERROR = 1e-6;

using namespace srsgnb;

void test_cf_add(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<cf_t> y(N);
  for (cf_t& v : y) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<cf_t> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold_z = x[i] + y[i];
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_CF_MAX_ERROR);
  }
}

void test_float_add(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> y(N);
  for (float& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    float gold_z = x[i] + y[i];
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_FLOAT_MAX_ERROR);
  }
}

void test_i16_add(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN / 2, INT16_MAX / 2);

  srsvec::aligned_vec<int16_t> x(N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int16_t> y(N);
  for (int16_t& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int16_t> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    int gold_z = x[i] + y[i];
    int err    = std::abs(gold_z - z[i]);
    TESTASSERT_EQ(err, 0);
  }
}

void test_i8_add(std::size_t N)
{
  std::uniform_int_distribution<int8_t> dist(INT8_MIN / 2, INT8_MAX / 2);

  srsvec::aligned_vec<int8_t> x(N);
  for (int8_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int8_t> y(N);
  for (int8_t& v : y) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int8_t> z(N);

  srsvec::add(x, y, z);

  for (size_t i = 0; i != N; i++) {
    int gold_z = x[i] + y[i];
    int err    = std::abs(gold_z - z[i]);
    TESTASSERT_EQ(err, 0);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_cf_add(N);
    test_float_add(N);
    test_i16_add(N);
    test_i8_add(N);
  }
}