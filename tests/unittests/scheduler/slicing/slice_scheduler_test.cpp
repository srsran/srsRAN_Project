/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/slicing/slice_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;

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
  }

  ~slice_scheduler_test() { srslog::flush(); }

  const ue_configuration* add_ue(const sched_ue_creation_request_message& req)
  {
    const ue_configuration* ue_cfg = test_cfg.add_ue(req);
    ues.add_ue(std::make_unique<ue>(ue_creation_command{*ue_cfg, req.starts_in_fallback, harq_timeout_handler}));
    slice_sched.add_ue(*ue_cfg);
    return ue_cfg;
  }

  srslog::basic_logger&                   logger = srslog::fetch_basic_logger("TEST");
  test_helpers::test_sched_config_manager test_cfg;
  const cell_configuration&               cell_cfg;

  scheduler_harq_timeout_dummy_handler harq_timeout_handler;

  ue_repository ues;

  slice_scheduler slice_sched{cell_cfg, ues};
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
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{0}).min_prb, MAX_NOF_PRBS);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{0}).max_prb, MAX_NOF_PRBS);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{1}).min_prb, 0);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{1}).max_prb, MAX_NOF_PRBS);
}

TEST_F(default_slice_scheduler_test, when_no_lcid_exists_then_default_slice_is_not_a_candidate)
{
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  auto next_ul_slice = slice_sched.get_next_ul_candidate();
  ASSERT_FALSE(next_ul_slice.has_value());
}

TEST_F(default_slice_scheduler_test, when_lcid_is_part_of_default_slice_then_default_slice_is_valid_candidate)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{0});
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0)));
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_SRB1));

  auto next_ul_slice = slice_sched.get_next_ul_candidate();
  ASSERT_TRUE(next_ul_slice.has_value());
  ASSERT_EQ(next_ul_slice->id(), ran_slice_id_t{0});
  ASSERT_TRUE(next_ul_slice->is_candidate(to_du_ue_index(0)));
  ASSERT_TRUE(next_ul_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_SRB1));
}

TEST_F(default_slice_scheduler_test,
       when_candidate_instance_goes_out_of_scope_then_it_stops_being_a_candidate_for_the_same_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{0});

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(default_slice_scheduler_test, when_candidate_instance_goes_out_of_scope_then_it_can_be_a_candidate_for_next_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  slice_sched.slot_indication();
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());

  slice_sched.slot_indication();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{0});
}

TEST_F(default_slice_scheduler_test, when_grant_gets_allocated_then_number_of_available_rbs_decreases)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();

  unsigned alloc_rbs = 10;
  unsigned rem_rbs   = next_dl_slice->remaining_rbs();
  next_dl_slice->store_grant(alloc_rbs);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), rem_rbs - alloc_rbs);
}

TEST_F(default_slice_scheduler_test, returns_only_dl_pending_bytes_of_bearers_belonging_to_a_slice)
{
  constexpr static ran_slice_id_t default_srb_slice_id{0};
  constexpr static ran_slice_id_t default_drb_slice_id{1};

  constexpr static unsigned srb_pending_bytes{200};
  constexpr static unsigned drb_pending_bytes{5000};

  constexpr static du_ue_index_t ue_idx{to_du_ue_index(0)};

  ASSERT_NE(this->add_ue(ue_idx), nullptr);
  // Push buffer state indication for DRB.
  dl_buffer_state_indication_message ind{};
  ind.ue_index = ue_idx;
  ind.lcid     = LCID_MIN_DRB;
  ind.bs       = drb_pending_bytes;
  this->ues[ue_idx].handle_dl_buffer_state_indication(ind);
  // Push buffer state indication for SRB.
  ind.lcid = LCID_SRB1;
  ind.bs   = srb_pending_bytes;
  this->ues[ue_idx].handle_dl_buffer_state_indication(ind);

  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);
  const slice_ue_repository& srb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(srb_slice_ues[ue_idx].pending_dl_newtx_bytes(), get_mac_sdu_required_bytes(srb_pending_bytes));

  // Default DRB slice is next candidate.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_drb_slice_id);
  const slice_ue_repository& drb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(drb_slice_ues[ue_idx].pending_dl_newtx_bytes(), get_mac_sdu_required_bytes(drb_pending_bytes));
}

