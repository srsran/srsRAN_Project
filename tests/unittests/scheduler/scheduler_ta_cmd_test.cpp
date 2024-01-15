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

#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_bench.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_helper.h"
#include <gtest/gtest.h>

using namespace srsran;

class scheduler_ta_cmd_tester : public scheduler_test_bench, public ::testing::TestWithParam<duplex_mode>
{
protected:
  scheduler_ta_cmd_tester() :
    scheduler_test_bench(4, GetParam() == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30)
  {
    // Add Cell.
    this->add_cell([this, duplex_mode = GetParam()]() {
      if (duplex_mode == duplex_mode::TDD) {
        params.scs_common     = subcarrier_spacing::kHz30;
        params.dl_arfcn       = 520002;
        params.band           = nr_band::n41;
        params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
      } else {
        params.scs_common     = subcarrier_spacing::kHz15;
        params.dl_arfcn       = 365000;
        params.band           = nr_band::n3;
        params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
      }
      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          params.channel_bw_mhz, params.scs_common, band_helper::get_freq_range(*params.band));
      static const uint8_t                              ss0_idx = 0;
      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location(params.dl_arfcn,
                                                      *params.band,
                                                      nof_crbs,
                                                      params.scs_common,
                                                      params.scs_common,
                                                      ss0_idx,
                                                      params.max_coreset0_duration);
      if (!ssb_freq_loc.has_value()) {
        report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", params.pci);
      }
      params.offset_to_point_a = (*ssb_freq_loc).offset_to_point_A;
      params.k_ssb             = (*ssb_freq_loc).k_ssb;
      params.coreset0_index    = (*ssb_freq_loc).coreset0_idx;
      params.csi_rs_enabled    = true;

      sched_cell_configuration_request_message sched_cfg_req =
          test_helpers::make_default_sched_cell_configuration_request(params);

      return sched_cfg_req;
    }());

    // Add UE
    this->add_ue(test_helpers::create_default_sched_ue_creation_request(params, {ue_drb_lcid}));
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
    uci_ind.ucis.resize(1);
    uci_ind.ucis[0].crnti    = pucch_pdu.crnti;
    uci_ind.ucis[0].ue_index = ue_index;
    switch (pucch_pdu.format) {
      case pucch_format::FORMAT_1: {
        uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f1{};
        f1.harqs.resize(pucch_pdu.format_1.harq_ack_nof_bits);
        std::fill(f1.harqs.begin(), f1.harqs.end(), mac_harq_ack_report_status::ack);
        f1.ul_sinr             = ul_sinr;
        f1.time_advance_offset = phy_time_unit::from_units_of_Tc(time_advance_offset_in_tc);
        uci_ind.ucis[0].pdu    = f1;
      } break;
      case pucch_format::FORMAT_2: {
        uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2{};
        f2.harqs.resize(pucch_pdu.format_2.harq_ack_nof_bits);
        std::fill(f2.harqs.begin(), f2.harqs.end(), mac_harq_ack_report_status::ack);
        f2.ul_sinr             = ul_sinr;
        f2.time_advance_offset = phy_time_unit::from_units_of_Tc(time_advance_offset_in_tc);
        uci_ind.ucis[0].pdu    = f2;
        if (pucch_pdu.format_2.csi_part1_bits > 0) {
          f2.csi.emplace();
          f2.csi->first_tb_wideband_cqi = 15;
        }
      } break;
      default:
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
      if (pucch.format == pucch_format::FORMAT_1 and pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
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
