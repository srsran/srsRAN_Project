/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/support/dmrs_helpers.h"
#include "lib/scheduler/support/mcs_tbs_calculator.h"
#include "lib/scheduler/support/sch_pdu_builder.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

///////////////       HELPER FUNCTIONS         ///////////////

// Contains the input test parameters.
struct mcs_test_entry {
  // Outputs.
  unsigned final_mcs;
  unsigned tbs_bytes;
  // Inputs.
  unsigned max_mcs;
  unsigned nof_prbs;
};

class common_mcs_tbs_calculator_test
{
public:
  const scheduler_expert_config                  expert_cfg = config_helpers::make_default_scheduler_expert_config();
  const sched_cell_configuration_request_message cell_req =
      sched_config_helper::make_default_sched_cell_configuration_request();
  const serving_cell_config   serv_cell_cfg = config_helpers::create_default_initial_ue_serving_cell_config();
  du_cell_config_pool         cell_cfg_pool{cell_req};
  const cell_configuration    cell_cfg{expert_cfg, cell_req};
  const ue_cell_configuration ue_cell_cfg{to_rnti(0x4601), cell_cfg, cell_cfg_pool.update_ue(serv_cell_cfg)};
};

///////////////       DL TEST         ///////////////

class dl_mcs_tbs_calculator_test_bench : public common_mcs_tbs_calculator_test,
                                         public ::testing::TestWithParam<mcs_test_entry>
{
public:
  dl_mcs_tbs_calculator_test_bench() :
    pdsch_cfg(sched_helper::get_pdsch_config_f1_0_c_rnti(
        cell_cfg,
        ue_cell_cfg.pdsch_serving_cell_cfg(),
        cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource]))
  {
  }

protected:
  unsigned            time_resource{0};
  pdsch_config_params pdsch_cfg;
};

TEST_P(dl_mcs_tbs_calculator_test_bench, test_values)
{
  mcs_test_entry test_entry{.max_mcs = GetParam().max_mcs, .nof_prbs = GetParam().nof_prbs};

  // Run test function.
  std::optional<sch_mcs_tbs> test =
      compute_dl_mcs_tbs(pdsch_cfg, sch_mcs_index(test_entry.max_mcs), test_entry.nof_prbs, false);

  ASSERT_TRUE(test.has_value());
  ASSERT_EQ(GetParam().final_mcs, test.value().mcs);
  ASSERT_EQ(GetParam().tbs_bytes, test.value().tbs);
}

INSTANTIATE_TEST_SUITE_P(
    manual_test_for_max_mcs_and_prbs,
    dl_mcs_tbs_calculator_test_bench,
    testing::Values(mcs_test_entry{.final_mcs = 28, .tbs_bytes = 372, .max_mcs = 28, .nof_prbs = 5},
                    mcs_test_entry{.final_mcs = 28, .tbs_bytes = 1505, .max_mcs = 28, .nof_prbs = 20},
                    mcs_test_entry{.final_mcs = 26, .tbs_bytes = 7298, .max_mcs = 26, .nof_prbs = 106},
                    mcs_test_entry{.final_mcs = 23, .tbs_bytes = 177, .max_mcs = 23, .nof_prbs = 3},
                    mcs_test_entry{.final_mcs = 19, .tbs_bytes = 11272, .max_mcs = 19, .nof_prbs = 275},
                    mcs_test_entry{.final_mcs = 13, .tbs_bytes = 816, .max_mcs = 13, .nof_prbs = 32},
                    mcs_test_entry{.final_mcs = 11, .tbs_bytes = 816, .max_mcs = 11, .nof_prbs = 41},
                    mcs_test_entry{.final_mcs = 8, .tbs_bytes = 2817, .max_mcs = 8, .nof_prbs = 179},
                    mcs_test_entry{.final_mcs = 3, .tbs_bytes = 561, .max_mcs = 3, .nof_prbs = 84},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 177, .max_mcs = 0, .nof_prbs = 55},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 15, .max_mcs = 0, .nof_prbs = 5}));

///////////////       UL TEST         ///////////////

