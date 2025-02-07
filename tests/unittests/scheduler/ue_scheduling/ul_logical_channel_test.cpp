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
#include "lib/scheduler/ue_context/ul_logical_channel_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

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
  static constexpr unsigned RLC_HEADER_SIZE_ESTIMATE = 3U;
  // In case of no payload or LCG-ID == 0, there is no need to account for upper layer header.
  if (payload_bytes == 0) {
    return 0;
  }
  if (lcgid == 0) {
    return get_mac_sdu_required_bytes(payload_bytes);
  }
  return get_mac_sdu_required_bytes(payload_bytes + RLC_HEADER_SIZE_ESTIMATE);
}

class ul_logical_channel_tester : public ::testing::Test
{
public:
  logical_channel_config_pool cfg_pool;

  logical_channel_config_list_ptr create_lcid_config(lcg_id_t lcg_id)
  {
    lcid_t lcid  = uint_to_lcid((unsigned)lcg_id);
    auto   cfg   = config_helpers::create_default_logical_channel_config(lcid);
    cfg.lc_group = lcg_id;
    return cfg_pool.create(std::vector<logical_channel_config>{cfg});
  }
  logical_channel_config_list_ptr create_lcid_config(const std::vector<lcg_id_t>& lcgids)
  {
    std::vector<logical_channel_config> configs;
    for (lcg_id_t lcgid : lcgids) {
      lcid_t lcid  = uint_to_lcid((unsigned)lcgid);
      auto   cfg   = config_helpers::create_default_logical_channel_config(lcid);
      cfg.lc_group = lcgid;
      configs.push_back(cfg);
    }
    return cfg_pool.create(configs);
  }
};

} // namespace

TEST_F(ul_logical_channel_tester, when_bsr_is_zero_no_tx_data_is_pending)
{
  const lcg_id_t             lcgid = uint_to_lcg_id(get_random_uint(0, MAX_LCG_ID));
  ul_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, create_lcid_config(lcgid)};

  if (get_random_uint(0, 1) > 0) {
    lch_mng.handle_bsr_indication(make_sbsr(lcgid, 0));
  }

  ASSERT_EQ(lch_mng.pending_bytes(lcgid), 0);
  ASSERT_EQ(lch_mng.pending_bytes(), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
}

TEST_F(ul_logical_channel_tester, bsr_has_no_effect_in_inactive_bearer)
{
  ul_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, cfg_pool.create({})};
  const lcg_id_t             lcgid = uint_to_lcg_id(get_random_uint(1, MAX_LCG_ID));

  lch_mng.handle_bsr_indication(make_sbsr(lcgid, get_random_uint(1, 1000)));

  ASSERT_EQ(lch_mng.pending_bytes(lcgid), 0);
  ASSERT_EQ(lch_mng.pending_bytes(), 0);
  ASSERT_FALSE(lch_mng.has_pending_bytes());
}

TEST_F(ul_logical_channel_tester, bsr_updates_tx_pending_bytes)
{
  const lcg_id_t             lcgid = uint_to_lcg_id(get_random_uint(0, MAX_LCG_ID));
  ul_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, create_lcid_config(lcgid)};

  unsigned bsr = get_random_uint(0, 100);
  lch_mng.handle_bsr_indication(make_sbsr(lcgid, bsr));

  ASSERT_EQ(lch_mng.pending_bytes(), add_header_bytes(lcgid, bsr));
  ASSERT_EQ(lch_mng.pending_bytes(lcgid), add_header_bytes(lcgid, bsr));
  ASSERT_EQ(lch_mng.has_pending_bytes(), bsr > 0);
}

TEST_F(ul_logical_channel_tester, total_pending_bytes_equal_sum_of_logical_channel_pending_bytes)
{
  std::vector<lcg_id_t> lcgids;
  std::vector<unsigned> bsrs;
  for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
    lcgids.push_back(uint_to_lcg_id(i));
  }
  ul_logical_channel_manager lch_mng{subcarrier_spacing::kHz15, this->create_lcid_config(lcgids)};
  for (unsigned i = 0; i != lcgids.size(); ++i) {
    unsigned bsr = get_random_uint(0, 10000);
    bsrs.push_back(add_header_bytes(lcgids[i], bsr));
    lch_mng.handle_bsr_indication(make_sbsr(lcgids[i], bsr));
  }

  ASSERT_EQ(lch_mng.pending_bytes(), std::accumulate(bsrs.begin(), bsrs.end(), 0));
  for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
    lcg_id_t lcgid = uint_to_lcg_id(i);
    ASSERT_EQ(lch_mng.pending_bytes(lcgid), bsrs[i]);
  }
}

TEST_F(ul_logical_channel_tester, when_logical_channel_groups_are_inactive_then_no_ul_bytes_are_pending)
{
  lcg_id_t lcg_id = (lcg_id_t)test_rgen::uniform_int<unsigned>(1, MAX_LCG_ID);

  ul_logical_channel_manager ul_lch_mng{subcarrier_spacing::kHz15, cfg_pool.create({})};
  ul_logical_channel_manager ul_lch_mng2{subcarrier_spacing::kHz15, create_lcid_config(lcg_id)};

  ul_lch_mng.handle_bsr_indication(make_sbsr(lcg_id, 10));
  ul_lch_mng2.handle_bsr_indication(make_sbsr(lcg_id, 10));

  ASSERT_EQ(ul_lch_mng.pending_bytes(), 0);
  ASSERT_GE(ul_lch_mng2.pending_bytes(), 10);

  ul_lch_mng2.configure(cfg_pool.create({}));
  ASSERT_EQ(ul_lch_mng2.pending_bytes(), 0);
}
