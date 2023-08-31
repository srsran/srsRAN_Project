/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/support/pdcch_aggregation_level_calculator.h"
#include "srsran/ran/csi_rs/csi_report_config.h"
#include <gtest/gtest.h>

using namespace srsran;

class pdcch_aggregation_level_calculation_corner_cases_tester : public ::testing::Test
{};

TEST_F(pdcch_aggregation_level_calculation_corner_cases_tester,
       returns_nullopt_if_no_pdcch_candidates_are_configured_for_computed_aggregation_level_and_higher)
{
  // PDCCH candidates configured only for aggregation level 1.
  const std::array<uint8_t, 5> pdcch_candidates = {4, 0, 0, 0, 0};
  // Force computed aggregation level to be 2 by considering DCI size in bits large enough to not fit in 1 CCE.
  const unsigned dci_size_in_bits = 115;
  const unsigned cqi              = 6;

  const auto aggr_lvl = map_cqi_to_aggregation_level(
      cqi, cqi_table_t::table1, aggregation_level::n1, aggregation_level::n16, pdcch_candidates, dci_size_in_bits);

  ASSERT_FALSE(aggr_lvl.has_value()) << fmt::format("Aggregation level must not be returned");
}

TEST_F(pdcch_aggregation_level_calculation_corner_cases_tester,
       returns_higher_aggregation_level_when_code_rate_exceeds_effective_code_rate_at_an_aggregation_level)
{
  // PDCCH candidates configured only for aggregation level 2 and 4.
  const std::array<uint8_t, 5> pdcch_candidates = {0, 4, 2, 0, 0};
  // Consider DCI size in bits so that code rate computed over aggregation level 2 exceeds allowed effective code
  // rate at given CQI (i.e. 4).
  const unsigned dci_size_in_bits = 115;
  const unsigned cqi              = 4;

  const auto aggr_lvl = map_cqi_to_aggregation_level(
      cqi, cqi_table_t::table1, aggregation_level::n1, aggregation_level::n16, pdcch_candidates, dci_size_in_bits);

  ASSERT_TRUE(aggr_lvl.has_value() and aggr_lvl.value() == srsran::aggregation_level::n4)
      << fmt::format("Computed Aggregation level must be 4");
}

TEST_F(pdcch_aggregation_level_calculation_corner_cases_tester,
       returns_nullopt_if_no_aggregation_level_is_applicable_between_min_and_max_aggregation_level)
{
  // PDCCH candidates configured only for aggregation level 4, 8 and 16.
  const std::array<uint8_t, 5> pdcch_candidates = {0, 0, 2, 1, 1};
  // Consider DCI size large enough to require aggregation level 4.
  const unsigned dci_size_in_bits = 125;
  const unsigned cqi              = 2;

  const auto aggr_lvl = map_cqi_to_aggregation_level(
      cqi, cqi_table_t::table1, aggregation_level::n1, aggregation_level::n2, pdcch_candidates, dci_size_in_bits);

  ASSERT_FALSE(aggr_lvl.has_value()) << fmt::format("Aggregation level must not be returned");
}

struct pdcch_aggregation_level_calculation_test_params {
  unsigned               cqi;
  cqi_table_t            cqi_table;
  aggregation_level      min_aggr_level;
  aggregation_level      max_aggr_level;
  std::array<uint8_t, 5> pdcch_candidates;
  unsigned               dci_size_in_bits;
  aggregation_level      expected_aggr_level;
};

/// Formatter for test params.
void PrintTo(const pdcch_aggregation_level_calculation_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("cqi={} cqi_table={} min_al={} max_al={} pdcch_candidates=[{}] dci_size={} bits expected_al={}",
                     value.cqi,
                     value.cqi_table,
                     1U << static_cast<uint8_t>(value.min_aggr_level),
                     1U << static_cast<uint8_t>(value.max_aggr_level),
                     fmt::join(value.pdcch_candidates.begin(), value.pdcch_candidates.end(), ","),
                     value.dci_size_in_bits,
                     1U << static_cast<uint8_t>(value.expected_aggr_level));
}

class pdcch_aggregation_level_calculation_params_tester
  : public ::testing::TestWithParam<pdcch_aggregation_level_calculation_test_params>
{
protected:
  pdcch_aggregation_level_calculation_params_tester() : params(GetParam()) {}

  pdcch_aggregation_level_calculation_test_params params;
};

TEST_P(pdcch_aggregation_level_calculation_params_tester, successful_computation_of_aggregation_level)
{
  const auto computed_aggr_lvl = map_cqi_to_aggregation_level(params.cqi,
                                                              params.cqi_table,
                                                              params.min_aggr_level,
                                                              params.max_aggr_level,
                                                              params.pdcch_candidates,
                                                              params.dci_size_in_bits);
  ASSERT_TRUE(computed_aggr_lvl.has_value());
  ASSERT_TRUE(params.expected_aggr_level == computed_aggr_lvl)
      << fmt::format("Expected aggregation level={} does not match computed aggregation level={}",
                     1U << static_cast<uint8_t>(params.expected_aggr_level),
                     1U << static_cast<uint8_t>(computed_aggr_lvl.value()));
}

INSTANTIATE_TEST_SUITE_P(
    pdcch_aggregation_level_calculation_params_tester,
    pdcch_aggregation_level_calculation_params_tester,
    testing::Values(pdcch_aggregation_level_calculation_test_params{.cqi                 = 2,
                                                                    .cqi_table           = cqi_table_t::table1,
                                                                    .min_aggr_level      = aggregation_level::n1,
                                                                    .max_aggr_level      = aggregation_level::n16,
                                                                    .pdcch_candidates    = {0, 0, 1, 1, 1},
                                                                    .dci_size_in_bits    = 41,
                                                                    .expected_aggr_level = aggregation_level::n4},
                    pdcch_aggregation_level_calculation_test_params{.cqi                 = 8,
                                                                    .cqi_table           = cqi_table_t::table2,
                                                                    .min_aggr_level      = aggregation_level::n1,
                                                                    .max_aggr_level      = aggregation_level::n16,
                                                                    .pdcch_candidates    = {0, 1, 1, 1, 1},
                                                                    .dci_size_in_bits    = 100,
                                                                    .expected_aggr_level = aggregation_level::n2},
                    pdcch_aggregation_level_calculation_test_params{.cqi                 = 6,
                                                                    .cqi_table           = cqi_table_t::table3,
                                                                    .min_aggr_level      = aggregation_level::n1,
                                                                    .max_aggr_level      = aggregation_level::n16,
                                                                    .pdcch_candidates    = {0, 1, 1, 1, 1},
                                                                    .dci_size_in_bits    = 100,
                                                                    .expected_aggr_level = aggregation_level::n4}));
