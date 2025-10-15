/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srsvec/modulus_square.h"
#include "srsran/support/math/math_utils.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);
static const float  ASSERT_CF_MAX_ERROR = 1e-6;

using namespace srsran;

namespace {

using SrsvecModulusSquareParams = unsigned;

class SrsvecModulusSquareFixture : public ::testing::TestWithParam<SrsvecModulusSquareParams>
{
protected:
  unsigned size;

  void SetUp() override
  {
    // Get test parameters.
    auto params = GetParam();
    size        = params;
  }
};

TEST_P(SrsvecModulusSquareFixture, SrsvecModulusSquareTestFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<float> result(size);

  srsvec::modulus_square(result, input);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(input[i]);
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(SrsvecModulusSquareFixture, SrsvecModulusSquareAndAddTestFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cf_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cf_t(dist(rgen), dist(rgen)); });

  std::vector<float> offset(size);
  std::generate(offset.begin(), offset.end(), [&dist]() { return dist(rgen); });

  std::vector<float> result(size);

  srsvec::modulus_square_and_add(result, input, offset);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(input[i]) + offset[i];
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(SrsvecModulusSquareFixture, SrsvecModulusSquareTestBrainFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cbf16_t(dist(rgen), dist(rgen)); });

  std::vector<float> result(size);

  srsvec::modulus_square(result, input);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(to_cf(input[i]));
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

TEST_P(SrsvecModulusSquareFixture, SrsvecModulusSquareAndAddTestBrainFloatComplex)
{
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  std::vector<cbf16_t> input(size);
  std::generate(input.begin(), input.end(), [&dist]() { return cbf16_t(dist(rgen), dist(rgen)); });

  std::vector<float> offset(size);
  std::generate(offset.begin(), offset.end(), [&dist]() { return dist(rgen); });

  std::vector<float> result(size);

  srsvec::modulus_square_and_add(result, input, offset);

  for (size_t i = 0; i != size; ++i) {
    cf_t  gold_z = std::norm(to_cf(input[i])) + offset[i];
    float err    = std::abs(gold_z - result[i]);
    ASSERT_TRUE(err < ASSERT_CF_MAX_ERROR);
  }
}

INSTANTIATE_TEST_SUITE_P(SrsvecModulusSquareTest,
                         SrsvecModulusSquareFixture,
                         ::testing::Values(1, 5, 7, 19, 23, 257, 1234));

} // namespace
