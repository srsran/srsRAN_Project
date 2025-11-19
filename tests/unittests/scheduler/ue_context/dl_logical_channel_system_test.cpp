/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/config/logical_channel_config_pool.h"
#include "lib/scheduler/ue_context/dl_logical_channel_system.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// (Implementation-defined) Estimation of how much space the MAC should leave for RLC segmentation header overhead.
static constexpr unsigned RLC_SEGMENTATION_OVERHEAD = 3;

static lcid_dl_sch_t get_random_dl_mac_ce()
{
  return (lcid_dl_sch_t)test_rgen::uniform_int<unsigned>((unsigned)lcid_dl_sch_t::SCELL_ACTIV_4_OCTET,
                                                         (unsigned)lcid_dl_sch_t::UE_CON_RES_ID);
}

TEST(mac_ce_size_test, derivation_of_mac_ce_size)
{
  // Note: see TS38.321, Section 6.1.3.
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce(), 6);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce(), 1);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::DRX_CMD}.sizeof_ce(), 0);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::LONG_DRX_CMD}.sizeof_ce(), 0);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::SCELL_ACTIV_1_OCTET}.sizeof_ce(), 1);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::SCELL_ACTIV_4_OCTET}.sizeof_ce(), 4);
}

/// Battery of tests to ensure the logical consistency of the ue_dl_logical_channel_repository state.
static void assert_ue_lch_valid(const ue_dl_logical_channel_repository& u)
{
  EXPECT_TRUE(u.has_pending_bytes() == (u.pending_bytes() > 0));
  EXPECT_TRUE(u.has_pending_ces() == (u.pending_ce_bytes() > 0));
  auto sorted_channels = u.get_prioritized_logical_channels();
  EXPECT_LE(sorted_channels.size(), u.cfg().size());
  for (auto& lc : u.cfg().logical_channels()) {
    EXPECT_EQ(std::find(sorted_channels.begin(), sorted_channels.end(), lc->lcid) != sorted_channels.end(),
              u.is_active(lc->lcid));
  }
  unsigned ces     = u.pending_ce_bytes();
  unsigned buffers = 0;
  for (lcid_t lcid : sorted_channels) {
    EXPECT_TRUE(u.has_pending_bytes(lcid) == (u.pending_bytes(lcid) > 0));
    buffers += u.pending_bytes(lcid);
  }
  EXPECT_EQ(buffers + ces, u.total_pending_bytes());
  std::map<ran_slice_id_t, unsigned> slice_bytes;
  unsigned                           slices_total = 0;
  for (lcid_t lcid : sorted_channels) {
    if (u.get_slice_id(lcid).has_value()) {
      slice_bytes.insert(std::pair<ran_slice_id_t, unsigned>(u.get_slice_id(lcid).value(), 0));
      slice_bytes.at(u.get_slice_id(lcid).value()) += u.pending_bytes(lcid);
      slices_total += u.pending_bytes(lcid);
    }
  }
  ASSERT_LE(slices_total, u.total_pending_bytes());
  for (auto& [slice_id, bytes] : slice_bytes) {
    EXPECT_GE(u.pending_bytes(slice_id), bytes);
    EXPECT_LE(u.pending_bytes(slice_id), bytes + ces);
  }
}

class dl_logical_channel_system_test
{
protected:
  logical_channel_config_list_ptr create_empty_config()
  {
    return cfg_pool.create(std::vector<logical_channel_config>{});
  }
  logical_channel_config_list_ptr create_lcid_config(lcid_t lcid)
  {
    return cfg_pool.create(
        std::vector<logical_channel_config>{config_helpers::create_default_logical_channel_config(lcid)});
  }
  logical_channel_config_list_ptr create_lcid_config(const std::vector<lcid_t>& lcids)
  {
    std::vector<logical_channel_config> configs;
    for (lcid_t lcid : lcids) {
      configs.push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    return cfg_pool.create(configs);
  }

  dl_logical_channel_system   lch_system;
  logical_channel_config_pool cfg_pool;
};

class single_ue_dl_logical_channel_system_test : public dl_logical_channel_system_test, public ::testing::Test
{
protected:
  ~single_ue_dl_logical_channel_system_test() override
  {
    if (ue_lchs.valid()) {
      assert_ue_lch_valid(ue_lchs);
    }
  }

