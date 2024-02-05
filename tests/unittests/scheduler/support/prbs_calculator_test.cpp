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

#include "lib/scheduler/support/prbs_calculator.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>

using namespace srsran;

// Pair of prbs_calculator_test_entry and corresponding number of prbs.
struct prbs_calculator_test_entry {
  prbs_calculator_sch_config params;
  unsigned                   nof_prbs;
  unsigned                   tbs;
};

prbs_calculator_sch_config get_prb_calc_pdsch_config(unsigned        payload_bytes,
                                                     sch_mcs_index   mcs_index,
                                                     pdsch_mcs_table mcs_table     = srsran::pdsch_mcs_table::qam64,
                                                     unsigned        nof_symbols   = 12,
                                                     unsigned        nof_dmrs_prbs = 36,
                                                     unsigned        nof_oh_prb    = 0,
                                                     unsigned        nof_layers    = 1,
                                                     unsigned        tb_scaling    = 0)
{
  return {payload_bytes,
          nof_symbols,
          nof_dmrs_prbs,
          nof_oh_prb,
          pdsch_mcs_get_config(mcs_table, mcs_index),
          nof_layers,
          tb_scaling};
}

unsigned get_tbs_bytes(const prbs_calculator_sch_config& pdsch_cfg, unsigned nof_prbs)
{
  unsigned tbs_bits_lb = tbs_calculator_calculate(tbs_calculator_configuration{pdsch_cfg.nof_symb_sh,
                                                                               pdsch_cfg.nof_dmrs_prb,
                                                                               pdsch_cfg.nof_oh_prb,
                                                                               pdsch_cfg.mcs_descr,
                                                                               pdsch_cfg.nof_layers,
                                                                               pdsch_cfg.tb_scaling_field,
                                                                               nof_prbs});
  return tbs_bits_lb / 8;
}

using prb_calculator_tester_params = std::tuple<unsigned, unsigned>;

class prbs_calculator_tester : public ::testing::TestWithParam<prb_calculator_tester_params>
{};

TEST_P(prbs_calculator_tester, calculated_nof_prbs_is_upper_bound)
{
  prbs_calculator_sch_config pdsch_cfg =
      get_prb_calc_pdsch_config(std::get<0>(GetParam()), sch_mcs_index(std::get<1>(GetParam())));
  sch_prbs_tbs tbs_prb = get_nof_prbs(pdsch_cfg);

  unsigned tbs_lb = get_tbs_bytes(pdsch_cfg, tbs_prb.nof_prbs - 1);
  ASSERT_TRUE(tbs_lb < pdsch_cfg.payload_size_bytes) << fmt::format(
      "Lower bound nof_prb={},tbs={} is above payload={}", tbs_prb.nof_prbs - 1, tbs_lb, pdsch_cfg.payload_size_bytes);
  unsigned tbs_ub = get_tbs_bytes(pdsch_cfg, tbs_prb.nof_prbs);
  ASSERT_TRUE(tbs_ub >= pdsch_cfg.payload_size_bytes) << fmt::format(
      "Upper bound nof_prb={},tbs={} is below payload={}", tbs_prb.nof_prbs, tbs_ub, pdsch_cfg.payload_size_bytes);
}

INSTANTIATE_TEST_SUITE_P(low_tbs_prb_calculation,
                         prbs_calculator_tester,
                         testing::Combine(testing::Range<unsigned>(30, 440, 10), testing::Range<unsigned>(1, 25)),
                         [](const testing::TestParamInfo<prbs_calculator_tester::ParamType>& info_) {
                           return fmt::format(
                               "payload_{}_and_mcs_{}", std::get<0>(info_.param), std::get<1>(info_.param));
                         });

INSTANTIATE_TEST_SUITE_P(large_tbs_prb_calculation,
                         prbs_calculator_tester,
                         testing::Combine(testing::Range<unsigned>(450, 1000, 50), testing::Range<unsigned>(1, 25)),
                         [](const testing::TestParamInfo<prbs_calculator_tester::ParamType>& info_) {
                           return fmt::format(
                               "payload_{}_and_mcs_{}", std::get<0>(info_.param), std::get<1>(info_.param));
                         });

