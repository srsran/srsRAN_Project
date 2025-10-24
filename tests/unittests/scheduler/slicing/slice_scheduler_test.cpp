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

#include "lib/scheduler/slicing/inter_slice_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;

static constexpr logical_channel_config create_logical_channel_config(lcid_t lcid, rrm_policy_member policy = {})
{
  logical_channel_config lc_ch = config_helpers::create_default_logical_channel_config(lcid);
  lc_ch.rrm_policy             = policy;
  return lc_ch;
}

class slice_scheduler_test
{
protected:
  slice_scheduler_test(const std::vector<slice_rrm_policy_config>& rrm_policy_members = {}) :
    test_cfg(
        []() {
          cell_config_builder_params params{};
          params.scs_common     = subcarrier_spacing::kHz30;
          params.channel_bw_mhz = bs_channel_bandwidth::MHz100;
          params.dl_f_ref_arfcn = 520000;
          params.band           = nr_band::n41;
          return params;
        }(),
        scheduler_expert_config{}),
    cell_cfg([this, rrm_policy_members]() -> const cell_configuration& {
      auto req               = test_cfg.get_default_cell_config_request();
      req.rrm_policy_members = rrm_policy_members;
      return *test_cfg.add_cell(req);
    }())
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    ues.add_cell(cell_cfg.cell_index);
  }

  ~slice_scheduler_test() { srslog::flush(); }

  void run_slot()
  {
    dummy_alloc.slot_indication(next_slot);
    slice_sched.slot_indication(next_slot, dummy_alloc);
    ++next_slot;
  }

  const ue_configuration* add_ue(const sched_ue_creation_request_message& req)
  {
    const ue_configuration* ue_cfg = test_cfg.add_ue(req);
    ues.add_ue(std::make_unique<ue>(ue_creation_command{*ue_cfg, req.starts_in_fallback, cell_harqs}));
    slice_sched.add_ue(req.ue_index);
    return ue_cfg;
  }

  srslog::basic_logger&                   logger = srslog::fetch_basic_logger("TEST");
  test_helpers::test_sched_config_manager test_cfg;
  const cell_configuration&               cell_cfg;

  cell_harq_manager cell_harqs{MAX_NOF_DU_UES,
                               MAX_NOF_HARQS,
                               std::make_unique<scheduler_harq_timeout_dummy_notifier>()};
  ue_repository     ues;

  cell_resource_allocator dummy_alloc{cell_cfg};
  inter_slice_scheduler   slice_sched{cell_cfg, ues};

public:
  slot_point next_slot{to_numerology_value(cell_cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().scs), 0};
};

class default_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    auto req               = test_cfg.get_default_ue_config_request();
    req.ue_index           = ue_idx;
    req.crnti              = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback = false;
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(default_slice_scheduler_test, if_no_rrm_policy_cfg_exists_then_only_default_slices_are_created)
{
  ASSERT_EQ(slice_sched.nof_slices(), 2);
}

TEST_F(default_slice_scheduler_test, when_no_lcid_exists_then_default_slice_is_not_a_candidate)
{
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  auto next_ul_slice = slice_sched.get_next_ul_candidate();
  ASSERT_FALSE(next_ul_slice.has_value());
}

TEST_F(default_slice_scheduler_test, when_lcid_is_part_of_default_slice_then_default_slice_is_valid_candidate)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  for (unsigned count = 0, e = 10; count != e; ++count) {
    bool is_dl_active = has_active_tdd_dl_symbols(cell_cfg.tdd_cfg_common.value(), next_slot.slot_index());
    run_slot();

    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    auto next_ul_slice = slice_sched.get_next_ul_candidate();

    if (is_dl_active) {
      ASSERT_TRUE(next_dl_slice.has_value());
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0)));
      ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_SRB1));

      ASSERT_TRUE(next_ul_slice.has_value());
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_TRUE(next_ul_slice->is_candidate(to_du_ue_index(0)));
      ASSERT_TRUE(next_ul_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_SRB1));
    } else {
      ASSERT_FALSE(next_dl_slice.has_value());
      ASSERT_FALSE(next_ul_slice.has_value());
    }
  }
}

TEST_F(default_slice_scheduler_test,
       when_candidate_instance_goes_out_of_scope_then_it_stops_being_a_candidate_for_the_same_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), cell_cfg.nof_dl_prbs);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), cell_cfg.nof_dl_prbs);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(default_slice_scheduler_test, when_candidate_instance_goes_out_of_scope_then_it_can_be_a_candidate_for_next_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  run_slot();
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);

  run_slot();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
}

