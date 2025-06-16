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

#include "lib/scheduler/config/logical_channel_config_pool.h"
#include "lib/scheduler/ue_context/dl_logical_channel_manager.h"
#include "lib/scheduler/ue_context/ul_logical_channel_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

std::vector<lcid_t> shuffled_lcids()
{
  std::vector<lcid_t> lcids(MAX_LCID);
  for (unsigned i = 0; i != lcids.size(); ++i) {
    lcids[i] = (lcid_t)i;
  }
  std::shuffle(lcids.begin(), lcids.end(), test_rgen::get());
  return lcids;
}

lcid_dl_sch_t get_random_dl_mac_ce()
{
  return (lcid_dl_sch_t)test_rgen::uniform_int<unsigned>((unsigned)lcid_dl_sch_t::SCELL_ACTIV_4_OCTET,
                                                         (unsigned)lcid_dl_sch_t::UE_CON_RES_ID);
}

class dl_logical_channel_tester : public ::testing::Test
{
public:
  logical_channel_config_pool cfg_pool;

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
};

} // namespace

TEST_F(dl_logical_channel_tester, when_buffer_state_is_zero_no_tx_data_is_pending)
{
  lcid_t                     lcid = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_LCID);
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_lcid_config(lcid)};
  lch_mng.handle_dl_buffer_status_indication(lcid, 0);

  ASSERT_EQ(lch_mng.pending_bytes(lcid), 0);
  ASSERT_EQ(lch_mng.total_pending_bytes(), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST_F(dl_logical_channel_tester, buffer_state_indication_has_no_effect_in_inactive_bearer)
{
  lcid_t                     lcid = (lcid_t)test_rgen::uniform_int<unsigned>(1, MAX_LCID);
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};
  unsigned                   buf_st = test_rgen::uniform_int<unsigned>(0, 10000);
  lch_mng.handle_dl_buffer_status_indication(lcid, buf_st);

  ASSERT_EQ(lch_mng.total_pending_bytes(), 0);
  ASSERT_EQ(lch_mng.pending_bytes(lcid), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST_F(dl_logical_channel_tester, buffer_status_indication_updates_tx_pending_bytes)
{
  lcid_t                     lcid = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_LCID);
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_lcid_config(lcid)};

  unsigned buf_st = test_rgen::uniform_int<unsigned>(0, 10000);
  lch_mng.handle_dl_buffer_status_indication(lcid, buf_st);

  if (lcid != LCID_SRB0) {
    // Pending bytes calculation skips SRB0 in fallback.
    ASSERT_EQ(lch_mng.total_pending_bytes(), get_mac_sdu_required_bytes(buf_st));
    ASSERT_EQ(lch_mng.has_pending_bytes(), buf_st > 0);
  }
  ASSERT_EQ(lch_mng.pending_bytes(lcid), get_mac_sdu_required_bytes(buf_st));
  ASSERT_EQ(lch_mng.has_pending_bytes(lcid), buf_st > 0);
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST_F(dl_logical_channel_tester, total_pending_bytes_equal_sum_of_logical_channel_pending_bytes)
{
  std::vector<lcid_t>        lcids = shuffled_lcids();
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_lcid_config(lcids)};
  std::vector<unsigned>      buf_st_inds;
  for (lcid_t lcid : lcids) {
    unsigned dl_bs = test_rgen::uniform_int<unsigned>(0, 10000);
    buf_st_inds.push_back(get_mac_sdu_required_bytes(dl_bs));
    lch_mng.handle_dl_buffer_status_indication(lcid, dl_bs);
  }

  ASSERT_EQ(lch_mng.total_pending_bytes(), std::accumulate(buf_st_inds.begin(), buf_st_inds.end(), 0));
  ASSERT_FALSE(lch_mng.has_pending_ces());
  for (unsigned i = 0; i != lcids.size(); ++i) {
    ASSERT_EQ(lch_mng.pending_bytes(lcids[i]), buf_st_inds[i]);
  }
}