  std::vector<lcid_t>              lcids{LCID_SRB0, LCID_SRB1, LCID_MIN_DRB};
  ue_dl_logical_channel_repository ue_lchs{
      lch_system.create_ue(subcarrier_spacing::kHz30, false, create_lcid_config(lcids))};
};

TEST_F(single_ue_dl_logical_channel_system_test, no_pending_data)
{
  ASSERT_EQ(this->lch_system.nof_ues(), 1);

  ASSERT_EQ(ue_lchs.cfg().size(), 3);
  ASSERT_TRUE(ue_lchs.is_active(LCID_SRB0) and ue_lchs.is_active(LCID_SRB1) and
              ue_lchs.is_active(lcid_t::LCID_MIN_DRB));
  ASSERT_EQ(ue_lchs.pending_bytes(), 0);
  ASSERT_EQ(ue_lchs.total_pending_bytes(), 0);
  ASSERT_FALSE(ue_lchs.has_pending_bytes(lcid_t::LCID_SRB0));
  ASSERT_FALSE(ue_lchs.has_pending_bytes(lcid_t::LCID_SRB1));
  ASSERT_EQ(ue_lchs.pending_bytes(lcid_t::LCID_SRB0), 0);
  ASSERT_EQ(ue_lchs.pending_bytes(lcid_t::LCID_SRB1), 0);
  ASSERT_FALSE(ue_lchs.has_slice(ran_slice_id_t{0}));
  assert_ue_lch_valid(ue_lchs);
}

TEST_F(single_ue_dl_logical_channel_system_test, ue_removal)
{
  ASSERT_EQ(this->lch_system.nof_ues(), 1);
  ASSERT_EQ(this->lch_system.nof_logical_channels(), 3);
  ASSERT_EQ(ue_lchs.get_prioritized_logical_channels().size(), 3);
  ue_lchs.reset();
  ASSERT_EQ(this->lch_system.nof_ues(), 0);
  ASSERT_EQ(this->lch_system.nof_logical_channels(), 0);
}

TEST_F(single_ue_dl_logical_channel_system_test, when_buffer_state_is_zero_no_tx_data_is_pending)
{
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB1, 0);
  ue_lchs.handle_dl_buffer_status_indication(LCID_MIN_DRB, 0);

  ASSERT_EQ(ue_lchs.pending_bytes(LCID_SRB1), 0);
  ASSERT_EQ(ue_lchs.pending_bytes(LCID_MIN_DRB), 0);
  ASSERT_EQ(ue_lchs.total_pending_bytes(), 0);
  ASSERT_FALSE(ue_lchs.has_pending_bytes());
}

TEST_F(single_ue_dl_logical_channel_system_test, buffer_state_indication_has_no_effect_in_inactive_bearer)
{
  lcid_t   lcid   = uint_to_lcid(lcid_t::LCID_MIN_DRB + 1); // Not configured LCID.
  unsigned buf_st = test_rgen::uniform_int<unsigned>(0, 10000);
  ue_lchs.handle_dl_buffer_status_indication(lcid, buf_st);

  ASSERT_EQ(ue_lchs.total_pending_bytes(), 0);
  ASSERT_EQ(ue_lchs.pending_bytes(lcid), 0);
  ASSERT_FALSE(ue_lchs.has_pending_bytes(lcid));
  ASSERT_FALSE(ue_lchs.has_pending_bytes());
  ASSERT_FALSE(ue_lchs.has_pending_ces());
}

