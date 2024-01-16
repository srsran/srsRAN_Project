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

#include "lib/scheduler/ue_scheduling/dl_logical_channel_manager.h"
#include "lib/scheduler/ue_scheduling/ta_manager.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_helper.h"
#include <gtest/gtest.h>

using namespace srsran;

class ta_manager_tester : public ::testing::TestWithParam<duplex_mode>
{
protected:
  ta_manager_tester() :
    ul_scs(GetParam() == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30),
    ta_mgr(expert_cfg.ue, ul_scs, &dl_lc_ch_mgr),
    current_sl(to_numerology_value(ul_scs), test_rgen::uniform_int<unsigned>(0, 10239))
  {
    run_slot();
  }

  /// Computes the N_TA update i.e. N_TA_new - N_TA_old value in T_C units, which will result in new TA command
  /// (new_ta_cmd) being computed by the TA manager.
  int64_t compute_n_ta_diff_leading_to_new_ta_cmd(uint8_t new_ta_cmd)
  {
    return static_cast<int64_t>(std::round(((new_ta_cmd - 31) * 16 * 64) / pow2(to_numerology_value(ul_scs))));
  }

  void run_slot()
  {
    current_sl++;
    ta_mgr.slot_indication(current_sl);
  }

  optional<dl_msg_lc_info> fetch_ta_cmd_mac_ce_allocation()
  {
    static const lcid_dl_sch_t lcid            = lcid_dl_sch_t::TA_CMD;
    static const unsigned      remaining_bytes = lcid.sizeof_ce() + FIXED_SIZED_MAC_CE_SUBHEADER_SIZE + 3;
    dl_msg_lc_info             subpdu{};
    if (dl_lc_ch_mgr.allocate_mac_ce(subpdu, remaining_bytes) > 0) {
      return subpdu;
    }
    return {};
  }

  subcarrier_spacing         ul_scs;
  scheduler_expert_config    expert_cfg = config_helpers::make_default_scheduler_expert_config();
  dl_logical_channel_manager dl_lc_ch_mgr;
  ta_manager                 ta_mgr;
  slot_point                 current_sl;
};

TEST_P(ta_manager_tester, ta_cmd_is_not_triggered_when_reported_ul_n_ta_update_indication_has_low_sinr)
{
  const uint8_t      new_ta_cmd = 33;
  static const float ul_sinr    = expert_cfg.ue.ta_update_measurement_ul_sinr_threshold - 10;
  ta_mgr.handle_ul_n_ta_update_indication(0, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);

  for (unsigned count = 0; count < expert_cfg.ue.ta_measurement_slot_period * 2; ++count) {
    run_slot();
    ASSERT_TRUE(not fetch_ta_cmd_mac_ce_allocation().has_value()) << "TA command should not be triggered";
  }
}

TEST_P(ta_manager_tester, ta_cmd_is_not_triggered_when_no_ul_n_ta_update_indication_are_reported)
{
  for (unsigned count = 0; count < expert_cfg.ue.ta_measurement_slot_period * 2; ++count) {
    run_slot();
    ASSERT_TRUE(not fetch_ta_cmd_mac_ce_allocation().has_value()) << "TA command should not be triggered";
  }
}

TEST_P(ta_manager_tester, ta_cmd_is_successfully_triggered)
{
  const uint8_t      new_ta_cmd = 33;
  static const float ul_sinr    = expert_cfg.ue.ta_update_measurement_ul_sinr_threshold + 10;
  ta_mgr.handle_ul_n_ta_update_indication(0, compute_n_ta_diff_leading_to_new_ta_cmd(new_ta_cmd), ul_sinr);

  optional<dl_msg_lc_info> ta_cmd_mac_ce_alloc;
  for (unsigned count = 0; count < expert_cfg.ue.ta_measurement_slot_period * 2; ++count) {
    run_slot();
    ta_cmd_mac_ce_alloc = fetch_ta_cmd_mac_ce_allocation();
    if (ta_cmd_mac_ce_alloc.has_value()) {
      break;
    }
  }
  ASSERT_TRUE(ta_cmd_mac_ce_alloc.has_value()) << "Missing TA command CE allocation";
  ASSERT_TRUE(ta_cmd_mac_ce_alloc->lcid == lcid_dl_sch_t::TA_CMD) << "TA command is not be triggered";
  ASSERT_TRUE(variant_holds_alternative<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload))
      << "TA command CE payload is absent";
  auto ta_cmd_ce = variant_get<ta_cmd_ce_payload>(ta_cmd_mac_ce_alloc->ce_payload);
  ASSERT_EQ(ta_cmd_ce.ta_cmd, new_ta_cmd) << "New TA command does not match the expected TA command value";
}

INSTANTIATE_TEST_SUITE_P(ue_scheduler_test, ta_manager_tester, testing::Values(duplex_mode::TDD, duplex_mode::FDD));
