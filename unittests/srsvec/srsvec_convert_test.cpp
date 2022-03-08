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
#include "srsgnb/srsvec/conversion.h"
#include "srsgnb/support/srsran_assert.h"
#include <random>

static std::mt19937 rgen(0);
static const float  assert_cf_max_error    = 1e-3;
static const float  assert_float_max_error = 1e-3;

using namespace srsgnb;

void test_convert_ci(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<int16_t> z(2 * N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    int16_t gold_re = static_cast<int16_t>(x[i].real() * scale);
    int16_t gold_im = static_cast<int16_t>(x[i].imag() * scale);
    srsran_assert(gold_re == z[2 * i + 0], "Failed");
    srsran_assert(gold_im == z[2 * i + 1], "Failed");
  }
}

void test_convert_ci_swap(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<cf_t> x(N);
  for (cf_t& v : x) {
    v = cf_t(dist(rgen), dist(rgen));
  }

  srsvec::aligned_vec<int16_t> z(2 * N);

  float scale = 1000.0F;

  srsvec::convert_swap(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    int16_t gold_re = static_cast<int16_t>(x[i].imag() * scale);
    int16_t gold_im = static_cast<int16_t>(x[i].real() * scale);
    srsran_assert(gold_re == z[2 * i + 0], "Failed");
    srsran_assert(gold_im == z[2 * i + 1], "Failed");
  }
}

void test_convert_ic(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  srsvec::aligned_vec<int16_t> x(2 * N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<cf_t> z(N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold = {static_cast<float>(x[2 * i]) / scale, static_cast<float>(x[2 * i + 1]) / scale};
    float err  = std::abs(gold - z[i]);
    srsran_assert(err < assert_cf_max_error, "Failed");
  }
}

void test_convert_ic_swap(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  srsvec::aligned_vec<int16_t> x(2 * N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<cf_t> z(N);

  float scale = 1000.0F;

  srsvec::convert_swap(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    cf_t  gold = {static_cast<float>(x[2 * i + 1]) / scale, static_cast<float>(x[2 * i]) / scale};
    float err  = std::abs(gold - z[i]);
    srsran_assert(err < assert_cf_max_error, "Failed");
  }
}

void test_convert_fi(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1, 1);

  srsvec::aligned_vec<float> x(N);
  for (float& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<int16_t> z(N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    int16_t gold = static_cast<int16_t>(x[i] * scale);
    srsran_assert(gold == z[i], "Failed");
  }
}

void test_convert_if(std::size_t N)
{
  std::uniform_int_distribution<int16_t> dist(INT16_MIN, INT16_MAX);

  srsvec::aligned_vec<int16_t> x(N);
  for (int16_t& v : x) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> z(N);

  float scale = 1000.0F;

  srsvec::convert(x, scale, z);

  for (size_t i = 0; i != N; i++) {
    float gold = static_cast<float>(x[i]) / scale;
    float err  = std::abs(gold - z[i]);
    srsran_assert(err < assert_float_max_error, "Failed");
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_convert_ci(N);
    test_convert_ci_swap(N);
    test_convert_ic(N);
    test_convert_ic_swap(N);
    test_convert_fi(N);
    test_convert_if(N);
  }
}
