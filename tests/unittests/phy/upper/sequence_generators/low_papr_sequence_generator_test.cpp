/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "low_papr_sequence_generator_test_data.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "u={} v={} n_cs={} n_cs_max={} M_zc={}",
             test_case.context.u,
             test_case.context.v,
             test_case.context.n_cs,
             test_case.context.n_cs_max,
             test_case.context.M_zc);
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

bool operator==(span<const cf_t> left, span<const cf_t> right)
{
  return std::equal(
      left.begin(), left.end(), right.begin(), right.end(), [](cf_t a, cf_t b) { return std::abs(a - b) < 1e-5; });
}

} // namespace srsran

namespace {

class LowPaprSequenceGeneratorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  void SetUp() override
  {
    std::shared_ptr<low_papr_sequence_generator_factory> factory = create_low_papr_sequence_generator_sw_factory();
    ASSERT_NE(factory, nullptr);

    generator = factory->create();
    ASSERT_NE(generator, nullptr);
  }

  std::unique_ptr<low_papr_sequence_generator> generator = nullptr;
};

} // namespace

TEST_P(LowPaprSequenceGeneratorFixture, FromVector)
{
  const test_case_t& test_case = GetParam();

  std::vector<cf_t> sequence(test_case.context.M_zc);
  generator->generate(
      sequence, test_case.context.u, test_case.context.v, test_case.context.n_cs, test_case.context.n_cs_max);

  std::vector<cf_t> expected_sequence = test_case.sequence.read();

  ASSERT_EQ(span<const cf_t>(sequence), span<const cf_t>(expected_sequence));
}

INSTANTIATE_TEST_SUITE_P(LowPaprSequenceGenerator,
                         LowPaprSequenceGeneratorFixture,
                         ::testing::ValuesIn(low_papr_sequence_generator_test_data));
