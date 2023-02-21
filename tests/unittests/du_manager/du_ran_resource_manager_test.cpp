/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/du_manager/ran_resource_management/du_ran_resource_manager_impl.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

struct params {
  duplex_mode duplx_mode;
};

class du_ran_resource_manager_tester : public ::testing::TestWithParam<params>
{
protected:
  du_ran_resource_manager_tester(const cell_config_builder_params& params =
                                     {.dl_arfcn = GetParam().duplx_mode == duplex_mode::FDD ? 365000U : 520002U}) :
    cell_cfg_list({config_helpers::make_default_du_cell_config(params)}),
    qos_cfg_list(config_helpers::make_default_du_qos_config_list()),
    default_ue_cell_cfg(config_helpers::create_default_initial_ue_serving_cell_config(params)),
    res_mng(std::make_unique<du_ran_resource_manager_impl>(cell_cfg_list, qos_cfg_list))
  {
  }

  ue_ran_resource_configurator& create_ue(du_ue_index_t ue_index)
  {
    ues.emplace(ue_index, res_mng->create_ue_resource_configurator(ue_index, to_du_cell_index(0)));
    return ues[ue_index];
  }

  static f1ap_ue_context_update_request srb1_creation_req(du_ue_index_t ue_index)
  {
    f1ap_ue_context_update_request req;
    req.ue_index = ue_index;
    req.srbs_to_setup.resize(1);
    req.srbs_to_setup[0] = srb_id_t::srb1;
    return req;
  }

  unsigned get_config_sr_period() const
  {
    return sr_periodicity_to_slot(default_ue_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].period);
  }

  std::vector<du_cell_config>                                 cell_cfg_list;
  std::map<uint8_t, du_qos_config>                            qos_cfg_list;
  const serving_cell_config                                   default_ue_cell_cfg;
  std::unique_ptr<du_ran_resource_manager>                    res_mng;
  slotted_array<ue_ran_resource_configurator, MAX_NOF_DU_UES> ues;
};

TEST_P(du_ran_resource_manager_tester, when_ue_resource_config_is_created_then_pcell_is_configured)
{
  du_ue_index_t                 ue_idx1 = to_du_ue_index(0);
  ue_ran_resource_configurator& ue_res  = create_ue(ue_idx1);

  ASSERT_FALSE(ue_res.empty());
  ASSERT_EQ(ue_res->cells.size(), 1);
  ASSERT_TRUE(ue_res->cells.contains(0));
  ASSERT_TRUE(ue_res->rlc_bearers.empty());
  ASSERT_EQ(ue_res->cells[0].serv_cell_cfg.cell_index, to_du_cell_index(0));
  ASSERT_EQ(ue_res->cells[0].serv_cell_idx, SERVING_CELL_PCELL_IDX);
  ASSERT_FALSE(ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list.empty());
  ASSERT_FALSE(ue_res->mcg_cfg.scheduling_request_config.empty());
}

TEST_P(du_ran_resource_manager_tester, when_srb1_is_added_then_ue_resource_config_is_updated)
{
  du_ue_index_t                 ue_idx1 = to_du_ue_index(0);
  ue_ran_resource_configurator& ue_res  = create_ue(ue_idx1);
  auto                          resp    = ue_res.update(to_du_cell_index(0), srb1_creation_req(ue_idx1));

  ASSERT_FALSE(resp.release_required);
  ASSERT_TRUE(resp.failed_srbs.empty());
  ASSERT_EQ(ue_res->rlc_bearers.size(), 1);
  ASSERT_EQ(ue_res->rlc_bearers[0].lcid, srsran::LCID_SRB1);
  ASSERT_EQ(ue_res->rlc_bearers[0].rlc_cfg.mode, rlc_mode::am);
}

TEST_P(du_ran_resource_manager_tester, when_multiple_ues_are_created_then_they_use_different_sr_offsets)
{
  unsigned sr_period            = get_config_sr_period();
  unsigned slots_per_frame      = NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(cell_cfg_list[0].scs_common);
  unsigned nof_avail_sr_offsets = sr_period;
  if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
    nof_avail_sr_offsets = 0;
    for (unsigned i = 0; i != sr_period; ++i) {
      if (has_active_tdd_ul_symbols(*cell_cfg_list[0].tdd_ul_dl_cfg_common, i % slots_per_frame)) {
        nof_avail_sr_offsets++;
      }
    }
  }

  // > Created UEs have unique SR offsets.
  std::set<unsigned> sr_offsets;
  for (unsigned i = 0; i != nof_avail_sr_offsets; ++i) {
    ue_ran_resource_configurator& ue_res = create_ue(to_du_ue_index(i));
    ASSERT_FALSE(ue_res.empty());
    const auto& sr_res_list = ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list;
    ASSERT_FALSE(sr_res_list.empty());
    ASSERT_EQ(sr_periodicity_to_slot(sr_res_list[0].period), sr_period);
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      ASSERT_TRUE(
          has_active_tdd_ul_symbols(*cell_cfg_list[0].tdd_ul_dl_cfg_common, sr_res_list[0].offset % slots_per_frame));
    }
    ASSERT_EQ(sr_offsets.count(sr_res_list[0].offset), 0);
    sr_offsets.insert(sr_res_list[0].offset);
  }

  {
    // > No more SR offsets available. UE Resource Allocation fails.
    ue_ran_resource_configurator& empty_ue_res = create_ue(to_du_ue_index(sr_period));
    ASSERT_TRUE(empty_ue_res.empty());
    ues.erase(to_du_ue_index(sr_period));
  }

  // Removing one UE, should make one SR offset available.
  du_ue_index_t ue_idx_to_rem = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, nof_avail_sr_offsets - 1));
  unsigned      rem_sr_offset =
      ues[ue_idx_to_rem]->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset;
  ues.erase(ue_idx_to_rem);
  ue_ran_resource_configurator& ue_res = create_ue(to_du_ue_index(sr_period + 1));
  ASSERT_FALSE(ue_res.empty());
  ASSERT_EQ(rem_sr_offset, ue_res->cells[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset);
}

INSTANTIATE_TEST_SUITE_P(du_ran_resource_manager_tester,
                         du_ran_resource_manager_tester,
                         ::testing::Values(params{duplex_mode::FDD}, params{duplex_mode::TDD}));
