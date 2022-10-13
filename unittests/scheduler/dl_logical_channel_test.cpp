/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/ue_scheduling/dl_logical_channel_manager.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

namespace {

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

std::vector<lcid_t> shuffled_lcids()
{
  std::vector<lcid_t> lcids(MAX_LCID);
  for (unsigned i = 0; i != lcids.size(); ++i) {
    lcids[i] = (lcid_t)i;
  }
  std::shuffle(lcids.begin(), lcids.end(), g);
  return lcids;
}

lcid_dl_sch_t get_random_dl_mac_ce()
{
  return (lcid_dl_sch_t)get_random_uint((unsigned)lcid_dl_sch_t::SCELL_ACTIV_4_OCTET,
                                        (unsigned)lcid_dl_sch_t::UE_CON_RES_ID);
}

} // namespace

TEST(dl_logical_channel_test, when_buffer_state_is_zero_no_tx_data_is_pending)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid = (lcid_t)get_random_uint(0, MAX_LCID);
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
  lcid_t                     lcid   = (lcid_t)get_random_uint(0, MAX_LCID);
  unsigned                   buf_st = get_random_uint(0, 10000);
  lch_mng.handle_dl_buffer_status_indication(lcid, buf_st);

  // SRB0 is active by default.
  ASSERT_EQ(lch_mng.pending_bytes(), lcid == LCID_SRB0 ? get_mac_sdu_required_bytes(buf_st) : 0);
  ASSERT_EQ(lch_mng.pending_bytes(lcid), lcid == LCID_SRB0 ? get_mac_sdu_required_bytes(buf_st) : 0);
  if (lcid == LCID_SRB0) {
    ASSERT_TRUE(lch_mng.has_pending_bytes());
  } else {
    ASSERT_FALSE(lch_mng.has_pending_bytes());
  }
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST(dl_logical_channel_test, buffer_status_indication_updates_tx_pending_bytes)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid = (lcid_t)get_random_uint(0, MAX_LCID);
  lch_mng.set_status(lcid, true);

  unsigned buf_st = get_random_uint(0, 10000);
  lch_mng.handle_dl_buffer_status_indication(lcid, buf_st);

  ASSERT_EQ(lch_mng.pending_bytes(), get_mac_sdu_required_bytes(buf_st));
  ASSERT_EQ(lch_mng.pending_bytes(lcid), get_mac_sdu_required_bytes(buf_st));
  ASSERT_EQ(lch_mng.has_pending_bytes(), buf_st > 0);
  ASSERT_FALSE(lch_mng.has_pending_ces());
}

TEST(dl_logical_channel_test, total_pending_bytes_equal_sum_of_logical_channel_pending_bytes)
{
  dl_logical_channel_manager lch_mng;
  std::vector<lcid_t>        lcids = shuffled_lcids();
  std::vector<unsigned>      buf_st_inds;
  for (lcid_t lcid : lcids) {
    unsigned dl_bs = get_random_uint(0, 10000);
    buf_st_inds.push_back(get_mac_sdu_required_bytes(dl_bs));
    lch_mng.set_status(lcid, true);
    lch_mng.handle_dl_buffer_status_indication(lcid, dl_bs);
  }

  ASSERT_EQ(lch_mng.pending_bytes(), std::accumulate(buf_st_inds.begin(), buf_st_inds.end(), 0));
  ASSERT_FALSE(lch_mng.has_pending_ces());
  for (unsigned i = 0; i != lcids.size(); ++i) {
    ASSERT_EQ(lch_mng.pending_bytes(lcids[i]), buf_st_inds[i]);
  }
}

TEST(dl_logical_channel_test, mac_ce_indication_updates_tx_pending_bytes)
{
  dl_logical_channel_manager lch_mng;

  lch_mng.handle_mac_ce_indication(lcid_dl_sch_t::UE_CON_RES_ID);

  ASSERT_TRUE(lch_mng.has_pending_bytes());
  ASSERT_TRUE(lch_mng.has_pending_ces());
  ASSERT_EQ(lch_mng.pending_bytes(),
            lcid_dl_sch_t{lcid_dl_sch_t::UE_CON_RES_ID}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE);
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

  lcid_dl_sch_t ce_lcid = get_random_dl_mac_ce();
  lch_mng.handle_mac_ce_indication(ce_lcid);
  unsigned mac_ce_required_bytes = lcid_dl_sch_t{ce_lcid}.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE;
  unsigned tb_size               = get_random_uint(0, 50);

  dl_msg_lc_info subpdu;
  unsigned       allocated_bytes = lch_mng.allocate_mac_ce(subpdu, tb_size);

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

  unsigned sdu_size = get_random_uint(1, 1000);
  lch_mng.handle_dl_buffer_status_indication(lcid, sdu_size);
  unsigned tb_size = get_random_uint(0, sdu_size * 2);

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