TEST_F(dl_logical_channel_tester, mac_ce_indication_updates_tx_pending_bytes)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};
  const unsigned             dummy_ce_payload = 0;
  ASSERT_TRUE(lch_mng.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = dummy_ce_payload}));

  ASSERT_TRUE(lch_mng.has_pending_bytes());
  ASSERT_TRUE(lch_mng.has_pending_ces());
  ASSERT_EQ(lch_mng.total_pending_bytes(),
            lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE);
}

TEST(dl_logical_channel_ce_test, derivation_of_mac_ce_size)
{
  // Note: see TS38.321, Section 6.1.3.
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce(), 6);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce(), 1);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::DRX_CMD}.sizeof_ce(), 0);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::LONG_DRX_CMD}.sizeof_ce(), 0);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::SCELL_ACTIV_1_OCTET}.sizeof_ce(), 1);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::SCELL_ACTIV_4_OCTET}.sizeof_ce(), 4);
}

TEST_F(dl_logical_channel_tester, no_mac_subpdus_scheduled_if_no_bytes_pending)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};

  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = lch_mng.allocate_mac_ce(subpdu, 100000);
  allocated_bytes += lch_mng.allocate_mac_sdu(subpdu, 100000);

  ASSERT_EQ(allocated_bytes, 0);
}

TEST_F(dl_logical_channel_tester, mac_ce_is_scheduled_if_tb_has_space)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};

  lcid_dl_sch_t  ce_lcid          = get_random_dl_mac_ce();
  const unsigned dummy_ce_payload = 0;
  ASSERT_TRUE(lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload}));
  unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  unsigned tb_size               = test_rgen::uniform_int<unsigned>(0, 50);

  dl_msg_lc_info subpdu;

  unsigned allocated_bytes = 0;
  if (ce_lcid == lcid_dl_sch_t::UE_CON_RES_ID) {
    allocated_bytes = lch_mng.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);
  } else {
    allocated_bytes = lch_mng.allocate_mac_ce(subpdu, tb_size);
  }
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

TEST_F(dl_logical_channel_tester, mac_sdu_is_scheduled_if_tb_has_space)
{
  lcid_t                     lcid = LCID_SRB1;
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_lcid_config(lcid)};

  unsigned sdu_size = test_rgen::uniform_int<unsigned>(1, 1000);
  lch_mng.handle_dl_buffer_status_indication(lcid, sdu_size);
  unsigned tb_size = test_rgen::uniform_int<unsigned>(0, sdu_size * 2);

  unsigned rem_bytes = tb_size, rem_sdu_size = sdu_size;
  do {
    unsigned       pending_bytes = lch_mng.total_pending_bytes();
    dl_msg_lc_info subpdu;
    unsigned       allocated_bytes = lch_mng.allocate_mac_sdu(subpdu, rem_bytes, lcid);
    if (not subpdu.lcid.is_valid()) {
      // There was not enough space in the TB to deplete all the pending tx bytes.
      ASSERT_LT(tb_size, get_mac_sdu_required_bytes(sdu_size));
      break;
    }

    ASSERT_EQ(subpdu.lcid, lcid) << "Incorrect LCID allocated";
    ASSERT_EQ(get_mac_sdu_required_bytes(subpdu.sched_bytes), allocated_bytes);
    ASSERT_LE(allocated_bytes, rem_bytes) << "allocated bytes are larger than the TB size";

    if (allocated_bytes >= pending_bytes) {
      ASSERT_FALSE(lch_mng.has_pending_bytes()) << "subPDU is large enough to deplete all the pending tx bytes";
    } else {
      rem_sdu_size -= subpdu.sched_bytes;
      // Note: In the case the logical channel was not totally flushed, the manager adds some extra bytes to account for
      // RLC overhead.
      const unsigned RLC_SEGMENTATION_OVERHEAD = 4;
      unsigned       req_bytes                 = get_mac_sdu_required_bytes(rem_sdu_size);
      unsigned       lc_pending_bytes          = lch_mng.total_pending_bytes() - RLC_SEGMENTATION_OVERHEAD;
      if (req_bytes >= 254 and req_bytes <= 258) {
        // Note: account for ambiguity in transition between MAC subheader sizes.
        req_bytes++;
      }
      ASSERT_EQ(req_bytes, lc_pending_bytes)
          << fmt::format("incorrect calculation of remaining pending tx bytes for SDU of size={}", subpdu.sched_bytes);
    }

    rem_bytes -= allocated_bytes;
  } while (lch_mng.has_pending_bytes());
}

