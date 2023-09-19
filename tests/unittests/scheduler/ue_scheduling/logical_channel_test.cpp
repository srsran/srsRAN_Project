/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "lib/scheduler/ue_scheduling/dl_logical_channel_manager.h"
#include "lib/scheduler/ue_scheduling/ul_logical_channel_manager.h"
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

ul_bsr_indication_message make_sbsr(lcg_id_t lcg_id, unsigned bytes)
{
  ul_bsr_indication_message bsr{};
  bsr.ue_index   = to_du_ue_index(0);
  bsr.crnti      = to_rnti(0x4601);
  bsr.cell_index = to_du_cell_index(0);
  bsr.type       = bsr_format::SHORT_BSR;
  bsr.reported_lcgs.push_back(ul_bsr_lcg_report{lcg_id, bytes});
  return bsr;
}

} // namespace

TEST(dl_logical_channel_test, when_buffer_state_is_zero_no_tx_data_is_pending)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_LCID);
  lch_mng.set_status(lcid, true);
  lch_mng.handle_dl_buffer_status_indication(lcid, 0);

  ASSERT_EQ(lch_mng.pending_bytes(lcid), 0);
  ASSERT_EQ(lch_mng.pending_bytes(), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST(dl_logical_channel_test, buffer_state_indication_has_no_effect_in_inactive_bearer)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid   = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_LCID);
  unsigned                   buf_st = test_rgen::uniform_int<unsigned>(0, 10000);
  lch_mng.handle_dl_buffer_status_indication(lcid, buf_st);

  // SRB0 is active by default.
  if (lcid == LCID_SRB0) {
    ASSERT_EQ(lch_mng.pending_bytes(lcid), get_mac_sdu_required_bytes(buf_st));
  } else {
    ASSERT_EQ(lch_mng.pending_bytes(), 0);
    ASSERT_EQ(lch_mng.pending_bytes(lcid), 0);
  }

  if (lcid == LCID_SRB0) {
    ASSERT_TRUE(lch_mng.has_pending_bytes(LCID_SRB0));
  } else {
    ASSERT_FALSE(lch_mng.has_pending_bytes());
  }
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST(dl_logical_channel_test, buffer_status_indication_updates_tx_pending_bytes)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid = (lcid_t)test_rgen::uniform_int<unsigned>(0, MAX_LCID);
  lch_mng.set_status(lcid, true);

  unsigned buf_st = test_rgen::uniform_int<unsigned>(0, 10000);
  lch_mng.handle_dl_buffer_status_indication(lcid, buf_st);

  if (lcid == LCID_SRB0) {
    // Pending bytes calculation skips SRB0.
    ASSERT_EQ(lch_mng.pending_bytes(lcid), get_mac_sdu_required_bytes(buf_st));
    ASSERT_EQ(lch_mng.has_pending_bytes(lcid), buf_st > 0);
  } else {
    ASSERT_EQ(lch_mng.pending_bytes(), get_mac_sdu_required_bytes(buf_st));
    ASSERT_EQ(lch_mng.has_pending_bytes(), buf_st > 0);
  }
  ASSERT_EQ(lch_mng.pending_bytes(lcid), get_mac_sdu_required_bytes(buf_st));
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST(dl_logical_channel_test, total_pending_bytes_equal_sum_of_logical_channel_pending_bytes)
{
  dl_logical_channel_manager lch_mng;
  std::vector<lcid_t>        lcids = shuffled_lcids();
  std::vector<unsigned>      buf_st_inds;
  for (lcid_t lcid : lcids) {
    unsigned dl_bs = test_rgen::uniform_int<unsigned>(0, 10000);
    buf_st_inds.push_back(get_mac_sdu_required_bytes(dl_bs));
    lch_mng.set_status(lcid, true);
    lch_mng.handle_dl_buffer_status_indication(lcid, dl_bs);
  }

  ASSERT_EQ(lch_mng.pending_bytes() + lch_mng.pending_bytes(LCID_SRB0),
            std::accumulate(buf_st_inds.begin(), buf_st_inds.end(), 0));
  ASSERT_FALSE(lch_mng.has_pending_ces());
  for (unsigned i = 0; i != lcids.size(); ++i) {
    ASSERT_EQ(lch_mng.pending_bytes(lcids[i]), buf_st_inds[i]);
  }
}

TEST(dl_logical_channel_test, mac_ce_indication_updates_tx_pending_bytes)
{
  dl_logical_channel_manager lch_mng;
  const unsigned             dummy_ce_payload = 0;
  lch_mng.handle_mac_ce_indication({.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = dummy_ce_payload});

  ASSERT_TRUE(lch_mng.has_pending_bytes());
  ASSERT_TRUE(lch_mng.has_pending_ces());
  ASSERT_EQ(lch_mng.pending_bytes(),
            lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE);
}