class ul_mcs_tbs_prbs_calculator_test_bench : public common_mcs_tbs_calculator_test,
                                              public ::testing::TestWithParam<mcs_test_entry>
{
public:
  ul_mcs_tbs_prbs_calculator_test_bench() :
    pusch_cfg(get_pusch_config_f0_0_tc_rnti(
        cell_cfg,
        cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list[time_resource]))
  {
  }

protected:
  unsigned            time_resource{0};
  pusch_config_params pusch_cfg;
};

TEST_P(ul_mcs_tbs_prbs_calculator_test_bench, test_values)
{
  mcs_test_entry test_entry{.max_mcs = GetParam().max_mcs, .nof_prbs = GetParam().nof_prbs};

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_entry.max_mcs), test_entry.nof_prbs, false);

  ASSERT_TRUE(test.has_value());
  ASSERT_EQ(GetParam().final_mcs, test.value().mcs);
  ASSERT_EQ(GetParam().tbs_bytes, test.value().tbs);
}

INSTANTIATE_TEST_SUITE_P(
    manual_test_for_max_mcs_and_prbs,
    ul_mcs_tbs_prbs_calculator_test_bench,
    testing::Values(mcs_test_entry{.final_mcs = 27, .tbs_bytes = 437, .max_mcs = 28, .nof_prbs = 5},
                    mcs_test_entry{.final_mcs = 28, .tbs_bytes = 1825, .max_mcs = 28, .nof_prbs = 20},
                    mcs_test_entry{.final_mcs = 26, .tbs_bytes = 8961, .max_mcs = 26, .nof_prbs = 106},
                    mcs_test_entry{.final_mcs = 23, .tbs_bytes = 209, .max_mcs = 23, .nof_prbs = 3},
                    mcs_test_entry{.final_mcs = 19, .tbs_bytes = 13829, .max_mcs = 19, .nof_prbs = 275},
                    mcs_test_entry{.final_mcs = 13, .tbs_bytes = 1008, .max_mcs = 13, .nof_prbs = 32},
                    mcs_test_entry{.final_mcs = 11, .tbs_bytes = 992, .max_mcs = 11, .nof_prbs = 41},
                    mcs_test_entry{.final_mcs = 8, .tbs_bytes = 3457, .max_mcs = 8, .nof_prbs = 179},
                    mcs_test_entry{.final_mcs = 3, .tbs_bytes = 673, .max_mcs = 3, .nof_prbs = 84},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 217, .max_mcs = 0, .nof_prbs = 55},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 19, .max_mcs = 0, .nof_prbs = 5},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 3, .max_mcs = 0, .nof_prbs = 1}));

class ul_mcs_tbs_prbs_calculator_dci_0_1_test_bench : public common_mcs_tbs_calculator_test,
                                                      public ::testing::TestWithParam<mcs_test_entry>
{
public:
  ul_mcs_tbs_prbs_calculator_dci_0_1_test_bench() :
    pusch_cfg(get_pusch_config_f0_1_c_rnti(
        ue_cell_cfg,
        cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list[time_resource],
        1,
        7,
        true))
  {
  }

protected:
  unsigned            time_resource{0};
  pusch_config_params pusch_cfg;
};

TEST_P(ul_mcs_tbs_prbs_calculator_dci_0_1_test_bench, test_values_with_uci)
{
  mcs_test_entry test_entry{.max_mcs = GetParam().max_mcs, .nof_prbs = GetParam().nof_prbs};

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_entry.max_mcs), test_entry.nof_prbs, false);

  ASSERT_TRUE(test.has_value());
  ASSERT_EQ(GetParam().final_mcs, test.value().mcs);
  ASSERT_EQ(GetParam().tbs_bytes, test.value().tbs);
}

