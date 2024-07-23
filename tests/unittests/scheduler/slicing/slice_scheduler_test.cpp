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
          params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz100;
          params.dl_arfcn       = 520000;
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

TEST_F(default_slice_scheduler_test, if_no_rrm_policy_cfg_exists_then_only_default_slice_is_created)
{
  ASSERT_EQ(slice_sched.nof_slices(), 1);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{0}).min_prb, 0);
  ASSERT_EQ(slice_sched.slice_config(ran_slice_id_t{0}).max_prb, MAX_NOF_PRBS);
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

// rb_ratio_slice_scheduler_test

class rb_ratio_slice_scheduler_test : public slice_scheduler_test, public ::testing::Test
{
protected:
  constexpr static unsigned MIN_SLICE_RB = 10;
  constexpr static unsigned MAX_SLICE_RB = 20;

  rb_ratio_slice_scheduler_test() : slice_scheduler_test({{{"00101", s_nssai_t{1}}, MIN_SLICE_RB, MAX_SLICE_RB}}) {}

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    const unsigned drb1_idx                                = 2;
    auto           req                                     = test_cfg.get_default_ue_config_request();
    req.ue_index                                           = ue_idx;
    req.crnti                                              = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback                                 = false;
    (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.plmn_id = "00101";
    (*req.cfg.lc_config_list)[drb1_idx].rrm_policy.s_nssai = s_nssai_t{1};
    return slice_scheduler_test::add_ue(req);
  }
};

TEST_F(rb_ratio_slice_scheduler_test, when_slice_with_min_rb_has_ues_then_it_is_the_first_candidate)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_MIN_DRB));
}

TEST_F(rb_ratio_slice_scheduler_test, when_slice_rb_ratios_are_min_bounded_then_remaining_rbs_is_min_bounded)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});

  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_partially_scheduled_then_it_is_never_a_candidate_again_for_the_same_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
  next_dl_slice->store_grant(MIN_SLICE_RB - 1); // we leave one RB empty (MIN_SLICE_RB - 1).
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 1);

  // Another slice should be selected as second candidate.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{0});

  // No more slices to schedule.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slice_with_min_rb_is_allocated_until_min_rb_then_it_can_still_a_candidate_until_max_rb_is_reached)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
  next_dl_slice->store_grant(MIN_SLICE_RB);
  ASSERT_EQ(next_dl_slice->remaining_rbs(), 0);

  // Another slice should be selected as second candidate (assuming RR between the slices 0 and 1).
  ran_slice_id_t last_slice_id{};
  for (unsigned i = 0; i != 2; ++i) {
    next_dl_slice = slice_sched.get_next_dl_candidate();
    if (i == 1) {
      ASSERT_NE(next_dl_slice->id(), last_slice_id) << "The same slice was selected twice.";
    }
    if (next_dl_slice->id() == ran_slice_id_t{0}) {
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_NOF_PRBS);
    } else {
      // Original slice is selected again, now using maxRB ratio as the remaining RBs.
      ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
    }
    last_slice_id = next_dl_slice->id();
  }

  // No more slices to schedule.
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_candidates_are_scheduled_in_a_slot_then_priorities_are_recomputed_in_a_new_slot)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());

  // New slot and priorities are reestablished.
  slice_sched.slot_indication();
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
  ASSERT_EQ(next_dl_slice->remaining_rbs(), MIN_SLICE_RB);
  next_dl_slice->store_grant(MIN_SLICE_RB);
  next_dl_slice                = slice_sched.get_next_dl_candidate();
  ran_slice_id_t last_slice_id = next_dl_slice->id();
  next_dl_slice                = slice_sched.get_next_dl_candidate();
  ASSERT_NE(next_dl_slice->id(), last_slice_id);
  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_FALSE(next_dl_slice.has_value());
}

TEST_F(rb_ratio_slice_scheduler_test,
       when_slices_are_saturated_then_slices_should_have_equal_opportunity_to_reach_max_rbs)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);

  unsigned max_nof_slots = 100;
  unsigned slice_0_count = 0;
  for (unsigned count = 0; count != max_nof_slots; ++count) {
    slice_sched.slot_indication();

    // Slice 1 with minRBs to fill is first selected.
    auto next_dl_slice = slice_sched.get_next_dl_candidate();
    ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
    next_dl_slice->store_grant(MIN_SLICE_RB);

    // Either Slice 0 or 1 are then selected.
    next_dl_slice = slice_sched.get_next_dl_candidate();
    if (next_dl_slice->id() == ran_slice_id_t{0}) {
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_NOF_PRBS);
      slice_0_count++;
    } else {
      ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
      ASSERT_EQ(next_dl_slice->remaining_rbs(), MAX_SLICE_RB - MIN_SLICE_RB);
    }
  }

  ASSERT_EQ(slice_0_count, max_nof_slots / 2) << "Round-robin of slices of same priority failed";
}