TEST_F(default_slice_scheduler_test, when_grant_gets_allocated_then_number_of_available_rbs_decreases)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();

  unsigned alloc_rbs = 10;
  unsigned rem_rbs   = next_dl_slice->remaining_rbs();
  next_dl_slice->store_grant(alloc_rbs);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), rem_rbs - alloc_rbs);
}

TEST_F(default_slice_scheduler_test, returns_only_dl_pending_bytes_of_bearers_belonging_to_a_slice)
{
  constexpr unsigned      srb_pending_bytes{200};
  constexpr unsigned      drb_pending_bytes{5000};
  constexpr du_ue_index_t ue_idx{to_du_ue_index(0)};

  ASSERT_NE(this->add_ue(ue_idx), nullptr);
  // Push buffer state indication for DRB.
  this->ues[ue_idx].handle_dl_buffer_state_indication(LCID_MIN_DRB, drb_pending_bytes);
  // Push buffer state indication for SRB.
  this->ues[ue_idx].handle_dl_buffer_state_indication(LCID_SRB1, srb_pending_bytes);

  run_slot();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  const slice_ue_repository& srb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(srb_slice_ues[ue_idx].pending_dl_newtx_bytes(), get_mac_sdu_required_bytes(srb_pending_bytes));

  // Default DRB slice is next candidate.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
  const slice_ue_repository& drb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(drb_slice_ues[ue_idx].pending_dl_newtx_bytes(), get_mac_sdu_required_bytes(drb_pending_bytes));
}

TEST_F(default_slice_scheduler_test, returns_only_ul_pending_bytes_of_bearers_belonging_to_a_slice)
{
  // Estimate of the number of bytes required for the upper layer header.
  constexpr unsigned      RLC_HEADER_SIZE_ESTIMATE = 3U;
  constexpr unsigned      srb_pending_bytes{200};
  constexpr unsigned      drb_pending_bytes{5000};
  constexpr du_ue_index_t ue_idx{to_du_ue_index(0)};

  const lcg_id_t srb_lcg_id = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1).lc_group;
  const lcg_id_t drb_lcg_id = config_helpers::create_default_logical_channel_config(lcid_t::LCID_MIN_DRB).lc_group;

  ASSERT_NE(this->add_ue(ue_idx), nullptr);
  // Push UL BSR for DRB.
  ul_bsr_indication_message msg{
      to_du_cell_index(0), ue_idx, to_rnti(0x4601 + (unsigned)ue_idx), bsr_format::SHORT_BSR, {}};
  msg.reported_lcgs.push_back(ul_bsr_lcg_report{drb_lcg_id, drb_pending_bytes});
  this->ues[ue_idx].handle_bsr_indication(msg);
  // Push UL BSR for SRB.
  msg.reported_lcgs.clear();
  msg.reported_lcgs.push_back(ul_bsr_lcg_report{srb_lcg_id, srb_pending_bytes});
  this->ues[ue_idx].handle_bsr_indication(msg);

  for (unsigned count = 0, e = 10; count != e; ++count) {
    run_slot();

    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    if (next_ul_slice.has_value()) {
      // Default SRB slice has very high priority.
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      const slice_ue_repository& srb_slice_ues = next_ul_slice->get_slice_ues();
      ASSERT_EQ(srb_slice_ues[ue_idx].pending_ul_newtx_bytes(), get_mac_sdu_required_bytes(srb_pending_bytes));

      // Default DRB slice is next candidate.
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_ul_slice->id(), DEFAULT_DRB_RAN_SLICE_ID);
      const slice_ue_repository& drb_slice_ues = next_ul_slice->get_slice_ues();
      ASSERT_EQ(drb_slice_ues[ue_idx].pending_ul_newtx_bytes(),
                get_mac_sdu_required_bytes(drb_pending_bytes + RLC_HEADER_SIZE_ESTIMATE));
    }
  }
}

// rb_ratio_slice_scheduler_test

