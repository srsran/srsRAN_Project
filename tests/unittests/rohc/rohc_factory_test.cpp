/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/rohc/rohc_factory.h"
#include "srsran/rohc/rohc_support.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::rohc;

/// Fixture class for ROHC factory tests
class rohc_factory_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init ROHC logger
    srslog::fetch_basic_logger("ROHC", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ROHC", false).set_hex_dump_max_size(-1);

    logger.info("ROHC factory test initialized.");
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);
};

TEST_F(rohc_factory_test, create_rohc_engine_when_supported)
{
  std::unique_ptr<rohc_engine> engine = create_rohc_engine();
  if (rohc_supported()) {
    EXPECT_NE(engine, nullptr);
  } else {
    EXPECT_EQ(engine, nullptr);
  }
}

int main(int argc, char** argv)
{
  srslog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