TEST_F(dl_logical_channel_tester, check_scheduling_of_ue_con_res_id_mac_ce)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};

  lcid_dl_sch_t  ce_lcid          = lcid_dl_sch_t::UE_CON_RES_ID;
  const unsigned dummy_ce_payload = 0;
  ASSERT_TRUE(lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload}));
  unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  unsigned tb_size               = 10;

  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = lch_mng.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);

  ASSERT_EQ(allocated_bytes, mac_ce_required_bytes);
  ASSERT_EQ(subpdu.lcid, ce_lcid);
  ASSERT_EQ(subpdu.sched_bytes, lcid_dl_sch_t{ce_lcid}.sizeof_ce());
}

TEST_F(dl_logical_channel_tester, pending_ue_con_res_id_ce_bytes_does_not_include_other_mac_ce)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};

  lcid_dl_sch_t  ce_lcid          = lcid_dl_sch_t::LONG_DRX_CMD;
  const unsigned dummy_ce_payload = 0;
  ASSERT_TRUE(lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload}));

  ASSERT_EQ(lch_mng.pending_con_res_ce_bytes(), 0);
}

TEST_F(dl_logical_channel_tester, assign_leftover_bytes_to_sdu_if_leftover_bytes_is_less_than_five_bytes)
{
  const unsigned tb_size = 309;

  dl_logical_channel_manager lch_mng{
      subcarrier_spacing::kHz15, false, create_lcid_config(std::vector<lcid_t>{LCID_SRB0, LCID_SRB1})};
  lcid_dl_sch_t  ce_lcid          = lcid_dl_sch_t::UE_CON_RES_ID;
  const unsigned dummy_ce_payload = 0;
  ASSERT_TRUE(lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload}));
  lch_mng.handle_dl_buffer_status_indication(LCID_SRB0, 295);
  lch_mng.handle_dl_buffer_status_indication(LCID_SRB1, 10000);

  dl_msg_lc_info subpdu;

  unsigned allocated_bytes = 0;
  // ConRes occupies 7 bytes => 6 bytes ConRes CE + 1 bytes header.
  allocated_bytes += lch_mng.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);
  // SRB0 SDU requires at least 298 bytes => 295 payload size + 3 bytes MAC header. Leftover bytes = 4 bytes.
  allocated_bytes += lch_mng.allocate_mac_sdu(subpdu, tb_size - allocated_bytes, LCID_SRB0);
  // Verify leftover bytes are assigned to the last SDU.
  ASSERT_EQ(allocated_bytes, tb_size);
}

TEST_F(dl_logical_channel_tester, ta_cmd_mac_ce_gets_updated_if_already_in_pending_ces_queue)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_empty_config()};
  const auto first_ta_cmd_ce_payload = ta_cmd_ce_payload{.tag_id = time_alignment_group::id_t{0}, .ta_cmd = 29};
  ASSERT_TRUE(
      lch_mng.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = first_ta_cmd_ce_payload}));
  ASSERT_TRUE(lch_mng.has_pending_bytes());
  ASSERT_TRUE(lch_mng.has_pending_ces());

  const auto second_ta_cmd_ce_payload = ta_cmd_ce_payload{.tag_id = time_alignment_group::id_t{0}, .ta_cmd = 33};
  ASSERT_TRUE(
      lch_mng.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = second_ta_cmd_ce_payload}));
  ASSERT_TRUE(lch_mng.has_pending_bytes());
  ASSERT_TRUE(lch_mng.has_pending_ces());

  dl_msg_lc_info subpdu;
  lch_mng.allocate_mac_ce(subpdu, 100000);

  ASSERT_EQ(subpdu.lcid, lcid_dl_sch_t::TA_CMD);
  ASSERT_EQ(subpdu.sched_bytes, lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce());
  ASSERT_TRUE(std::holds_alternative<ta_cmd_ce_payload>(subpdu.ce_payload));
  ASSERT_EQ(std::get<ta_cmd_ce_payload>(subpdu.ce_payload).ta_cmd, second_ta_cmd_ce_payload.ta_cmd);
}