TEST_F(single_ue_dl_logical_channel_system_test, buffer_status_indication_updates_tx_pending_bytes)
{
  lcid_t   lcid   = test_rgen::uniform_int(0U, 1U) == 0 ? LCID_SRB1 : LCID_MIN_DRB;
  unsigned buf_st = test_rgen::uniform_int<unsigned>(0, 10000);
  ue_lchs.handle_dl_buffer_status_indication(lcid, buf_st);

  ASSERT_EQ(ue_lchs.total_pending_bytes(), get_mac_sdu_required_bytes(buf_st));
  ASSERT_EQ(ue_lchs.has_pending_bytes(), buf_st > 0);
  ASSERT_EQ(ue_lchs.pending_bytes(lcid), get_mac_sdu_required_bytes(buf_st));
  ASSERT_EQ(ue_lchs.has_pending_bytes(lcid), buf_st > 0);
  ASSERT_FALSE(ue_lchs.has_pending_ces());
}

TEST_F(single_ue_dl_logical_channel_system_test, srb0_not_reported_in_has_pending_bytes_of_non_fallback_ue)
{
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB0, 5);
  ASSERT_FALSE(ue_lchs.has_pending_bytes());
  ASSERT_TRUE(ue_lchs.has_pending_bytes(LCID_SRB0));
  ASSERT_EQ(ue_lchs.pending_bytes(LCID_SRB0), get_mac_sdu_required_bytes(5));
}

TEST_F(single_ue_dl_logical_channel_system_test, deactivate_ue)
{
  lcid_t   lcid   = uint_to_lcid(lcid_t::LCID_MIN_DRB);
  unsigned buf_st = test_rgen::uniform_int<unsigned>(1, 10000);
  ue_lchs.handle_dl_buffer_status_indication(lcid, buf_st);

  ASSERT_EQ(ue_lchs.cfg().size(), 3);
  ASSERT_TRUE(ue_lchs.is_active(LCID_SRB0) and ue_lchs.is_active(LCID_SRB1) and
              ue_lchs.is_active(lcid_t::LCID_MIN_DRB));
  ASSERT_TRUE(ue_lchs.has_pending_bytes(lcid));
  ASSERT_TRUE(ue_lchs.has_pending_bytes());
  assert_ue_lch_valid(ue_lchs);
  ue_lchs.deactivate();
  ASSERT_EQ(this->lch_system.nof_ues(), 1);
  ASSERT_EQ(ue_lchs.cfg().size(), 3);
  ASSERT_TRUE(not ue_lchs.is_active(LCID_SRB0) and not ue_lchs.is_active(LCID_SRB1) and
              not ue_lchs.is_active(lcid_t::LCID_MIN_DRB));
  ASSERT_FALSE(ue_lchs.has_pending_bytes(lcid));
  ASSERT_FALSE(ue_lchs.has_pending_bytes());
}

TEST_F(single_ue_dl_logical_channel_system_test, total_pending_bytes_equal_sum_of_logical_channel_pending_bytes)
{
  std::vector<unsigned> buf_st_inds;
  for (lcid_t lcid : lcids) {
    unsigned dl_bs = test_rgen::uniform_int<unsigned>(0, 10000);
    buf_st_inds.push_back(get_mac_sdu_required_bytes(dl_bs));
    ue_lchs.handle_dl_buffer_status_indication(lcid, dl_bs);
  }

  ASSERT_EQ(ue_lchs.total_pending_bytes(), std::accumulate(buf_st_inds.begin(), buf_st_inds.end(), 0));
  ASSERT_FALSE(ue_lchs.has_pending_ces());
  for (unsigned i = 0; i != lcids.size(); ++i) {
    ASSERT_EQ(ue_lchs.pending_bytes(lcids[i]), buf_st_inds[i]);
  }
}

TEST_F(single_ue_dl_logical_channel_system_test, mac_ce_indication_updates_tx_pending_bytes)
{
  const unsigned dummy_ce_payload = 0;
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = dummy_ce_payload}));

  ASSERT_TRUE(ue_lchs.has_pending_bytes());
  ASSERT_TRUE(ue_lchs.has_pending_ces());
  ASSERT_EQ(ue_lchs.total_pending_bytes(),
            lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE);
}

TEST_F(single_ue_dl_logical_channel_system_test, no_mac_subpdus_scheduled_if_no_bytes_pending)
{
  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = ue_lchs.allocate_mac_ce(subpdu, 100000);
  allocated_bytes += ue_lchs.allocate_mac_sdu(subpdu, 100000);
  ASSERT_EQ(allocated_bytes, 0);
}

