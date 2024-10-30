/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/prod.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace srsran;

void test_prod_ccc(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> y(N);
  for (cf_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  std::vector<cf_t> z(N);

  srsvec::prod(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

void test_prod_fff(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  std::vector<float> y(N);
  for (float& v : y) {
    v = dist(rgen);
  }

  std::vector<float> z(N);

  srsvec::prod(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_prod_ccc(N);
    test_prod_fff(N);
  }
}
