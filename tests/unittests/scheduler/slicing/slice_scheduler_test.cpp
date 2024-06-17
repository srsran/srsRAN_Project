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
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;

class slice_scheduler_test : public ::testing::Test
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

  ~slice_scheduler_test() override { srslog::flush(); }

  const ue_configuration* add_ue(const sched_ue_creation_request_message& req)
  {
    const ue_configuration* ue_cfg = test_cfg.add_ue(req);
    slice_sched.add_ue(*ue_cfg);
    return ue_cfg;
  }

  srslog::basic_logger&                   logger = srslog::fetch_basic_logger("TEST");
  test_helpers::test_sched_config_manager test_cfg;
  const cell_configuration&               cell_cfg;

  slice_scheduler slice_sched{cell_cfg};
};

class default_slice_scheduler_test : public slice_scheduler_test
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

class rb_ratio_slice_scheduler_test : public slice_scheduler_test
{
protected:
  constexpr static unsigned MIN_PRB = 10;
  constexpr static unsigned MAX_PRB = 20;

  rb_ratio_slice_scheduler_test() : slice_scheduler_test({{{"00101", s_nssai_t{1}}, MIN_PRB, MAX_PRB}}) {}

  const ue_configuration* add_ue(du_ue_index_t ue_idx)
  {
    auto req                                        = test_cfg.get_default_ue_config_request();
    req.ue_index                                    = ue_idx;
    req.crnti                                       = to_rnti(0x4601 + ue_idx);
    req.starts_in_fallback                          = false;
    (*req.cfg.lc_config_list)[2].rrm_policy.plmn_id = "00101";
    (*req.cfg.lc_config_list)[2].rrm_policy.s_nssai = s_nssai_t{1};
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

TEST_F(rb_ratio_slice_scheduler_test, when_slice_with_min_rb_has_ues_then_it_is_the_first_candidate)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});
  ASSERT_TRUE(next_dl_slice->is_candidate(to_du_ue_index(0), lcid_t::LCID_MIN_DRB));

  next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{0});
}

TEST_F(rb_ratio_slice_scheduler_test, when_slice_rb_ratios_are_bounded_then_remaining_rbs_is_bounded)
{
  ASSERT_NE(this->add_ue(to_du_ue_index(0)), nullptr);
  slice_sched.slot_indication();

  auto next_dl_slice = slice_sched.get_next_dl_candidate();
  ASSERT_EQ(next_dl_slice->id(), ran_slice_id_t{1});

  ASSERT_LE(next_dl_slice->remaining_rbs(), MAX_PRB);
}
