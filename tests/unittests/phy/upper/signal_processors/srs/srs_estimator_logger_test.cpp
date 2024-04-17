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
#include "srs_estimator_test_doubles.h"
#include "srsran/phy/upper/signal_processors/srs/formatters.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_factory.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

namespace {

// Valid SRS configuration used as a base for the test case.
const srs_estimator_configuration base_config = {{0, 130, 8, 0},
                                                 {srs_resource_configuration::one_two_four_enum(2),
                                                  srs_resource_configuration::one_two_four_enum(1),
                                                  12,
                                                  17,
                                                  647,
                                                  2,
                                                  srs_resource_configuration::comb_size_enum(2),
                                                  1,
                                                  1,
                                                  66,
                                                  1,
                                                  3,
                                                  srs_resource_configuration::group_or_sequence_hopping_enum::neither,
                                                  {}},
                                                 {0}};

class srs_estimator_dummy_factory : public srs_estimator_factory
{
public:
  std::unique_ptr<srs_estimator> create() override { return std::make_unique<srs_estimator_dummy>(); }

  std::unique_ptr<srs_estimator_configuration_validator> create_validator() override { return nullptr; }
};

TEST(srsEstimator, LoggerInfoTest)
{
  srslog::init();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY", true);
  logger.set_level(srslog::basic_levels::info);

  std::shared_ptr<srs_estimator_factory> factory = std::make_shared<srs_estimator_dummy_factory>();

  std::unique_ptr<srs_estimator> estimator = factory->create(logger);
  ASSERT_NE(estimator, nullptr);

  // Prepare resource grid and resource grid mapper spies.
  resource_grid_reader_spy grid(0, 0, 0);

  estimator->estimate(grid, base_config);
}

TEST(srsEstimator, LoggerDebugTest)
{
  srslog::init();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY", true);
  logger.set_level(srslog::basic_levels::debug);

  std::shared_ptr<srs_estimator_factory> factory = std::make_shared<srs_estimator_dummy_factory>();

  std::unique_ptr<srs_estimator> estimator = factory->create(logger);
  ASSERT_NE(estimator, nullptr);

  // Prepare resource grid and resource grid mapper spies.
  resource_grid_reader_spy grid(0, 0, 0);

  estimator->estimate(grid, base_config);
}

} // namespace