TEST(nof_prb_calculation, test_entries_match)
{
  // NOTE: payload size is given in bytes.
  static const std::vector<prbs_calculator_test_entry> prbs_calculator_test_table = {
      // clang-format off
      {{39,  12, 36, 0, {modulation_scheme::QPSK,  379}, 1, 0}, 4U,  40U},
      {{41,  12, 36, 0, {modulation_scheme::QPSK,  379}, 1, 0}, 5U,  51U},
      {{53,  12, 36, 0, {modulation_scheme::QPSK,  379}, 1, 0}, 6U,  60U},
      {{53,  12, 36, 0, {modulation_scheme::QAM16, 616}, 1, 0}, 2U,  66U},
      {{54,  12, 36, 0, {modulation_scheme::QAM16, 616}, 1, 0}, 2U,  66U},
      {{75,  12, 36, 0, {modulation_scheme::QAM16, 616}, 1, 0}, 3U,  101},
      {{102, 12, 36, 0, {modulation_scheme::QAM16, 616}, 1, 0}, 4U,  129U},
      {{133, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 3U,  177U},
      {{172, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 3U,  177U},
      {{209, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 4U,  233U},
      {{241, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 5U,  285U},
      {{275, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 5U,  285U},
      {{285, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 5U,  285U},
      {{303, 12, 36, 0, {modulation_scheme::QAM64, 719}, 1, 0}, 6U,  341U},
      // The parameters below correspond to the max SIB1 payload size (2976 bits, or 372 bytes).
      {{372, 12, 36, 0, {modulation_scheme::QPSK,  379}, 1, 0}, 37U, 372U},
      // This entry is the edge-case when TBS derivation stops being table-based (3824 bits, or 478 bytes).
      {{478, 12, 36, 0, {modulation_scheme::QPSK,  379}, 1, 0}, 48U, 480U},
      // clang-format on
  };

  // Run the test for all the values in the table.
  for (const auto& test_entry : prbs_calculator_test_table) {
    sch_prbs_tbs test_results = get_nof_prbs(test_entry.params);
    ASSERT_EQ(test_entry.nof_prbs, test_results.nof_prbs);
    ASSERT_EQ(test_entry.tbs, test_results.tbs_bytes);
  }
}

TEST(estimate_nof_prbs, distance_from_actual_solution)
{
  auto compute_nof_prb_estim_err = [](const std::vector<prbs_calculator_sch_config>& configs) {
    double tbs_diff = 0, tbs_abs = 0, tbs_max = 0;
    for (const auto& test_cfg : configs) {
      int    nof_prbs_estimate = estimate_required_nof_prbs(test_cfg);
      int    nof_prbs_solution = get_nof_prbs(test_cfg).nof_prbs;
      double diff              = nof_prbs_estimate - nof_prbs_solution;
      tbs_diff += diff;
      tbs_abs += std::abs(diff);
      if (std::abs(diff) > tbs_max) {
        tbs_max = diff;
      }
    }
    return std::make_tuple(tbs_diff / configs.size(), tbs_abs / configs.size(), tbs_max);
  };

  std::vector<prbs_calculator_sch_config> short_tbs_prbs_calc_configs, medium_tbs_prbs_calc_configs,
      long_tbs_prbs_calc_configs;
  for (unsigned mcs = 1; mcs < 25; ++mcs) {
    // build short TBS table.
    for (unsigned tbs = 30; tbs < 450; tbs += 5) {
      short_tbs_prbs_calc_configs.push_back(get_prb_calc_pdsch_config(tbs, sch_mcs_index(mcs)));
    }

    // build medium size TBS table.
    for (unsigned tbs = 450; tbs < 2000; tbs += 10) {
      medium_tbs_prbs_calc_configs.push_back(get_prb_calc_pdsch_config(tbs, sch_mcs_index(mcs)));
    }

    // build long TBS table.
    for (unsigned tbs = 2000; tbs < 100000; tbs += 100) {
      long_tbs_prbs_calc_configs.push_back(get_prb_calc_pdsch_config(tbs, sch_mcs_index(mcs)));
    }
  }

  // Compute estimation errors.
  auto   tup           = compute_nof_prb_estim_err(short_tbs_prbs_calc_configs);
  double short_tbs_err = std::get<0>(tup), short_tbs_err_abs = std::get<1>(tup), short_tbs_err_max = std::get<2>(tup);
  tup                   = compute_nof_prb_estim_err(medium_tbs_prbs_calc_configs);
  double medium_tbs_err = std::get<0>(tup), medium_tbs_err_abs = std::get<1>(tup),
         medium_tbs_err_max = std::get<2>(tup);
  tup                       = compute_nof_prb_estim_err(long_tbs_prbs_calc_configs);
  double long_tbs_err = std::get<0>(tup), long_tbs_err_abs = std::get<1>(tup), long_tbs_err_max = std::get<2>(tup);

  fmt::print("NofPRBs estimation error for short TBS: mean={}, abs={}, max={}\n",
             short_tbs_err,
             short_tbs_err_abs,
             short_tbs_err_max);
  fmt::print("NofPRBs estimation error for medium TBS: mean={}, abs={}, max={}\n",
             medium_tbs_err,
             medium_tbs_err_abs,
             medium_tbs_err_max);
  fmt::print("NofPRBs estimation error for large TBS: mean={}, abs={}, max={}\n",
             long_tbs_err,
             long_tbs_err_abs,
             long_tbs_err_max);

  ASSERT_LT(short_tbs_err, 1);
  ASSERT_LT(short_tbs_err_abs, 1);
  ASSERT_LT(medium_tbs_err, 1);
  ASSERT_LT(medium_tbs_err_abs, 1);
  //  ASSERT_LT(long_tbs_err, 1);
  //  ASSERT_LT(long_tbs_err_abs, 1);
}
