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

#include "../tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/ran/duplex_mode.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

class sched_pucch_res_builder_tester : public ::testing::TestWithParam<bool>
{
protected:
  sched_pucch_res_builder_tester() :
    cell_cfg(config_helpers::make_default_scheduler_expert_config(),
             test_helpers::make_default_sched_cell_configuration_request()),
    cell_cfg_dedicated(config_helpers::create_default_initial_ue_spcell_cell_config()),
    pucch_builder(GetParam() ? pucch_res_builder_test_helper(cell_cfg.ul_cfg_common.init_ul_bwp, std::nullopt)
                             : pucch_res_builder_test_helper())
  {
    if (not GetParam()) {
      pucch_builder.setup(cell_cfg.ul_cfg_common.init_ul_bwp, std::nullopt);
    }
  }

  struct ue_info {
    unsigned            ue_idx;
    serving_cell_config serv_cell_cfg;
  };

  const ue_info* add_ue()
  {
    ues.push_back(ue_info{ue_cnt++, config_helpers::create_default_initial_ue_spcell_cell_config().serv_cell_cfg});
    pucch_builder.add_build_new_ue_pucch_cfg(ues.back().serv_cell_cfg);
    return &ues.back();
  }

  cell_configuration            cell_cfg;
  cell_config_dedicated         cell_cfg_dedicated;
  pucch_builder_params          pucch_params;
  std::vector<ue_info>          ues;
  unsigned                      ue_cnt = 0;
  pucch_res_builder_test_helper pucch_builder;
};

TEST_P(sched_pucch_res_builder_tester, when_ues_are_added_their_cfg_have_different_csi_and_sr)
{
  std::set<std::pair<unsigned, unsigned>> sr_offsets;
  std::set<std::pair<unsigned, unsigned>> csi_offsets;
  const unsigned                          nof_ues = 20;
  for (unsigned i = 0; i != nof_ues; ++i) {
    const ue_info* ue = add_ue();

    ASSERT_NE(ue, nullptr);

    // Check that the SR is configured and all UEs have different SR offsets or PUCCH res id.
    const auto& sr_res_list = ue->serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    ASSERT_FALSE(sr_res_list.empty());
    auto ue_sr_res_offset_pair = std::make_pair(sr_res_list[0].pucch_res_id.cell_res_id, sr_res_list[0].offset);
    ASSERT_EQ(sr_offsets.count(ue_sr_res_offset_pair), 0);
    sr_offsets.insert(ue_sr_res_offset_pair);

    if (cell_cfg_dedicated.serv_cell_cfg.csi_meas_cfg.has_value()) {
      // Check that the CSI is configured and all UEs have different CSI offsets or PUCCH res id.
      const bool has_csi_cfg = ue->serv_cell_cfg.csi_meas_cfg.has_value() and
                               not ue->serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.empty();
      ASSERT_TRUE(has_csi_cfg);
      const auto& csi_res_cfg = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
          ue->serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.front().report_cfg_type);
      auto ue_csi_res_offset_pair = std::make_pair(csi_res_cfg.pucch_csi_res_list.front().pucch_res_id.cell_res_id,
                                                   csi_res_cfg.report_slot_offset);
      ASSERT_EQ(csi_offsets.count(ue_csi_res_offset_pair), 0);
      csi_offsets.insert(ue_csi_res_offset_pair);
    }

    const auto& ue_pucch_cfg = ue->serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();
    // Each UE should have 2 PUCCH resource sets configured
    ASSERT_EQ(ue_pucch_cfg.pucch_res_set.size(), 2);
    ASSERT_EQ(ue_pucch_cfg.pucch_res_set[0].pucch_res_id_list.size(), pucch_params.nof_ue_pucch_f0_or_f1_res_harq);
    ASSERT_EQ(ue_pucch_cfg.pucch_res_set[1].pucch_res_id_list.size(), pucch_params.nof_ue_pucch_f2_res_harq);
    // Make sure UE has all PUCCH resources with different cell_res_id.
    {
      std::set<unsigned> pucch_res_idxs;
      for (unsigned n = 0; n != ue_pucch_cfg.pucch_res_set[0].pucch_res_id_list.size(); ++n) {
        pucch_res_idxs.count(ue_pucch_cfg.pucch_res_list[n].res_id.cell_res_id);
        pucch_res_idxs.insert(ue_pucch_cfg.pucch_res_list[n].res_id.cell_res_id);
      }
    }
  }

  ASSERT_TRUE(true);
}

INSTANTIATE_TEST_SUITE_P(test_helper_inited_with_ctor_vs_setup,
                         sched_pucch_res_builder_tester,
                         ::testing::Values(true, false));