class rb_ratio_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned MIN_SLICE_RB = 10;
  static constexpr unsigned MAX_SLICE_RB = 20;

  static constexpr ran_slice_id_t drb1_slice_id{2};
  static constexpr ran_slice_id_t drb2_slice_id{3};

  rb_ratio_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}}, {MIN_SLICE_RB, MAX_SLICE_RB}},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {MIN_SLICE_RB, MAX_SLICE_RB}}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t                                        ue_idx,
                                 const std::initializer_list<logical_channel_config>& lc_cfgs = {})
  {
    const unsigned drb1_idx = 2;
    auto           req      = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    if (lc_cfgs.size() == 0) {
      (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.plmn_id = plmn_identity::test_value();
      (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.s_nssai = s_nssai_t{slice_service_type{1}};
    } else {
      *req.cfg.lc_config_list = lc_cfgs;
    }
    return slice_scheduler_test::add_ue(req);
  }

  const ue_configuration* add_two_drb_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              lc_chs = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice2_nssai)};
    return this->add_ue(ue_idx, lc_chs);
  }
};

TEST_F(rb_ratio_slice_scheduler_test, when_slice_with_min_rb_has_ues_then_it_is_prioritized)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_MIN_DRB));
}

TEST_F(
    rb_ratio_slice_scheduler_test,
    when_slice_rb_ratios_are_min_bounded_but_there_are_no_other_slices_as_candidates_then_remaining_rbs_is_max_bounded)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_rb_ratios_are_min_bounded_and_there_are_other_slices_as_candidates_then_remaining_rbs_is_min_bounded)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_partially_scheduled_then_it_is_never_a_candidate_again_for_the_same_slot)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB - 1); // we leave one RB empty (MIN_SLICE_RB - 1).
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 1);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_allocated_until_min_rb_then_it_can_still_a_candidate_until_max_rb_is_reached)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  // Default SRB slice has very high priority. We ignore it as candidate for this test.
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 0);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  // Original slice is selected again, now using maxRB ratio as the remaining RBs.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  // No more slices to schedule.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_candidates_are_scheduled_in_a_slot_then_their_priority_gets_recomputed_in_next_slot)
{
  ASSERT_NE(this->add_two_drb_ue(to_du_ue_index(0)), nullptr);
  run_slot();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice->store_grant(MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  // New slot and priorities are reestablished.
  run_slot();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slices_are_saturated_then_slices_should_have_equal_opportunity_to_reach_max_rbs)
{
  std::initializer_list<logical_channel_config> lc_cfgs = {
      config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0),
      config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1),
      config_helpers::create_default_logical_channel_config(lcid_t::LCID_MIN_DRB),
      config_helpers::create_default_logical_channel_config(uint_to_lcid(lcid_t::LCID_MIN_DRB + 1))};
  ASSERT_NE(this->add_ue(to_du_ue_index(0), lc_cfgs), nullptr);

  unsigned max_nof_slots = 100;
  unsigned slice_3_count = 0;
  unsigned slice_2_count = 0;
  for (unsigned count = 0; count != max_nof_slots; ++count) {
    run_slot();

    // Either default SRB slice (Slice 0) or DRB1 slice (Slice 2) or DRB2 slice (Slice 3) is selected first since both
    // have minRBs > 0.
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    if (not next_dl_slice.has_value()) {
      continue;
    }
    if (next_dl_slice->id() == drb1_slice_id or next_dl_slice->id() == drb2_slice_id) {
      next_dl_slice->store_grant(MIN_SLICE_RB);
    } else {
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
    }

    // Either Slice 0 or 2 or 3 are then selected.
    next_dl_slice = slice_sched.get_next_dl_candidate();
    if (next_dl_slice->id() == drb1_slice_id) {
      slice_2_count++;
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
    } else if (next_dl_slice->id() == drb2_slice_id) {
      slice_3_count++;
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
    }
  }

  ASSERT_EQ(slice_3_count, slice_2_count) << "Round-robin of slices of same priority failed";
}

// prioritised_slice_scheduler_test

class prioritised_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned       MIN_SLICE_RB = 10;
  static constexpr unsigned       PRIORITY     = 1;
  static constexpr ran_slice_id_t drb1_slice_id{1};
  static constexpr ran_slice_id_t drb2_slice_id{2};
  static constexpr ran_slice_id_t drb3_slice_id{3};

  prioritised_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}}, {MIN_SLICE_RB, MAX_NOF_PRBS}, PRIORITY},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {0, MAX_NOF_PRBS}, PRIORITY}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              req   = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 2), slice2_nssai)};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(prioritised_slice_scheduler_test, when_drb_slice_with_min_rb_has_ues_then_it_is_prioritized)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_pdcch_active = has_active_tdd_dl_symbols(cell_cfg.tdd_cfg_common.value(), next_slot.slot_index());
    run_slot();

    if (is_pdcch_active) {
      auto next_dl_slice = slice_sched.get_next_dl_candidate();
      auto next_ul_slice = slice_sched.get_next_ul_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_ul_slice = slice_sched.get_next_ul_candidate();
      // slice 1 (DRB2) is scheduled before slice 2 (DRB3) or default DRB slice because it has MIN_RB > 0.
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      ASSERT_EQ(next_ul_slice->id(), drb2_slice_id);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb3_slice_id);
      ASSERT_EQ(next_ul_slice->id(), drb3_slice_id);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      ASSERT_EQ(next_ul_slice->id(), drb1_slice_id);
    }
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    ASSERT_FALSE(next_dl_slice.has_value());
    ASSERT_FALSE(next_ul_slice.has_value());
  }
}