TEST_F(default_slice_scheduler_test, returns_only_ul_pending_bytes_of_bearers_belonging_to_a_slice)
{
  // Estimate of the number of bytes required for the upper layer header.
  constexpr static unsigned RLC_HEADER_SIZE_ESTIMATE = 3U;

  constexpr static ran_slice_id_t default_srb_slice_id{0};
  constexpr static ran_slice_id_t default_drb_slice_id{1};

  constexpr static unsigned srb_pending_bytes{200};
  constexpr static unsigned drb_pending_bytes{5000};

  const lcg_id_t srb_lcg_id = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1).lc_group;
  const lcg_id_t drb_lcg_id = config_helpers::create_default_logical_channel_config(lcid_t::LCID_MIN_DRB).lc_group;

  constexpr static du_ue_index_t ue_idx{to_du_ue_index(0)};

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

  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);
  const slice_ue_repository& srb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(srb_slice_ues[ue_idx].pending_ul_newtx_bytes(), get_mac_sdu_required_bytes(srb_pending_bytes));

  // Default DRB slice is next candidate.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_drb_slice_id);
  const slice_ue_repository& drb_slice_ues = next_dl_slice->get_slice_ues();
  ASSERT_EQ(drb_slice_ues[ue_idx].pending_ul_newtx_bytes(),
            RLC_HEADER_SIZE_ESTIMATE + get_mac_sdu_required_bytes(drb_pending_bytes));
}

// rb_ratio_slice_scheduler_test

class rb_ratio_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  constexpr static unsigned MIN_SLICE_RB = 10;
  constexpr static unsigned MAX_SLICE_RB = 20;

  constexpr static ran_slice_id_t default_srb_slice_id{0};
  constexpr static ran_slice_id_t default_drb_slice_id{1};
  constexpr static ran_slice_id_t drb1_slice_id{2};

  rb_ratio_slice_scheduler_test() :
    slice_scheduler_test({{{plmn_identity::test_value(), s_nssai_t{1}}, MIN_SLICE_RB, MAX_SLICE_RB},
                          {{plmn_identity::test_value(), s_nssai_t{2}}, MIN_SLICE_RB, MAX_SLICE_RB}})
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
      (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.s_nssai = s_nssai_t{1};
    } else {
      *req.cfg.lc_config_list = lc_cfgs;
    }
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(rb_ratio_slice_scheduler_test, when_slice_with_min_rb_has_ues_then_it_is_the_first_candidate)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_MIN_DRB));
}

TEST_F(rb_ratio_slice_scheduler_test, when_slice_rb_ratios_are_min_bounded_then_remaining_rbs_is_min_bounded)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);

  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_partially_scheduled_then_it_is_never_a_candidate_again_for_the_same_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  next_dl_slice->store_grant(MIN_SLICE_RB - 1); // we leave one RB empty (MIN_SLICE_RB - 1).
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 1);

  // No more slices to schedule.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_allocated_until_min_rb_then_it_can_still_a_candidate_until_max_rb_is_reached)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 0);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  // Original slice is selected again, now using maxRB ratio as the remaining RBs.
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);

  // No more slices to schedule.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_candidates_are_scheduled_in_a_slot_then_priorities_are_recomputed_in_a_new_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  // Default SRB slice has very high priority.
  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);

  next_dl_slice = slice_sched.get_next_dl_candidate();
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  // New slot and priorities are reestablished.
  slice_sched.slot_indication();
  // Default SRB slice has very high priority.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), drb1_slice_id);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slices_are_saturated_then_slices_should_have_equal_opportunity_to_reach_max_rbs)
{
  constexpr static ran_slice_id_t drb2_slice_id{3};

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
    slice_sched.slot_indication();

    // Either default SRB slice (Slice 0) or DRB1 slice (Slice 2) or DRB2 slice (Slice 3) is selected first since both
    // have minRBs > 0.
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    if (not next_dl_slice.has_value()) {
      continue;
    }
    if (next_dl_slice->id() == drb1_slice_id or next_dl_slice->id() == drb2_slice_id) {
      next_dl_slice->store_grant(MIN_SLICE_RB);
    } else {
      ASSERT_EQ(next_dl_slice->id(), default_srb_slice_id);
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

class srb_prioritization_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
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

TEST_F(srb_prioritization_slice_scheduler_test, schedules_default_srb_slice_first_before_any_other_slices)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  ASSERT_NE(this->add_ue(to_du_ue_index(1)), nullptr);
  slice_sched.slot_indication();

  auto next_ul_slice = slice_sched.get_next_ul_candidate();
  ASSERT_TRUE(next_ul_slice.has_value());
  ASSERT_EQ(next_ul_slice->id(), ran_slice_id_t{0});

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_TRUE(next_dl_slice.has_value());
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{0});
}