TEST_F(single_ue_dl_logical_channel_system_test, mac_ce_is_scheduled_if_tb_has_space)
{
  lcid_dl_sch_t ce_lcid = get_random_dl_mac_ce();
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = ce_lcid}));
  const unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  const unsigned tb_size               = test_rgen::uniform_int<unsigned>(0, 50);

  dl_msg_lc_info subpdu;

  unsigned allocated_bytes = 0;
  assert_ue_lch_valid(ue_lchs);
  if (ce_lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
    allocated_bytes = ue_lchs.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);
  } else {
    allocated_bytes = ue_lchs.allocate_mac_ce(subpdu, tb_size);
  }
  assert_ue_lch_valid(ue_lchs);
  if (mac_ce_required_bytes <= tb_size) {
    ASSERT_EQ(allocated_bytes, mac_ce_required_bytes);
    ASSERT_EQ(subpdu.lcid, ce_lcid);
    ASSERT_EQ(subpdu.sched_bytes, lcid_dl_sch_t{ce_lcid}.sizeof_ce());
  } else {
    ASSERT_EQ(allocated_bytes, 0);
    ASSERT_FALSE(subpdu.lcid.is_valid());
    ASSERT_EQ(subpdu.sched_bytes, 0);
  }
}

TEST_F(single_ue_dl_logical_channel_system_test, multiple_mac_ces_are_scheduled_if_tb_has_space)
{
  lcid_dl_sch_t ce_lcid = lcid_dl_sch_t::DRX_CMD;
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = ce_lcid}));
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = ce_lcid}));
  const unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  ASSERT_EQ(ue_lchs.pending_ce_bytes(), 2 * mac_ce_required_bytes);

  const unsigned tb_size = mac_ce_required_bytes;
  dl_msg_lc_info subpdu;
  assert_ue_lch_valid(ue_lchs);
  auto allocated_bytes = ue_lchs.allocate_mac_ce(subpdu, tb_size);
  ASSERT_EQ(allocated_bytes, mac_ce_required_bytes);
  ASSERT_EQ(ue_lchs.pending_ce_bytes(), mac_ce_required_bytes);
  assert_ue_lch_valid(ue_lchs);
  allocated_bytes = ue_lchs.allocate_mac_ce(subpdu, tb_size);
  ASSERT_EQ(allocated_bytes, mac_ce_required_bytes);
  ASSERT_EQ(ue_lchs.pending_ce_bytes(), 0);
  assert_ue_lch_valid(ue_lchs);
  allocated_bytes = ue_lchs.allocate_mac_ce(subpdu, tb_size);
  ASSERT_EQ(allocated_bytes, 0);
  ASSERT_EQ(ue_lchs.pending_ce_bytes(), 0);
}