INSTANTIATE_TEST_SUITE_P(
    manual_test_for_max_mcs_and_prbs,
    ul_mcs_tbs_prbs_calculator_dci_0_1_test_bench,
    testing::Values(mcs_test_entry{.final_mcs = 27, .tbs_bytes = 437, .max_mcs = 28, .nof_prbs = 5},
                    mcs_test_entry{.final_mcs = 28, .tbs_bytes = 1825, .max_mcs = 28, .nof_prbs = 20},
                    mcs_test_entry{.final_mcs = 26, .tbs_bytes = 8961, .max_mcs = 26, .nof_prbs = 106},
                    mcs_test_entry{.final_mcs = 23, .tbs_bytes = 209, .max_mcs = 23, .nof_prbs = 3},
                    mcs_test_entry{.final_mcs = 19, .tbs_bytes = 13829, .max_mcs = 19, .nof_prbs = 275},
                    mcs_test_entry{.final_mcs = 13, .tbs_bytes = 1008, .max_mcs = 13, .nof_prbs = 32},
                    mcs_test_entry{.final_mcs = 11, .tbs_bytes = 992, .max_mcs = 11, .nof_prbs = 41},
                    mcs_test_entry{.final_mcs = 8, .tbs_bytes = 3457, .max_mcs = 8, .nof_prbs = 179},
                    mcs_test_entry{.final_mcs = 3, .tbs_bytes = 673, .max_mcs = 3, .nof_prbs = 84},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 217, .max_mcs = 0, .nof_prbs = 55},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 19, .max_mcs = 0, .nof_prbs = 5},
                    mcs_test_entry{.final_mcs = 0, .tbs_bytes = 7, .max_mcs = 0, .nof_prbs = 2}));

class ul_mcs_tbs_prbs_calculator_low_mcs_test_bench : public common_mcs_tbs_calculator_test, public ::testing::Test
{
public:
  ul_mcs_tbs_prbs_calculator_low_mcs_test_bench() :
    pusch_cfg(get_pusch_config_f0_1_c_rnti(
        ue_cell_cfg,
        cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list[time_resource],
        1,
        7,
        true))
  {
  }

protected:
  unsigned            time_resource{0};
  pusch_config_params pusch_cfg;
};

TEST_F(ul_mcs_tbs_prbs_calculator_low_mcs_test_bench, test_values_with_uci)
{
  // NOTE: The mim MCS that allows allocation of 1 PRB (with positive effective code rate) depends on the configuration.
  // For different configuration, the test would need to be repeted for different MCS values.

  // Verify the MCS 5 and 1 PRB do yield to a valid MCS-PRB allocation.
  mcs_test_entry test_1_prb{.final_mcs = 5, .tbs_bytes = 12, .max_mcs = 5, .nof_prbs = 1};

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_1_prb.max_mcs), test_1_prb.nof_prbs, false);

  ASSERT_TRUE(test.has_value()) << to_string(test.error());
  ASSERT_EQ(test_1_prb.final_mcs, test.value().mcs);
  ASSERT_EQ(test_1_prb.tbs_bytes, test.value().tbs);

  // Verify the MCS < 5 and 1 PRB do NOT yield to a valid MCS-PRB allocation.
  test_1_prb.max_mcs = 4;

  // Run test function.
  test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_1_prb.max_mcs), test_1_prb.nof_prbs, false);

  ASSERT_FALSE(test.has_value());

  // Verify the MCS < 5 and 2 PRBs yield to a valid MCS-PRB allocation.
  mcs_test_entry test_2_prb{.final_mcs = 4, .tbs_bytes = 19, .max_mcs = 4, .nof_prbs = 2};

  // Run test function.
  test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_2_prb.max_mcs), test_2_prb.nof_prbs, false);
  ASSERT_TRUE(test.has_value());
  ASSERT_EQ(test_2_prb.final_mcs, test.value().mcs);
  ASSERT_EQ(test_2_prb.tbs_bytes, test.value().tbs);

  // Verify that, even in the worst case of MCS 0, 2 PRBs yield to a valid MCS-PRB allocation.
  mcs_test_entry test_2_prb_mcs_0{.final_mcs = 0, .tbs_bytes = 7, .max_mcs = 0, .nof_prbs = 2};

  // Run test function.
  test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_2_prb_mcs_0.max_mcs), test_2_prb_mcs_0.nof_prbs, false);
  ASSERT_TRUE(test.has_value());
  ASSERT_EQ(test_2_prb_mcs_0.final_mcs, test.value().mcs);
  ASSERT_EQ(test_2_prb_mcs_0.tbs_bytes, test.value().tbs);
}

