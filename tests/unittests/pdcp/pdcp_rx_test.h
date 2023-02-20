/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/pdcp/pdcp_entity_rx.h"
#include "pdcp_rx_test_helpers.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace srsran {

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_rx_test : public pdcp_rx_test_helper,
                     public ::testing::Test,
                     public ::testing::WithParamInterface<pdcp_sn_size>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init RLC logger
    srslog::fetch_basic_logger("PDCP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);

    // init SN size
    sn_size = GetParam();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};
} // namespace srsran