class dedicated_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  static constexpr unsigned       DED_SLICE_RB = 10;
  static constexpr unsigned       MIN_SLICE_RB = 20;
  static constexpr unsigned       PRIORITY     = 1;
  static constexpr ran_slice_id_t drb1_slice_id{1};
  static constexpr ran_slice_id_t drb2_slice_id{2};
  static constexpr ran_slice_id_t drb3_slice_id{3};

  dedicated_slice_scheduler_test() :
    slice_scheduler_test(
        {{{plmn_identity::test_value(), s_nssai_t{slice_service_type{1}}},
          {DED_SLICE_RB, MIN_SLICE_RB, MAX_NOF_PRBS},
          PRIORITY},
         {{plmn_identity::test_value(), s_nssai_t{slice_service_type{2}}}, {0, MAX_NOF_PRBS}, PRIORITY}})
  {
  }

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    rrm_policy_member slice1_nssai{.s_nssai = {slice_service_type{1}}};
    rrm_policy_member slice2_nssai{.s_nssai = {slice_service_type{2}}};
    auto              req   = test_cfg.get_default_ue_config_request();
    req.ue_index            = ue_idx;
    req.crnti               = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback  = false;
    *req.cfg.lc_config_list = std::initializer_list<logical_channel_config>{
        create_logical_channel_config(LCID_SRB0),
        create_logical_channel_config(LCID_SRB1),
        create_logical_channel_config(LCID_MIN_DRB),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 1), slice1_nssai),
        create_logical_channel_config(uint_to_lcid(LCID_MIN_DRB + 2), slice2_nssai)};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(dedicated_slice_scheduler_test, when_dedicated_resources_not_filled_then_they_remain_unused)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  const unsigned grant_rbs = 5;
  const unsigned max_prbs  = slice_sched.slice_config((ran_slice_id_t)0).rbs.max();

  for (unsigned count = 0, e = 10; count != e; ++count) {
    const bool is_pdcch_active = has_active_tdd_dl_symbols(cell_cfg.tdd_cfg_common.value(), next_slot.slot_index());
    run_slot();

    if (is_pdcch_active) {
      auto next_dl_slice = slice_sched.get_next_dl_candidate();
      auto next_ul_slice = slice_sched.get_next_ul_candidate();
      // Default SRB slice has very high priority. We ignore it as candidate for this test.
      ASSERT_EQ(next_dl_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_EQ(next_ul_slice->id(), SRB_RAN_SLICE_ID);
      ASSERT_EQ(next_dl_slice->remaining_rbs(), max_prbs);
      ASSERT_EQ(next_ul_slice->remaining_rbs(), max_prbs);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_ul_slice = slice_sched.get_next_ul_candidate();
      // Slice with prioritised and dedicated resources gets scheduled.
      ASSERT_EQ(next_dl_slice->id(), drb2_slice_id);
      ASSERT_EQ(next_ul_slice->id(), drb2_slice_id);
      next_dl_slice->store_grant(DED_SLICE_RB - grant_rbs);
      next_ul_slice->store_grant(DED_SLICE_RB - grant_rbs);
      // Other slices get scheduled.
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb3_slice_id);
      ASSERT_EQ(next_ul_slice->id(), drb3_slice_id);
      ASSERT_EQ(next_dl_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      ASSERT_EQ(next_ul_slice->remaining_rbs(), max_prbs - DED_SLICE_RB);
      next_dl_slice = slice_sched.get_next_dl_candidate();
      next_ul_slice = slice_sched.get_next_ul_candidate();
      ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
      ASSERT_EQ(next_ul_slice->id(), drb1_slice_id);
    }
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    auto next_ul_slice = slice_sched.get_next_ul_candidate();
    ASSERT_FALSE(next_dl_slice.has_value());
    ASSERT_FALSE(next_ul_slice.has_value());
  }
}