TEST(dl_mac_ce_test, derivation_of_mac_ce_size)
{
  // Note: see TS38.321, Section 6.1.3.
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce(), 6);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::TA_CMD}.sizeof_ce(), 1);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::DRX_CMD}.sizeof_ce(), 0);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::LONG_DRX_CMD}.sizeof_ce(), 0);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::SCELL_ACTIV_1_OCTET}.sizeof_ce(), 1);
  ASSERT_EQ(lcid_dl_sch_t{lcid_dl_sch_t::SCELL_ACTIV_4_OCTET}.sizeof_ce(), 4);
}

TEST(dl_logical_channel_test, no_mac_subpdus_scheduled_if_no_bytes_pending)
{
  dl_logical_channel_manager lch_mng;

  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = lch_mng.allocate_mac_ce(subpdu, 100000);
  allocated_bytes += lch_mng.allocate_mac_sdu(subpdu, 100000);

  ASSERT_EQ(allocated_bytes, 0);
}

TEST(dl_logical_channel_test, mac_ce_is_scheduled_if_tb_has_space)
{
  dl_logical_channel_manager lch_mng;

  lcid_dl_sch_t  ce_lcid          = get_random_dl_mac_ce();
  const unsigned dummy_ce_payload = 0;
  lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload});
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

TEST(dl_logical_channel_test, mac_sdu_is_scheduled_if_tb_has_space)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid = LCID_SRB1;
  lch_mng.set_status(lcid, true);

  unsigned sdu_size = test_rgen::uniform_int<unsigned>(1, 1000);
  lch_mng.handle_dl_buffer_status_indication(lcid, sdu_size);
  unsigned tb_size = test_rgen::uniform_int<unsigned>(0, sdu_size * 2);

  unsigned rem_bytes = tb_size, rem_sdu_size = sdu_size;
  do {
    unsigned       pending_bytes = lch_mng.pending_bytes();
    dl_msg_lc_info subpdu;
    unsigned       allocated_bytes = lch_mng.allocate_mac_sdu(subpdu, rem_bytes);
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
      ASSERT_EQ(get_mac_sdu_required_bytes(rem_sdu_size), lch_mng.pending_bytes())
          << "incorrect calculation of remaining pending tx bytes";
    }

    rem_bytes -= allocated_bytes;
  } while (lch_mng.has_pending_bytes());
}

TEST(dl_logical_channel_test, check_scheduling_of_ue_con_res_id_mac_ce)
{
  dl_logical_channel_manager lch_mng;

  lcid_dl_sch_t  ce_lcid          = lcid_dl_sch_t::UE_CON_RES_ID;
  const unsigned dummy_ce_payload = 0;
  lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload});
  unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  unsigned tb_size               = 10;

  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = lch_mng.allocate_ue_con_res_id_mac_ce(subpdu, tb_size);

  ASSERT_EQ(allocated_bytes, mac_ce_required_bytes);
  ASSERT_EQ(subpdu.lcid, ce_lcid);
  ASSERT_EQ(subpdu.sched_bytes, lcid_dl_sch_t{ce_lcid}.sizeof_ce());
}

TEST(dl_logical_channel_test, pending_bytes_does_not_include_ue_con_res_id_mac_ce)
{
  dl_logical_channel_manager lch_mng;

  lcid_dl_sch_t  ce_lcid          = lcid_dl_sch_t::UE_CON_RES_ID;
  const unsigned dummy_ce_payload = 0;
  lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload});

  ASSERT_EQ(lch_mng.pending_bytes(), 0);
}

TEST(dl_logical_channel_test, pending_ue_con_res_id_ce_bytes_does_not_include_other_mac_ce)
{
  dl_logical_channel_manager lch_mng;

  lcid_dl_sch_t  ce_lcid          = lcid_dl_sch_t::LONG_DRX_CMD;
  const unsigned dummy_ce_payload = 0;
  lch_mng.handle_mac_ce_indication({.ce_lcid = ce_lcid, .ce_payload = dummy_ce_payload});

  ASSERT_EQ(lch_mng.pending_ue_con_res_id_ce_bytes(), 0);
}

TEST(ul_logical_channel_test, when_logical_channel_groups_are_inactive_then_no_ul_bytes_are_pending)
{
  lcg_id_t lcg_id = (lcg_id_t)test_rgen::uniform_int<unsigned>(0, MAX_LCG_ID);

  ul_logical_channel_manager ul_lch_mng;
  ul_logical_channel_manager ul_lch_mng2;

  ul_lch_mng.set_status(lcg_id, false);
  ul_lch_mng2.set_status(lcg_id, true);

  ul_lch_mng.handle_bsr_indication(make_sbsr(lcg_id, 10));
  ul_lch_mng2.handle_bsr_indication(make_sbsr(lcg_id, 10));

  ASSERT_EQ(ul_lch_mng.pending_bytes(), 0);
  ASSERT_GE(ul_lch_mng2.pending_bytes(), 10);

  ul_lch_mng2.set_status(lcg_id, false);
  ASSERT_EQ(ul_lch_mng2.pending_bytes(), 0);
}
