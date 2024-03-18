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

#include "../../../support/resource_grid_test_doubles.h"
#include "srs_estimator_test_data.h"
#include "srsran/phy/upper/signal_processors/srs/formatters.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_factory.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/srs/srs_channel_matrix_formatters.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <memory>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.context.config);
  return os;
}

std::ostream& operator<<(std::ostream& os, const srs_channel_matrix& channel)
{
  fmt::print(os, "{}", channel);
  return os;
}

bool operator==(const srs_channel_matrix& left, const srs_channel_matrix& right)
{
  return left.is_near(right, 1e-2);
}

} // namespace srsran

using namespace srsran;

class SrsEstimatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  void SetUp() override
  {
    std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory =
        create_low_papr_sequence_generator_sw_factory();
    ASSERT_NE(sequence_generator_factory, nullptr);

    std::shared_ptr<srs_estimator_factory> srs_est_factory =
        create_srs_estimator_generic_factory(sequence_generator_factory);
    ASSERT_NE(srs_est_factory, nullptr);

    estimator = srs_est_factory->create();
    ASSERT_NE(estimator, nullptr);
  }

  std::unique_ptr<srs_estimator> estimator = nullptr;
};

TEST_P(SrsEstimatorFixture, FromVector)
{
  const test_case_t&                 test_case = GetParam();
  const srs_estimator_configuration& config    = test_case.context.config;

  resource_grid_reader_spy grid;
  grid.write(GetParam().rx_grid.read());

  srs_estimator_result result = estimator->estimate(grid, config);

  ASSERT_EQ(test_case.context.result.channel_matrix, result.channel_matrix);
}

INSTANTIATE_TEST_SUITE_P(SrsEstimatorFixture, SrsEstimatorFixture, ::testing::ValuesIn(srs_estimator_test_data));