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

#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/slicing/slice_ue_repository.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

struct test_lc_ch_cfg {
  lcid_t         lcid;
  lcg_id_t       lcg_id;
  ran_slice_id_t slice_id;
};

ul_bsr_indication_message create_short_bsr(du_ue_index_t ue_idx, ul_bsr_lcg_report_list report)
{
  return ul_bsr_indication_message{
      to_du_cell_index(0), ue_idx, to_rnti(0x4601 + ue_idx), bsr_format::SHORT_BSR, report};
}

} // namespace

class slice_ue_repository_test : public ::testing::Test
{
protected:
  slice_ue_repository_test() :
    test_cfg(
        []() {
          cell_config_builder_params params{};
          params.scs_common     = subcarrier_spacing::kHz30;
          params.channel_bw_mhz = bs_channel_bandwidth::MHz100;
          params.dl_f_ref_arfcn = 520000;
          params.band           = nr_band::n41;
          return params;
        }(),
        scheduler_expert_config{})
  {
    srslog::init();

    auto req = test_cfg.get_default_cell_config_request();
    test_cfg.add_cell(req);

    next_slot = {to_numerology_value(req.scs_common), 0};

    ue_db.add_cell(to_du_cell_index(0));

    slices.push_back(std::make_unique<slice_ue_repository>(SRB_RAN_SLICE_ID, du_cell_index_t(0)));
    slices.push_back(std::make_unique<slice_ue_repository>(DEFAULT_DRB_RAN_SLICE_ID, du_cell_index_t(0)));
    slices.push_back(std::make_unique<slice_ue_repository>(ran_slice_id_t{2}, du_cell_index_t(0)));
  }

  void add_ue(du_ue_index_t ue_idx, span<const test_lc_ch_cfg> lc_chs)
  {
    auto req               = test_cfg.get_default_ue_config_request();
    req.ue_index           = ue_idx;
    req.crnti              = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback = false;
    std::vector<logical_channel_config> lc_cfg_list;
    lc_cfg_list.reserve(lc_chs.size() + 1);
    lc_cfg_list.push_back(logical_channel_config{LCID_SRB0, uint_to_lcg_id(0)});
    for (const auto& lc_ch : lc_chs) {
      lc_cfg_list.push_back(logical_channel_config{lc_ch.lcid, lc_ch.lcg_id});
    }
    req.cfg.lc_config_list         = lc_cfg_list;
    const ue_configuration* ue_cfg = test_cfg.add_ue(req);
    std::unique_ptr<ue>     u      = std::make_unique<ue>(ue_creation_command{*ue_cfg, false, cell_harqs});
    ue_db.add_ue(std::move(u));

    for (const auto& lc_ch : lc_chs) {
      slices[lc_ch.slice_id.value()]->add_logical_channel(ue_db[ue_idx], lc_ch.lcid, lc_ch.lcg_id);
    }
  }

  // add_ue that automatically adds UE to all slices.
  void add_ue(du_ue_index_t ue_idx)
  {
    std::vector<test_lc_ch_cfg> lc_chs = {
        test_lc_ch_cfg{LCID_SRB1, uint_to_lcg_id(0), SRB_RAN_SLICE_ID},
        test_lc_ch_cfg{LCID_MIN_DRB, uint_to_lcg_id(1), DEFAULT_DRB_RAN_SLICE_ID},
        test_lc_ch_cfg{uint_to_lcid(LCID_MIN_DRB + 1), uint_to_lcg_id(2), ran_slice_id_t{2}}};
    add_ue(ue_idx, lc_chs);
  }

  srslog::basic_logger&                             logger = srslog::fetch_basic_logger("TEST");
  test_helpers::test_sched_config_manager           test_cfg;
  cell_harq_manager                                 cell_harqs{MAX_NOF_DU_UES,
                               MAX_NOF_HARQS,
                               std::make_unique<scheduler_harq_timeout_dummy_notifier>()};
  ue_repository                                     ue_db;
  std::vector<std::unique_ptr<slice_ue_repository>> slices;

