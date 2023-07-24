/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/srsvec/division.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_MAX_ERROR = 1e-3;

using namespace srsran;

void test_divide_fff(std::size_t N)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  srsvec::aligned_vec<float> num(N);
  for (float& v : num) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> den(N);
  for (float& v : den) {
    v = dist(rgen);
  }

  srsvec::aligned_vec<float> result(N);

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