TEST_F(single_ue_dl_logical_channel_system_test, mac_sdu_is_scheduled_if_tb_has_space)
{
  lcid_t   lcid     = LCID_SRB1;
  unsigned sdu_size = test_rgen::uniform_int<unsigned>(1, 1000);
  ue_lchs.handle_dl_buffer_status_indication(lcid, sdu_size);
  unsigned tb_size = test_rgen::uniform_int<unsigned>(0, sdu_size * 2);
  assert_ue_lch_valid(ue_lchs);

  unsigned rem_bytes = tb_size, rem_sdu_size = sdu_size;
  do {
    unsigned       pending_bytes = ue_lchs.total_pending_bytes();
    dl_msg_lc_info subpdu;
    unsigned       allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, rem_bytes, lcid);
    assert_ue_lch_valid(ue_lchs);
    if (not subpdu.lcid.is_valid()) {
      if (tb_size > RLC_SEGMENTATION_OVERHEAD + MIN_MAC_SDU_SUBHEADER_SIZE) {
        // The TB size was large enough to fit at least some MAC SDU + RLC overhead. However, there was not enough
        // space in the TB to deplete all the pending tx bytes.
        ASSERT_LT(tb_size, get_mac_sdu_required_bytes(sdu_size));
      } else {
        ASSERT_EQ(rem_bytes, tb_size);
      }
      break;
    }

    ASSERT_EQ(subpdu.lcid, lcid) << "Incorrect LCID allocated";
    ASSERT_EQ(get_mac_sdu_required_bytes(subpdu.sched_bytes), allocated_bytes);
    ASSERT_LE(allocated_bytes, rem_bytes) << "allocated bytes are larger than the TB size";

    if (allocated_bytes >= pending_bytes) {
      ASSERT_FALSE(ue_lchs.has_pending_bytes()) << "subPDU is large enough to deplete all the pending tx bytes";
    } else {
      rem_sdu_size -= subpdu.sched_bytes;
      // Note: In the case the logical channel was not totally flushed, the manager adds some extra bytes to account for
      // RLC overhead.
      unsigned req_bytes        = get_mac_sdu_required_bytes(rem_sdu_size + RLC_SEGMENTATION_OVERHEAD);
      unsigned lc_pending_bytes = ue_lchs.total_pending_bytes();
      ASSERT_EQ(req_bytes, lc_pending_bytes)
          << fmt::format("incorrect calculation of remaining pending tx bytes for SDU of size={}", subpdu.sched_bytes);
    }

    rem_bytes -= allocated_bytes;
  } while (ue_lchs.has_pending_bytes());
}

TEST_F(single_ue_dl_logical_channel_system_test, check_scheduling_of_ue_con_res_id_mac_ce)
{
  lcid_dl_sch_t ce_lcid = lcid_dl_sch_t::UE_CON_RES_ID;
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = ce_lcid}));
  unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  unsigned tb_size               = 10;

  dl_msg_lc_info subpdu;
  assert_ue_lch_valid(ue_lchs);
  unsigned allocated_bytes = ue_lchs.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);

  ASSERT_EQ(allocated_bytes, mac_ce_required_bytes);
  ASSERT_EQ(subpdu.lcid, ce_lcid);
  ASSERT_EQ(subpdu.sched_bytes, lcid_dl_sch_t{ce_lcid}.sizeof_ce());
}

TEST_F(single_ue_dl_logical_channel_system_test, pending_ue_con_res_id_ce_bytes_does_not_include_other_mac_ce)
{
  lcid_dl_sch_t ce_lcid = lcid_dl_sch_t::LONG_DRX_CMD;
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = ce_lcid}));
  ASSERT_EQ(ue_lchs.pending_con_res_ce_bytes(), 0);
  ASSERT_TRUE(ue_lchs.has_pending_ces());
}

TEST_F(single_ue_dl_logical_channel_system_test, assign_leftover_bytes_to_sdu_if_leftover_bytes_is_less_than_five_bytes)
{
  const unsigned tb_size = 309;

  lcid_dl_sch_t ce_lcid = lcid_dl_sch_t::UE_CON_RES_ID;
  ASSERT_TRUE(ue_lchs.handle_mac_ce_indication({.ce_lcid = ce_lcid}));
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB0, 295);
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB1, 10000);

  dl_msg_lc_info subpdu;

  unsigned allocated_bytes = 0;
  assert_ue_lch_valid(ue_lchs);
  // ConRes occupies 7 bytes => 6 bytes ConRes CE + 1 bytes header.
  allocated_bytes += ue_lchs.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);
  // SRB0 SDU requires at least 298 bytes => 295 payload size + 3 bytes MAC header. Leftover bytes = 4 bytes.
  allocated_bytes += ue_lchs.allocate_mac_sdu(subpdu, tb_size - allocated_bytes, LCID_SRB0);
  // Verify leftover bytes are assigned to the last SDU.
  ASSERT_EQ(allocated_bytes, tb_size);
}

