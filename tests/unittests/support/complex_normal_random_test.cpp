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

/// \file
/// \brief Complex normal distribution tests.

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

#include "srsran/support/complex_normal_random.h"
#include <gtest/gtest.h>

namespace srsran {

} // namespace srsran

namespace {
using namespace srsran;

template <typename ComplexType>
class ComplexNormalFixture : public ::testing::Test
{
protected:
  using value_type = ComplexType;
};

using TestTypes = ::testing::Types<cf_t, std::complex<double>>;
TYPED_TEST_SUITE(ComplexNormalFixture, TestTypes);

/// Test the interface to get and set the distribution parameters.
TYPED_TEST(ComplexNormalFixture, CheckParameters)
{
  using cx_type = typename TestFixture::value_type;

  // Create a default standard (mean 0 and variance 1) CND.
  complex_normal_distribution<cx_type> cnd = {};

  typename complex_normal_distribution<cx_type>::param_type exp_param(0, 1);
  ASSERT_EQ(cnd.param(), exp_param) << "Default CND not with mean 0 and variance 1.";
  ASSERT_EQ(cnd.mean(), cx_type(0, 0)) << "Wrong mean.";
  ASSERT_EQ(cnd.stddev(), 1) << "Wrong standard deviation.";

  exp_param = typename complex_normal_distribution<cx_type>::param_type(cx_type(2.0, 3.0), 2.33);
  cnd.param(exp_param);
  ASSERT_EQ(cnd.param(), exp_param) << "Default CND not with mean 0 and variance 1.";
  ASSERT_EQ(cnd.mean(), exp_param.mean()) << "Wrong mean.";
  ASSERT_EQ(cnd.stddev(), exp_param.stddev()) << "Wrong standard deviation.";
}

/// Test the generation of random numbers.
TYPED_TEST(ComplexNormalFixture, CheckGeneration)
{
  using cx_type = typename TestFixture::value_type;
  using r_type  = typename cx_type::value_type;

  const r_type tolerance = 1e-6;

  std::mt19937 rgen_cx(12345);
  std::mt19937 rgen_r(12345);

  // Create a default standard (mean 0 and variance 1) CND.
  complex_normal_distribution<cx_type> cnd = {};
  std::normal_distribution<r_type>     rnd = {};

  for (unsigned i = 0, i_max = 10; i != i_max; ++i) {
    cx_type expected = cx_type(rnd(rgen_r), rnd(rgen_r)) / std::sqrt<r_type>(2);
    cx_type obtained = cnd(rgen_cx);
    // Looks like gtest doesn't work well with complex numbers and we must split real and imaginary parts.
    ASSERT_NEAR(expected.real(), obtained.real(), tolerance) << "Real part generation failed.";
    ASSERT_NEAR(expected.imag(), obtained.imag(), tolerance) << "Imaginary part generation failed.";
  }

  // Try with ad-hoc parameters.
  typename complex_normal_distribution<cx_type>::param_type new_param(cx_type(2.0, 3.0), 2.33);
  for (unsigned i = 0, i_max = 10; i != i_max; ++i) {
    cx_type expected = cx_type(rnd(rgen_r), rnd(rgen_r)) * new_param.stddev() / std::sqrt<r_type>(2) + new_param.mean();
    cx_type obtained = cnd(rgen_cx, new_param);
    // Looks like gtest doesn't work well with complex numbers and we must split real and imaginary parts.
    ASSERT_NEAR(expected.real(), obtained.real(), tolerance) << "Real part generation failed.";
    ASSERT_NEAR(expected.imag(), obtained.imag(), tolerance) << "Imaginary part generation failed.";
  }

  // And now after changing the internal parameters.
  cnd.param(new_param);
  for (unsigned i = 0, i_max = 10; i != i_max; ++i) {
    cx_type expected = cx_type(rnd(rgen_r), rnd(rgen_r)) * new_param.stddev() / std::sqrt<r_type>(2) + new_param.mean();
    cx_type obtained = cnd(rgen_cx);
    // Looks like gtest doesn't work well with complex numbers and we must split real and imaginary parts.
    ASSERT_NEAR(expected.real(), obtained.real(), tolerance) << "Real part generation failed.";
    ASSERT_NEAR(expected.imag(), obtained.imag(), tolerance) << "Imaginary part generation failed.";
  }
}

TYPED_TEST(ComplexNormalFixture, CheckComparison)
{
  using cx_type = typename TestFixture::value_type;

  complex_normal_distribution<cx_type> cnd1 = {};
  complex_normal_distribution<cx_type> cnd2 = {};

  // Each distribution should be equal to itself.
  // Do comparison first since ASSERT_EQ doesn't seem to work well with non-elementary types.
  bool are_same      = (cnd1 == cnd1);
  bool are_different = (cnd1 != cnd1);
  ASSERT_TRUE(are_same) << "Comparison with itself failed.";
  ASSERT_FALSE(are_different) << "Comparison with itself failed.";

  // At this point, cnd1 and cnd2 should be the same.
  are_same      = (cnd1 == cnd2);
  are_different = (cnd1 != cnd2);
  ASSERT_TRUE(are_same) << "Comparison failed.";
  ASSERT_FALSE(are_different) << "Comparison with itself failed.";

  // If we change the cnd2 parameters, they shouldn't be equal anymore.
  typename complex_normal_distribution<cx_type>::param_type new_param(cx_type(2.0, 3.0), 2.33);
  cnd2.param(new_param);
  are_same      = (cnd1 == cnd2);
  are_different = (cnd1 != cnd2);
  ASSERT_FALSE(are_same) << "Comparison failed.";
  ASSERT_TRUE(are_different) << "Comparison with itself failed.";
}

} // namespace