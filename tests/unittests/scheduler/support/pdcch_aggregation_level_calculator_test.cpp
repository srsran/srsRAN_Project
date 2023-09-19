/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "lib/scheduler/support/pdcch_aggregation_level_calculator.h"
#include "srsran/ran/csi_rs/csi_report_config.h"
#include <gtest/gtest.h>

using namespace srsran;

class pdcch_aggregation_level_calculation_corner_cases_tester : public ::testing::Test
{};

TEST_F(
    pdcch_aggregation_level_calculation_corner_cases_tester,
    returns_max_aggr_level_with_pdcch_candiates_if_no_pdcch_candidates_are_configured_for_computed_aggr_level_and_above)
{
  // PDCCH candidates configured only for aggregation level 1.
  const std::array<uint8_t, 5> pdcch_candidates = {4, 0, 0, 0, 0};
  // Force computed aggregation level to be 2 by considering DCI size in bits large enough to not fit in 1 CCE.
  const unsigned  dci_size_in_bits = 115;
  const cqi_value cqi              = 6;

  const auto aggr_lvl = map_cqi_to_aggregation_level(cqi, cqi_table_t::table1, pdcch_candidates, dci_size_in_bits);
  ASSERT_TRUE(aggr_lvl == srsran::aggregation_level::n1) << fmt::format("Aggregation level returned must be 1");
}

TEST_F(pdcch_aggregation_level_calculation_corner_cases_tester,
       returns_higher_aggregation_level_when_code_rate_exceeds_effective_code_rate_at_an_aggregation_level)
{
  // PDCCH candidates configured only for aggregation level 2 and 4.
  const std::array<uint8_t, 5> pdcch_candidates = {0, 4, 2, 0, 0};
  // Consider DCI size in bits so that code rate computed over aggregation level 2 exceeds allowed effective code
  // rate at given CQI (i.e. 4).
  const unsigned  dci_size_in_bits = 115;
  const cqi_value cqi              = 4;

  const auto aggr_lvl = map_cqi_to_aggregation_level(cqi, cqi_table_t::table1, pdcch_candidates, dci_size_in_bits);
  ASSERT_TRUE(aggr_lvl == srsran::aggregation_level::n4) << fmt::format("Computed Aggregation level must be 4");
}

struct pdcch_aggregation_level_calculation_test_params {
  cqi_value              cqi;
  cqi_table_t            cqi_table;
  std::array<uint8_t, 5> pdcch_candidates;
  unsigned               dci_size_in_bits;
  aggregation_level      expected_aggr_level;
};

/// Formatter for test params.
void PrintTo(const pdcch_aggregation_level_calculation_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("cqi={} cqi_table={} pdcch_candidates=[{}] dci_size={} bits expected_al={}",
                     value.cqi,
                     value.cqi_table,
                     fmt::join(value.pdcch_candidates.begin(), value.pdcch_candidates.end(), ","),
                     value.dci_size_in_bits,
                     to_nof_cces(value.expected_aggr_level));
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
  const auto computed_aggr_lvl =
      map_cqi_to_aggregation_level(params.cqi, params.cqi_table, params.pdcch_candidates, params.dci_size_in_bits);
  ASSERT_TRUE(params.expected_aggr_level == computed_aggr_lvl)
      << fmt::format("Expected aggregation level={} does not match computed aggregation level={}",
                     to_nof_cces(params.expected_aggr_level),
                     to_nof_cces(computed_aggr_lvl));
}

INSTANTIATE_TEST_SUITE_P(
    pdcch_aggregation_level_calculation_params_tester,
    pdcch_aggregation_level_calculation_params_tester,
    testing::Values(pdcch_aggregation_level_calculation_test_params{.cqi                 = 2,
                                                                    .cqi_table           = cqi_table_t::table1,
                                                                    .pdcch_candidates    = {0, 0, 1, 1, 1},
                                                                    .dci_size_in_bits    = 41,
                                                                    .expected_aggr_level = aggregation_level::n4},
                    pdcch_aggregation_level_calculation_test_params{.cqi                 = 8,
                                                                    .cqi_table           = cqi_table_t::table2,
                                                                    .pdcch_candidates    = {0, 1, 1, 1, 1},
                                                                    .dci_size_in_bits    = 100,
                                                                    .expected_aggr_level = aggregation_level::n2},
                    pdcch_aggregation_level_calculation_test_params{.cqi                 = 10,
                                                                    .cqi_table           = cqi_table_t::table3,
                                                                    .pdcch_candidates    = {0, 1, 1, 1, 1},
                                                                    .dci_size_in_bits    = 100,
                                                                    .expected_aggr_level = aggregation_level::n2}));
