/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/division.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-3;

using namespace srsran;

void test_divide_fff(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<float> num(N);
  for (float& v : num) {
    v = dist(rgen);
  }

  std::vector<float> den(N);
  for (float& v : den) {
    v = dist(rgen);
  }

  std::vector<float> result(N);

  srsvec::divide(result, num, den);

  for (size_t i = 0; i != N; ++i) {
    float gold_z = 0.0F;
    if (std::isnormal(num[i]) && std::isnormal(den[i])) {
      gold_z = num[i] / den[i];
    }
    float err = std::abs(gold_z - result[i]);
    if (std::isnormal(gold_z)) {
      err /= std::abs(gold_z);
    }

    TESTASSERT(err < ASSERT_MAX_ERROR, "gold={} result={} err={}", gold_z, result[i], err);
  }
}

int main()
{
  std::vector<std::size_t> sizes = {1, 5, 7, 19, 23, 257};

  for (std::size_t N : sizes) {
    test_divide_fff(N);
  }
}
