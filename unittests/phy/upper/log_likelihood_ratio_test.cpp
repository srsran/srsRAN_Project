/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Unit test for the log-likelihood ratio type.
///
/// Tests all the operators and functions defined for the type.
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/support/srsgnb_test.h"
#include <iostream>

using namespace srsgnb;

int main()
{
  log_likelihood_ratio llr0 = 0;
  log_likelihood_ratio llr1 = 2;

  TESTASSERT_NEQ(llr0, llr1, "Not equal operator not working.");
  TESTASSERT((llr1 > llr0) && (llr0 < llr1), "Strict order comparison operators not working.");
  TESTASSERT((llr1 >= llr0) && (llr0 <= llr1), "Order comparison operators not working.");
  TESTASSERT((llr0 >= llr0) && (llr1 <= llr1), "Order comparison operators not working.");

  TESTASSERT_EQ(log_likelihood_ratio(-2), -llr1, "Negation not working.");

  TESTASSERT_EQ(log_likelihood_ratio(2), llr0 + llr1, "Sum not working.");
  TESTASSERT_EQ(log_likelihood_ratio(-2), llr0 - llr1, "Difference not working.");
  TESTASSERT_EQ(log_likelihood_ratio(2), log_likelihood_ratio::promotion_sum(llr0, llr1), "Promotion sum not working.");
  TESTASSERT_EQ(LLR_MAX, llr1 + 119, "Saturation not working.");

  llr0 += llr1;
  TESTASSERT_EQ(log_likelihood_ratio(2), llr0, "Addition assignment not working.");

  TESTASSERT_EQ(static_cast<int>(llr0), 2, "Static casting not working.");
  TESTASSERT_EQ(llr0.to_int(), 2, "Static casting not working.");

  llr0 = -100;
  llr1 = 100;
  TESTASSERT_EQ(-LLR_MAX, llr0 - llr1, "Saturation of negative values not working.");
  TESTASSERT_EQ(LLR_INFINITY, log_likelihood_ratio::promotion_sum(LLR_MAX, LLR_MAX), "Promotion sum not working.");

  std::array<log_likelihood_ratio, 4> llr_sequence = {2, -2, 2, -2};
  TESTASSERT(typeid(int) == typeid(log_likelihood_ratio::norm_squared(llr_sequence)),
             "norm_sqr_llr does not propagate type.");
  TESTASSERT(typeid(int) == typeid(log_likelihood_ratio::norm_squared(span<const log_likelihood_ratio>(llr_sequence))),
             "norm_sqr_llr does not propagate type.");

  constexpr std::array<log_likelihood_ratio, 4> llr_sequence_const = {2, -2, 2, -2};
  TESTASSERT(typeid(int) == typeid(log_likelihood_ratio::norm_squared(llr_sequence_const)),
             "norm_sqr_llr does not propagate type with const values.");

  TESTASSERT_EQ(16, log_likelihood_ratio::norm_squared(llr_sequence), "norm_sqr_llr not working.");
  std::array<int, 4> rhs = {1, 1, 1, 1};
  TESTASSERT(typeid(float) == typeid(log_likelihood_ratio::dot_prod(llr_sequence, rhs, 1.1F)),
             "dot_prod_llr does not propagate type.");
  TESTASSERT(std::abs(1.1F - log_likelihood_ratio::dot_prod(llr_sequence, rhs, 1.1F)) < .00001,
             "dot_prod_llr not working.");
}