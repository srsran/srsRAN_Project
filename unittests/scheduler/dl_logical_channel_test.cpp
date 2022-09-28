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

} // namespace

TEST(dl_logical_channel_test, when_buffer_state_is_zero_no_tx_data_is_pending)
{
  dl_logical_channel_manager lch_mng;
  lcid_t                     lcid = (lcid_t)get_random_uint(0, MAX_LCID);
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

  ASSERT_EQ(lch_mng.pending_bytes(), 0);
  ASSERT_EQ(lch_mng.pending_bytes(lcid), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
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
    ASSERT_EQ(lch_mng.has_pending_bytes(), buf_st_inds[i] > 0);
  }
}
