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

#include "lib/scheduler/ue_scheduling/ul_logical_channel_manager.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace {

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

ul_bsr_indication_message make_sbsr(lcg_id_t lcgid, unsigned bsr)
{
  ul_bsr_indication_message msg;
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);
  msg.ue_index   = to_du_ue_index(0);
  msg.type       = bsr_format::SHORT_BSR;
  msg.reported_lcgs.push_back(ul_bsr_lcg_report{lcgid, bsr});
  return msg;
}

unsigned add_header_bytes(lcg_id_t lcgid, unsigned payload_bytes)
{
  // Estimate of the number of bytes required for the upper layer header.
  constexpr static unsigned RLC_HEADER_SIZE_ESTIMATE = 3U;
  // In case of no payload or LCG-ID == 0, there is no need to account for upper layer header.
  if (payload_bytes == 0) {
    return 0;
  }
  if (lcgid == 0) {
    return get_mac_sdu_required_bytes(payload_bytes);
  }
  return get_mac_sdu_required_bytes(payload_bytes + RLC_HEADER_SIZE_ESTIMATE);
}

} // namespace

TEST(ul_logical_channel_test, when_bsr_is_zero_no_tx_data_is_pending)
{
  ul_logical_channel_manager lch_mng;
  lcg_id_t                   lcgid = (lcg_id_t)get_random_uint(0, MAX_LCG_ID);
  lch_mng.set_status(lcgid, true);

  if (get_random_uint(0, 1) > 0) {
    lch_mng.handle_bsr_indication(make_sbsr(lcgid, 0));
  }

  ASSERT_EQ(lch_mng.pending_bytes(lcgid), 0);
  ASSERT_EQ(lch_mng.pending_bytes(), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
}

TEST(ul_logical_channel_test, bsr_has_no_effect_in_inactive_bearer)
{
  ul_logical_channel_manager lch_mng;
  lcg_id_t                   lcgid = (lcg_id_t)get_random_uint(1, MAX_LCG_ID);

  lch_mng.handle_bsr_indication(make_sbsr(lcgid, get_random_uint(1, 1000)));

  ASSERT_EQ(lch_mng.pending_bytes(lcgid), 0);
  ASSERT_EQ(lch_mng.pending_bytes(), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
}

TEST(ul_logical_channel_test, bsr_updates_tx_pending_bytes)
{
  ul_logical_channel_manager lch_mng;
  lcg_id_t                   lcgid = (lcg_id_t)get_random_uint(0, MAX_LCG_ID);
  lch_mng.set_status(lcgid, true);
  unsigned bsr = get_random_uint(0, 100);
  lch_mng.handle_bsr_indication(make_sbsr(lcgid, bsr));

  ASSERT_EQ(lch_mng.pending_bytes(), add_header_bytes(lcgid, bsr));
  ASSERT_EQ(lch_mng.pending_bytes(lcgid), add_header_bytes(lcgid, bsr));
  ASSERT_EQ(lch_mng.has_pending_bytes(), bsr > 0);
}

TEST(ul_logical_channel_test, total_pending_bytes_equal_sum_of_logical_channel_pending_bytes)
{
  ul_logical_channel_manager lch_mng;
  std::vector<unsigned>      bsrs;
  for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
    lcg_id_t lcgid = uint_to_lcg_id(i);
    unsigned bsr   = get_random_uint(0, 10000);
    bsrs.push_back(add_header_bytes(lcgid, bsr));
    lch_mng.set_status(lcgid, true);
    lch_mng.handle_bsr_indication(make_sbsr(lcgid, bsr));
  }

  ASSERT_EQ(lch_mng.pending_bytes(), std::accumulate(bsrs.begin(), bsrs.end(), 0));
  for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
    lcg_id_t lcgid = uint_to_lcg_id(i);
    ASSERT_EQ(lch_mng.pending_bytes(lcgid), bsrs[i]);
  }
}
