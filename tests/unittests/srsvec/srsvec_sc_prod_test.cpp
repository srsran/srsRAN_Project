/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-6;

using namespace srsran;

void test_sc_prod_ccc(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  cf_t h = {dist(rgen), dist(rgen)};

  std::vector<cf_t> z(N);

  srsvec::sc_prod(x, h, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold_z = x[i] * h;
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

void test_sc_prod_ccc_bf16(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> x(N);
  std::generate(x.begin(), x.end(), [&dist]() { return to_cbf16(cf_t{dist(rgen), dist(rgen)}); });

  cf_t h = {dist(rgen), dist(rgen)};

  std::vector<cbf16_t> z(N);

  srsvec::sc_prod(x, h, z);

  std::vector<cf_t> expected(N);
  std::transform(x.begin(), x.end(), expected.begin(), [&h](cbf16_t value) { return h * to_cf(value); });

  for (size_t i = 0; i != N; i++) {
    cf_t  gold_z = to_cf(x[i]) * h;
    float err    = std::abs(gold_z - to_cf(z[i]));
    TESTASSERT(err < 5e-3, " err={}", err);
  }
}

void test_sc_prod_cfc(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  float h = dist(rgen);

  std::vector<cf_t> z(N);

  srsvec::sc_prod(x, h, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold_z = x[i] * h;
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

void test_sc_prod_fff(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  float h = dist(rgen);

  std::vector<float> z(N);

  srsvec::sc_prod(x, h, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold_z = x[i] * h;
    float err    = std::abs(gold_z - z[i]);
    TESTASSERT(err < ASSERT_MAX_ERROR);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_sc_prod_ccc(N);
    test_sc_prod_ccc_bf16(N);
    test_sc_prod_cfc(N);
    test_sc_prod_fff(N);
  }
}