TEST_F(single_ue_dl_logical_channel_system_test,
       when_buffer_occupancy_is_very_low_then_enough_space_is_still_considered_to_fit_rlc_ovh)
{
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB1, 1);

  dl_msg_lc_info subpdu;
  assert_ue_lch_valid(ue_lchs);
  // SRB1 SDU requires 2 bytes of subheader. However, if only 3 bytes were allocated, RLC overhead could not be fitted.
  unsigned allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, 50, LCID_SRB1);
  // Note: The MAC can only estimate the RLC overhead. However, it should always be equal or higher than 2 bytes, which
  // is the RLC UM header min size.
  const unsigned min_rlc_ovh = 2;
  ASSERT_GE(allocated_bytes, MIN_MAC_SDU_SUBHEADER_SIZE + 1 + min_rlc_ovh);
}

TEST_F(single_ue_dl_logical_channel_system_test, ta_cmd_mac_ce_gets_updated_if_already_in_pending_ces_queue)
{
  const auto first_ta_cmd_ce_payload = ta_cmd_ce_payload{.tag_id = time_alignment_group::id_t{0}, .ta_cmd = 29};
  ASSERT_TRUE(
      ue_lchs.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = first_ta_cmd_ce_payload}));
  ASSERT_TRUE(ue_lchs.has_pending_bytes());
  ASSERT_TRUE(ue_lchs.has_pending_ces());

  const auto second_ta_cmd_ce_payload = ta_cmd_ce_payload{.tag_id = time_alignment_group::id_t{0}, .ta_cmd = 33};
  ASSERT_TRUE(
      ue_lchs.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = second_ta_cmd_ce_payload}));
  ASSERT_TRUE(ue_lchs.has_pending_bytes());
  ASSERT_TRUE(ue_lchs.has_pending_ces());

  dl_msg_lc_info subpdu;
  ue_lchs.allocate_mac_ce(subpdu, 100000);

  ASSERT_EQ(subpdu.lcid, lcid_dl_sch_t::TA_CMD);
  ASSERT_EQ(subpdu.sched_bytes, lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce());
  ASSERT_TRUE(std::holds_alternative<ta_cmd_ce_payload>(subpdu.ce_payload));
  ASSERT_EQ(std::get<ta_cmd_ce_payload>(subpdu.ce_payload).ta_cmd, second_ta_cmd_ce_payload.ta_cmd);
}

TEST_F(single_ue_dl_logical_channel_system_test,
       when_pending_bytes_are_requested_for_a_slice_then_bearers_are_selected_accordingly)
{
  lcids = {LCID_SRB0, LCID_SRB1, LCID_MIN_DRB, uint_to_lcid(LCID_MIN_DRB + 1)};
  ue_lchs.configure(this->create_lcid_config(lcids));
  ue_lchs.set_lcid_ran_slice(LCID_SRB1, ran_slice_id_t{0});
  ue_lchs.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});
  ue_lchs.set_lcid_ran_slice(uint_to_lcid(LCID_MIN_DRB + 1), ran_slice_id_t{1});

  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));

  const unsigned srb1_bytes = 10000, drb1_bytes = 20000, drb2_bytes = 50;
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB1, srb1_bytes);
  ue_lchs.handle_dl_buffer_status_indication(LCID_MIN_DRB, drb1_bytes);
  ue_lchs.handle_dl_buffer_status_indication(uint_to_lcid(LCID_MIN_DRB + 1), drb2_bytes);
  ASSERT_TRUE(ue_lchs.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{0}), get_mac_sdu_required_bytes(srb1_bytes));
  ASSERT_TRUE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{1}),
            get_mac_sdu_required_bytes(drb1_bytes) + get_mac_sdu_required_bytes(drb2_bytes));

  // Remove SRB1 from slice.
  ue_lchs.reset_lcid_ran_slice(LCID_SRB1);
  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{0}));

  // Deactivate LC slice but not the whole slice.
  ue_lchs.reset_lcid_ran_slice(LCID_MIN_DRB);
  ASSERT_TRUE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));
  assert_ue_lch_valid(ue_lchs);

  // Deregister slice 1 completely.
  ue_lchs.reset_lcid_ran_slice(uint_to_lcid(LCID_MIN_DRB + 1));
  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));
}