  slot_point next_slot;
};

TEST_F(slice_ue_repository_test, add_ue_to_slice_repo_alters_repo_size)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);

  ASSERT_TRUE(slices[SRB_RAN_SLICE_ID.value()]->empty());
  ASSERT_EQ(slices[SRB_RAN_SLICE_ID.value()]->size(), 0);
  ASSERT_FALSE(slices[SRB_RAN_SLICE_ID.value()]->contains(ue_idx));
  add_ue(ue_idx);
  ASSERT_FALSE(slices[SRB_RAN_SLICE_ID.value()]->empty());
  ASSERT_EQ(slices[SRB_RAN_SLICE_ID.value()]->size(), 1);
  ASSERT_TRUE(slices[SRB_RAN_SLICE_ID.value()]->contains(ue_idx));
}

TEST_F(slice_ue_repository_test, if_positive_dl_bs_added_to_slice_then_slice_ue_has_pending_bytes)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  auto& ue_srb_slice = (*slices[SRB_RAN_SLICE_ID.value()])[ue_idx];

  ASSERT_EQ(ue_srb_slice.pending_dl_newtx_bytes(), 0);
  const unsigned bs_val = 1000;
  ue_db[ue_idx].handle_dl_buffer_state_indication(LCID_SRB1, bs_val);
  ASSERT_GE(ue_srb_slice.pending_dl_newtx_bytes(), bs_val);
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_dl_newtx_bytes(), 0);
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_dl_newtx_bytes(), 0);
}

TEST_F(slice_ue_repository_test, if_positive_bsr_added_to_slice_then_slice_ue_has_pending_bytes)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  auto& ue_srb_slice = (*slices[SRB_RAN_SLICE_ID.value()])[ue_idx];

  ASSERT_EQ(ue_srb_slice.pending_ul_newtx_bytes(), 0);
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{0}, bsr_val}}));
  ASSERT_GE(ue_srb_slice.pending_ul_newtx_bytes(), 1000);
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
}

TEST_F(slice_ue_repository_test, if_sr_detected_and_no_pending_bsr_data_then_pending_data_is_positive_for_srb_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);

  ue_db[ue_idx].handle_sr_indication();
  ASSERT_GT((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
}

TEST_F(slice_ue_repository_test,
       if_sr_detected_and_there_is_pending_bsr_data_in_other_slices_then_pending_data_is_not_positive_for_srb_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));

  ue_db[ue_idx].handle_sr_indication();
  ASSERT_EQ((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
  ASSERT_GE((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), bsr_val);
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
}

TEST_F(
    slice_ue_repository_test,
    if_sr_detected_and_there_is_no_pending_bsr_data_in_other_slices_due_to_allocated_harqs_then_pending_data_is_positive_for_srb_slice)
{
  du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(ue_idx);
  const unsigned bsr_val = 1000;
  ue_db[ue_idx].handle_bsr_indication(create_short_bsr(ue_idx, ul_bsr_lcg_report_list{{lcg_id_t{1}, bsr_val}}));
  auto                  h_ul = ue_db[ue_idx].get_pcell().harqs.alloc_ul_harq(next_slot, 4);
  ul_harq_alloc_context ctxt;
  ctxt.dci_cfg_type = dci_ul_rnti_config_type::c_rnti_f0_1;
  ctxt.slice_id     = DEFAULT_DRB_RAN_SLICE_ID;
  pusch_information pusch;
  pusch.rnti          = to_rnti(0x4601 + ue_idx);
  pusch.harq_id       = h_ul->id();
  pusch.new_data      = true;
  pusch.tb_size_bytes = 2000;
  h_ul->save_grant_params(ctxt, pusch);

  ue_db[ue_idx].handle_sr_indication();
  ASSERT_GT((*slices[SRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
  ASSERT_EQ((*slices[DEFAULT_DRB_RAN_SLICE_ID.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
  ASSERT_EQ((*slices[ran_slice_id_t{2}.value()])[ue_idx].pending_ul_newtx_bytes(), 0);
}
