/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/mac/config/mac_config_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

using gbr_bps = uint64_t;

struct mac_lc_config_test_params {
  gbr_bps              input;
  prioritized_bit_rate expected_output;
};

class mac_lc_config_test : public ::testing::TestWithParam<mac_lc_config_test_params>
{};

TEST_P(mac_lc_config_test, verify_get_pbr_ge_given_bit_rate)
{
  mac_lc_config_test_params params = GetParam();
  ASSERT_EQ(params.expected_output, get_pbr_ceil(params.input));
}

INSTANTIATE_TEST_SUITE_P(mac_config_helpers_test,
                         mac_lc_config_test,
                         testing::Values(mac_lc_config_test_params{128000, prioritized_bit_rate::kBps16},
                                         mac_lc_config_test_params{256000, prioritized_bit_rate::kBps32},
                                         mac_lc_config_test_params{512000, prioritized_bit_rate::kBps64},
                                         mac_lc_config_test_params{484000, prioritized_bit_rate::kBps64}));
