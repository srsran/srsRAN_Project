/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/rohc/rohc_lib/rohc_lib_engine.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::rohc;

/// Fixture class for ROHC engine with ROHC library tests
class rohc_lib_engine_test : public ::testing::Test
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

    logger.info("Creating ROHC engine with ROHC library.");

    engine = std::make_unique<rohc_lib_engine>();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&            logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<rohc_lib_engine> engine;
};

TEST_F(rohc_lib_engine_test, create_rohc_engine_when_supported)
{
  EXPECT_NE(engine, nullptr);
}

int main(int argc, char** argv)
{
  srslog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
