/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../lib/gtpu/gtpu.h"
#include "srsgnb/support/timers.h"
#include <gtest/gtest.h>

namespace srsgnb {

/// Fixture class for GTP-U PDU tests
class gtpu_test : public ::testing::Test
{
public:
  gtpu_test() : logger(srslog::fetch_basic_logger("TEST", false)) {}

protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init GTPU logger
    srslog::fetch_basic_logger("GTPU", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("GTPU", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  // Test logger
  srslog::basic_logger& logger;
};
} // namespace srsgnb