TEST_F(single_ue_dl_logical_channel_system_test, reregistering_slice_does_not_change_pending_bytes)
{
  ue_lchs.set_lcid_ran_slice(LCID_SRB1, ran_slice_id_t{0});

  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{0}), 0);
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{1}), 0);
  const unsigned srb1_bytes = 10000;
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB1, srb1_bytes);
  ASSERT_GE(ue_lchs.pending_bytes(ran_slice_id_t{0}), srb1_bytes);
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{1}), 0);

  // RAN slice 1 only has pending bytes when associated with a logical channel.
  const unsigned drb1_bytes = 20000, subhdr_overhead = 4;
  ue_lchs.handle_dl_buffer_status_indication(LCID_MIN_DRB, drb1_bytes);
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{1}), 0);
  ue_lchs.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});
  ASSERT_GE(ue_lchs.pending_bytes(ran_slice_id_t{1}), drb1_bytes);
  ASSERT_LT(ue_lchs.pending_bytes(ran_slice_id_t{1}), drb1_bytes + subhdr_overhead);

  // Reregistering slice 1 should not change pending bytes.
  for (unsigned i = 0; i != 10; ++i) {
    ue_lchs.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});
    ASSERT_GE(ue_lchs.pending_bytes(ran_slice_id_t{1}), drb1_bytes);
    ASSERT_LT(ue_lchs.pending_bytes(ran_slice_id_t{1}), drb1_bytes + subhdr_overhead);
  }
  for (unsigned i = 0; i != 10; ++i) {
    ue_lchs.reset_lcid_ran_slice(LCID_MIN_DRB);
    ue_lchs.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});
    ASSERT_GE(ue_lchs.pending_bytes(ran_slice_id_t{1}), drb1_bytes);
    ASSERT_LT(ue_lchs.pending_bytes(ran_slice_id_t{1}), drb1_bytes + subhdr_overhead);
  }
  assert_ue_lch_valid(ue_lchs);
}

TEST_F(single_ue_dl_logical_channel_system_test,
       when_ce_is_pending_and_non_srb_slice_has_pending_bytes_then_ce_goes_in_that_slice)
{
  ue_lchs.set_lcid_ran_slice(LCID_SRB1, ran_slice_id_t{0});
  ue_lchs.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});

  // CE is pending. SRB slice should be the selected slice to send it.
  const auto second_ta_cmd_ce_payload = ta_cmd_ce_payload{.tag_id = time_alignment_group::id_t{0}, .ta_cmd = 33};
  ASSERT_TRUE(
      ue_lchs.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = second_ta_cmd_ce_payload}));
  ASSERT_TRUE(ue_lchs.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));

  // Non-SRB slice has now pending bytes. CE should go in the Non-SRB slice.
  ue_lchs.handle_dl_buffer_status_indication(LCID_MIN_DRB, 10);
  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_TRUE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));

  // Non-SRB slice doesn't have pending bytes. CE should go in the SRB slice.
  ue_lchs.handle_dl_buffer_status_indication(LCID_MIN_DRB, 0);
  ASSERT_TRUE(ue_lchs.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_FALSE(ue_lchs.has_pending_bytes(ran_slice_id_t{1}));
}

TEST_F(single_ue_dl_logical_channel_system_test, when_sdu_is_scheduled_pending_slice_bytes_are_updated_accordingly)
{
  ue_lchs.set_lcid_ran_slice(LCID_SRB1, ran_slice_id_t{0});
  ue_lchs.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});

  const unsigned srb1_bytes = 1000, drb1_bytes = 2000;
  ue_lchs.handle_dl_buffer_status_indication(LCID_SRB1, srb1_bytes);
  ue_lchs.handle_dl_buffer_status_indication(LCID_MIN_DRB, drb1_bytes);

  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, 5000, LCID_SRB1);
  ASSERT_EQ(allocated_bytes, get_mac_sdu_required_bytes(srb1_bytes));
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{0}), 0);

  allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, 1000, LCID_MIN_DRB);
  ASSERT_EQ(allocated_bytes, 1000);
  ASSERT_EQ(ue_lchs.pending_bytes(ran_slice_id_t{1}),
            get_mac_sdu_required_bytes(drb1_bytes + RLC_SEGMENTATION_OVERHEAD - subpdu.sched_bytes));
}