class ul_mcs_tbs_prbs_calculator_with_harq_ack : public common_mcs_tbs_calculator_test, public ::testing::Test
{
public:
  ul_mcs_tbs_prbs_calculator_with_harq_ack() :
    pusch_cfg(get_pusch_config_f0_1_c_rnti(
        ue_cell_cfg,
        cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list[time_resource],
        1,
        2,
        false))
  {
  }

protected:
  unsigned            time_resource{0};
  pusch_config_params pusch_cfg;
};

TEST_F(ul_mcs_tbs_prbs_calculator_with_harq_ack, test_values_with_2_harq_bits)
{
  mcs_test_entry test_1_prb{.final_mcs = 27, .tbs_bytes = 88, .max_mcs = 28, .nof_prbs = 1};

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test = compute_ul_mcs_tbs(
      pusch_cfg, ue_cell_cfg.init_bwp(), sch_mcs_index(test_1_prb.max_mcs), test_1_prb.nof_prbs, false);

  ASSERT_TRUE(test.has_value());
  ASSERT_EQ(test_1_prb.final_mcs, test.value().mcs);
  ASSERT_EQ(test_1_prb.tbs_bytes, test.value().tbs);
}

class ul_mcs_tbs_prbs_calculator_error : public common_mcs_tbs_calculator_test, public ::testing::Test
{
public:
  ul_mcs_tbs_prbs_calculator_error() :
    pusch_cfg(get_pusch_config_f0_1_c_rnti(
        ue_cell_cfg,
        cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list[time_resource],
        1,
        0,
        false))
  {
  }

protected:
  static constexpr unsigned nof_prbs    = 1;
  static constexpr unsigned max_mcs     = 0;
  static constexpr bool     contains_dc = true;
  static constexpr unsigned time_resource{0};
  pusch_config_params       pusch_cfg;
};

TEST_F(ul_mcs_tbs_prbs_calculator_error, effective_code_rate_exceeds_maximum)
{
  pusch_cfg.nof_csi_part1_bits = 11;
  pusch_cfg.nof_harq_ack_bits  = 2;

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test =
      compute_ul_mcs_tbs(pusch_cfg, ue_cell_cfg.init_bwp(), max_mcs, nof_prbs, contains_dc);

  ASSERT_FALSE(test.has_value());
  ASSERT_EQ(test.error(), compute_ul_mcs_tbs_error::effective_code_rate_exceeds_maximum) << to_string(test.error());
}

TEST_F(ul_mcs_tbs_prbs_calculator_error, two_bit_harq_ack_and_dc_overhead)
{
  bwp_config active_bwp_cfg = ue_cell_cfg.init_bwp();
  std::get<uci_on_pusch::beta_offsets_semi_static>(
      active_bwp_cfg.ul_ded.value().pusch_cfg.value().uci_cfg.value().beta_offsets_cfg.value())
      .beta_offset_ack_idx_1 = 15;

  pusch_cfg.nof_csi_part1_bits = 11;
  pusch_cfg.nof_harq_ack_bits  = 2;

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test =
      compute_ul_mcs_tbs(pusch_cfg, active_bwp_cfg, max_mcs, nof_prbs, contains_dc);

  ASSERT_FALSE(test.has_value());
  ASSERT_EQ(test.error(), compute_ul_mcs_tbs_error::two_bit_harq_ack_and_dc_overhead) << to_string(test.error());
}

TEST_F(ul_mcs_tbs_prbs_calculator_error, invalid_ulsch_information)
{
  pusch_cfg.nof_csi_part1_bits = 8;
  pusch_cfg.nof_harq_ack_bits  = 4;

  // Run test function.
  expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> test =
      compute_ul_mcs_tbs(pusch_cfg, ue_cell_cfg.init_bwp(), max_mcs, nof_prbs, false);

  ASSERT_FALSE(test.has_value());
  ASSERT_EQ(test.error(), compute_ul_mcs_tbs_error::invalid_ulsch_information) << to_string(test.error());
}