TEST_F(dl_logical_channel_tester, when_pending_bytes_are_requested_for_a_slice_then_bearers_are_selected_accordingly)
{
  dl_logical_channel_manager lch_mng{
      subcarrier_spacing::kHz15, false, create_lcid_config({LCID_SRB1, LCID_MIN_DRB, uint_to_lcid(LCID_MIN_DRB + 1)})};
  lch_mng.register_ran_slice(ran_slice_id_t{0});
  lch_mng.register_ran_slice(ran_slice_id_t{1});
  lch_mng.set_lcid_ran_slice(LCID_SRB1, ran_slice_id_t{0});
  lch_mng.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});
  lch_mng.set_lcid_ran_slice(uint_to_lcid(LCID_MIN_DRB + 1), ran_slice_id_t{1});

  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{1}));

  const unsigned srb1_bytes = 10000, drb1_bytes = 20000, drb2_bytes = 50;
  lch_mng.handle_dl_buffer_status_indication(LCID_SRB1, srb1_bytes);
  lch_mng.handle_dl_buffer_status_indication(LCID_MIN_DRB, drb1_bytes);
  lch_mng.handle_dl_buffer_status_indication(uint_to_lcid(LCID_MIN_DRB + 1), drb2_bytes);
  ASSERT_TRUE(lch_mng.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_EQ(lch_mng.pending_bytes(ran_slice_id_t{0}), get_mac_sdu_required_bytes(srb1_bytes));
  ASSERT_TRUE(lch_mng.has_pending_bytes(ran_slice_id_t{1}));
  ASSERT_EQ(lch_mng.pending_bytes(ran_slice_id_t{1}),
            get_mac_sdu_required_bytes(drb1_bytes) + get_mac_sdu_required_bytes(drb2_bytes));

  // Remove SRB1 from slice.
  lch_mng.reset_lcid_ran_slice(LCID_SRB1);
  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{0}));

  // Deactivate the whole slice.
  lch_mng.deregister_ran_slice(ran_slice_id_t{1});
  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{1}));
}

TEST_F(dl_logical_channel_tester, when_ce_is_pending_and_non_srb_slice_has_pending_bytes_then_ce_goes_in_that_slice)
{
  dl_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, false, create_lcid_config({LCID_SRB1, LCID_MIN_DRB})};
  lch_mng.register_ran_slice(ran_slice_id_t{0});
  lch_mng.register_ran_slice(ran_slice_id_t{1});
  lch_mng.set_lcid_ran_slice(LCID_SRB1, ran_slice_id_t{0});
  lch_mng.set_lcid_ran_slice(LCID_MIN_DRB, ran_slice_id_t{1});

  // CE is pending. SRB slice should be the selected slice to send it.
  const auto second_ta_cmd_ce_payload = ta_cmd_ce_payload{.tag_id = time_alignment_group::id_t{0}, .ta_cmd = 33};
  ASSERT_TRUE(
      lch_mng.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = second_ta_cmd_ce_payload}));
  ASSERT_TRUE(lch_mng.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{1}));

  // Non-SRB slice has now pending bytes. CE should go in the Non-SRB slice.
  lch_mng.handle_dl_buffer_status_indication(LCID_MIN_DRB, 10);
  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_TRUE(lch_mng.has_pending_bytes(ran_slice_id_t{1}));

  // Non-SRB slice doesn't have pending bytes. CE should go in the SRB slice.
  lch_mng.handle_dl_buffer_status_indication(LCID_MIN_DRB, 0);
  ASSERT_TRUE(lch_mng.has_pending_bytes(ran_slice_id_t{0}));
  ASSERT_FALSE(lch_mng.has_pending_bytes(ran_slice_id_t{1}));
}
