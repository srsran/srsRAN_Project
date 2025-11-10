/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdcp_rx_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsran {

srsran::log_sink_spy& test_spy = []() -> srsran::log_sink_spy& {
  if (!srslog::install_custom_sink(
          srsran::log_sink_spy::name(),
          std::unique_ptr<srsran::log_sink_spy>(new srsran::log_sink_spy(srslog::get_default_log_formatter())))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<srsran::log_sink_spy*>(srslog::find_sink(srsran::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  srslog::fetch_basic_logger("PDCP", *spy, true);
  return *spy;
}();

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_rx_test : public pdcp_rx_test_helper_default_crypto,
                     public ::testing::Test,
                     public ::testing::WithParamInterface<std::tuple<pdcp_sn_size, unsigned>>
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // reset log spy
    test_spy.reset_counters();

    // init RLC logger
    srslog::fetch_basic_logger("PDCP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};
} // namespace srsran
