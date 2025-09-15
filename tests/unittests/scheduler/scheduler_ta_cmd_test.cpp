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

#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_simulator.h"
#include "srsran/ran/duplex_mode.h"
#include <gtest/gtest.h>

using namespace srsran;

class scheduler_ta_cmd_tester : public scheduler_test_simulator, public ::testing::TestWithParam<duplex_mode>
{
protected:
  scheduler_ta_cmd_tester() :
    scheduler_test_simulator(4, GetParam() == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30)
  {
    params = GetParam() == duplex_mode::FDD ? cell_config_builder_profiles::fdd() : cell_config_builder_profiles::tdd();

    // Add Cell.
    this->add_cell(sched_config_helper::make_default_sched_cell_configuration_request(params));

    // Add UE
    this->add_ue(sched_config_helper::create_default_sched_ue_creation_request(params, {ue_drb_lcid}));
  }

  uci_indication create_uci_indication(slot_point        uci_sl,
                                       du_ue_index_t     ue_index,
                                       const pucch_info& pucch_pdu,
                                       float             ul_sinr,
                                       unsigned          time_advance_offset_in_tc)
  {
    uci_indication uci_ind{};
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.slot_rx    = uci_sl;
    uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_index, pucch_pdu));
    if (auto* f0_f1 = std::get_if<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(&uci_ind.ucis[0].pdu)) {
      f0_f1->ul_sinr_dB          = ul_sinr;
      f0_f1->time_advance_offset = phy_time_unit::from_units_of_Tc(time_advance_offset_in_tc);
    } else if (auto* f2_f3_f4 =
                   std::get_if<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(&uci_ind.ucis[0].pdu)) {
      f2_f3_f4->ul_sinr_dB          = ul_sinr;
      f2_f3_f4->time_advance_offset = phy_time_unit::from_units_of_Tc(time_advance_offset_in_tc);
    } else {
      report_fatal_error("Unsupported PUCCH format");
    }
    return uci_ind;
  }

  bool is_ta_cmd_scheduled()
  {
    bool is_scheduled = false;
    if (cell_cfg_list[0].is_dl_enabled(this->last_result_slot())) {
      for (const auto& ue_grant : last_sched_res_list[to_du_cell_index(0)]->dl.ue_grants) {
        for (const auto& tb_info : ue_grant.tb_list) {
          is_scheduled =
              std::any_of(tb_info.lc_chs_to_sched.begin(),
                          tb_info.lc_chs_to_sched.end(),
                          [](const dl_msg_lc_info& lc_info) { return lc_info.lcid == lcid_dl_sch_t::TA_CMD; });
          break;
        }
      }
    }
    return is_scheduled;
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;

  cell_config_builder_params params;
};

TEST_P(scheduler_ta_cmd_tester, ta_cmd_is_scheduled)
{
  // Enqueue enough bytes for continuous DL tx.
  const dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

  static const float    ul_sinr                         = 34.0F;
  static const unsigned min_timing_advance_offset_in_tc = 2048;
  static const unsigned max_timing_advance_offset_in_tc = 4096;

  bool is_ta_cmd_scheduled_for_ue = false;
  for (unsigned count = 0; count != 100; ++count) {
    this->run_slot();

    if (is_ta_cmd_scheduled()) {
      is_ta_cmd_scheduled_for_ue = true;
      break;
    }

    for (const pucch_info& pucch : this->last_sched_res_list[to_du_cell_index(0)]->ul.pucchs) {
      if (pucch.format() == pucch_format::FORMAT_1 and pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
        // Skip SRs for this test.
        continue;
      }
      this->sched->handle_uci_indication(create_uci_indication(
          this->last_result_slot(),
          ue_idx,
          pucch,
          ul_sinr,
          test_rgen::uniform_int<unsigned>(min_timing_advance_offset_in_tc, max_timing_advance_offset_in_tc)));
    }
  }

  ASSERT_TRUE(is_ta_cmd_scheduled_for_ue) << "TA command is not scheduled";
}

INSTANTIATE_TEST_SUITE_P(scheduler_test, scheduler_ta_cmd_tester, testing::Values(duplex_mode::TDD, duplex_mode::FDD));