TEST_F(single_ue_dl_logical_channel_system_test, qos_gbr_bearer_bitrate_is_tracked)
{
  const lcid_t drb_lcid = LCID_MIN_DRB;
  lch_system.slot_indication();
  ASSERT_EQ(ue_lchs.average_bit_rate(drb_lcid), 0);
  const unsigned sdu_size = test_rgen::uniform_int<unsigned>(1, 1000);
  ue_lchs.handle_dl_buffer_status_indication(drb_lcid, sdu_size);
  unsigned       tb_size = sdu_size + 10;
  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, tb_size, drb_lcid);
  ASSERT_GT(allocated_bytes, 0);
  ASSERT_TRUE(subpdu.lcid.is_valid());
  ASSERT_EQ(subpdu.lcid, drb_lcid);
  ASSERT_EQ(get_mac_sdu_required_bytes(subpdu.sched_bytes), allocated_bytes);
  lch_system.slot_indication();
  ASSERT_EQ(ue_lchs.average_bit_rate(drb_lcid), 0);

  // Add QoS GBR to DRBs.
  std::vector<logical_channel_config> configs;
  for (lcid_t lcid : lcids) {
    configs.push_back(config_helpers::create_default_logical_channel_config(lcid));
    if (not is_srb(lcid)) {
      configs.back().qos.emplace();
      configs.back().qos->qos.average_window_ms = 100;
      configs.back().qos->gbr_qos_info.emplace();
    }
  }
  ue_lchs.configure(cfg_pool.create(configs));

  // QoS GBR bearer should now track bitrate.
  ue_lchs.handle_dl_buffer_status_indication(drb_lcid, sdu_size);
  subpdu          = {};
  allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, tb_size, drb_lcid);
  ASSERT_GT(allocated_bytes, 0);
  ASSERT_EQ(subpdu.lcid, drb_lcid);
  ASSERT_EQ(get_mac_sdu_required_bytes(subpdu.sched_bytes), allocated_bytes);
  lch_system.slot_indication();
  ASSERT_GT(ue_lchs.average_bit_rate(drb_lcid), 0);

  // Remove QoS GBR config should reset bitrate tracking.
  ue_lchs.configure(this->create_lcid_config(lcids));
  ue_lchs.handle_dl_buffer_status_indication(drb_lcid, sdu_size);
  subpdu          = {};
  allocated_bytes = ue_lchs.allocate_mac_sdu(subpdu, tb_size, drb_lcid);
  ASSERT_GT(allocated_bytes, 0);
  lch_system.slot_indication();
  ASSERT_EQ(ue_lchs.average_bit_rate(drb_lcid), 0);

  ASSERT_EQ(lch_system.nof_ues(), 1);
  ASSERT_EQ(ue_lchs.get_prioritized_logical_channels().size(), 3);
  ASSERT_EQ(lch_system.nof_logical_channels(), 3) << "Reconfigurations created extra logical channels";
}

class multi_ue_dl_logical_channel_system_test : public dl_logical_channel_system_test, public ::testing::Test
{};

TEST_F(multi_ue_dl_logical_channel_system_test, multiple_ue_creation)
{
  const auto                                    nof_ues = test_rgen::uniform_int<unsigned>(2, 10);
  std::vector<ue_dl_logical_channel_repository> ues;
  for (unsigned i = 0; i != nof_ues; ++i) {
    ues.push_back(lch_system.create_ue(
        subcarrier_spacing::kHz30, false, create_lcid_config({LCID_SRB0, LCID_SRB1, LCID_MIN_DRB})));
  }
  ASSERT_EQ(lch_system.nof_ues(), nof_ues);
  ASSERT_EQ(lch_system.nof_logical_channels(), nof_ues * 3);

  ues.clear();
  ASSERT_EQ(lch_system.nof_ues(), 0);
  ASSERT_EQ(lch_system.nof_logical_channels(), 0);
}
