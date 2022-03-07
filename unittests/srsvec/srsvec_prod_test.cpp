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
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/support/srsran_assert.h"
#include <random>

static std::mt19937 rgen(0);
static const float  assert_max_error = 1e-6;

using namespace srsgnb;

void test_prod_ccc(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = {dist(rgen), dist(rgen)};
  }

  srsvec::aligned_vec<cf_t> y(N);
  for (cf_t& v : y) {
    v = {dist(rgen), dist(rgen)};
  }

  srsvec::aligned_vec<cf_t> z(N);

  srsvec::prod(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    srsran_assert(err < assert_max_error, "Failed");
  }
}

void test_prod_fff(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> y(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> z(N);

  srsvec::prod(x, y, z);

  for (size_t i = 0; i != N; ++i) {
    cf_t  gold_z = x[i] * y[i];
    float err    = std::abs(gold_z - z[i]);
    srsran_assert(err < assert_max_error, "Failed");
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