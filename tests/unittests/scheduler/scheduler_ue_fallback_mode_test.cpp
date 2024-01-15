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

/// \file
/// \brief In this file, we test the correct behaviour of the scheduler when handling UE contention resolution, and
/// TC-RNTI to C-RNTI transitions.

#include "test_utils/config_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_bench.h"
#include <gtest/gtest.h>

using namespace srsran;

class base_scheduler_conres_test : public scheduler_test_bench
{
public:
  base_scheduler_conres_test(duplex_mode duplx_mode = duplex_mode::FDD) :
    scheduler_test_bench(4, duplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30)
  {
    if (duplx_mode == duplex_mode::TDD) {
      builder_params.dl_arfcn       = 520002;
      builder_params.scs_common     = subcarrier_spacing::kHz30;
      builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);
      builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz10;
      const unsigned nof_crbs       = band_helper::get_n_rbs_from_bw(
          builder_params.channel_bw_mhz, builder_params.scs_common, band_helper::get_freq_range(*builder_params.band));
      static const uint8_t                              ss0_idx = 0;
      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location(builder_params.dl_arfcn,
                                                      *builder_params.band,
                                                      nof_crbs,
                                                      builder_params.scs_common,
                                                      builder_params.scs_common,
                                                      ss0_idx,
                                                      builder_params.max_coreset0_duration);
      if (!ssb_freq_loc.has_value()) {
        report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", builder_params.pci);
      }
      builder_params.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      builder_params.k_ssb             = ssb_freq_loc->k_ssb;
      builder_params.coreset0_index    = ssb_freq_loc->coreset0_idx;
    }
    // Create cell config with space for two PDCCHs in the SearchSpace#1.
    sched_cell_configuration_request_message cell_cfg_req =
        test_helpers::make_default_sched_cell_configuration_request(builder_params);
    cell_cfg_req.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].set_non_ss0_nof_candidates(
        std::array<uint8_t, 5>{0, 0, 2, 0, 0});
    add_cell(cell_cfg_req);

    srsran_assert(not this->cell_cfg_list[0].nzp_csi_rs_list.empty(),
                  "This test assumes a setup with NZP CSI-RS enabled");
    srsran_assert(not this->cell_cfg_list[0].zp_csi_rs_list.empty(),
                  "This test assumes a setup with ZP CSI-RS enabled");

    // Create a UE with a DRB active.
    auto ue_cfg               = test_helpers::create_default_sched_ue_creation_request(builder_params, {});
    ue_cfg.ue_index           = ue_index;
    ue_cfg.crnti              = rnti;
    ue_cfg.starts_in_fallback = true;
    scheduler_test_bench::add_ue(ue_cfg, true);
  }

  ~base_scheduler_conres_test() { srslog::flush(); }

  cell_config_builder_params builder_params{};

  du_ue_index_t ue_index = to_du_ue_index(0);
  rnti_t        rnti     = to_rnti(0x4601);
};

class scheduler_conres_without_pdu_test : public base_scheduler_conres_test, public ::testing::Test
{};

TEST_F(scheduler_conres_without_pdu_test,
       when_conres_ce_is_enqueued_and_no_msg4_is_enqueued_then_pdsch_is_not_scheduled)
{
  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Ensure the ConRes CEU is not scheduled without a Msg4 SDU.
  ASSERT_FALSE(this->run_slot_until(
      [this]() { return find_ue_pdsch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]) != nullptr; }));
}

// ------------------------------------------------------------------------------------------------------------------ //

struct conres_test_params {
  lcid_t      msg4_lcid;
  duplex_mode duplx_mode;
};

/// Formatter for test params.
void PrintTo(const conres_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("LCID={}, mode={}", value.msg4_lcid, value.duplx_mode == duplex_mode::TDD ? "TDD" : "FDD");
}

class scheduler_con_res_msg4_test : public base_scheduler_conres_test,
                                    public ::testing::TestWithParam<conres_test_params>
{
public:
  scheduler_con_res_msg4_test() : base_scheduler_conres_test(GetParam().duplx_mode), params(GetParam()) {}

  void enqueue_random_number_of_rach_indications()
  {
    rach_indication_message rach_ind{to_du_cell_index(0), next_slot_rx(), {{0, 0, {}}}};
    unsigned                nof_preambles = test_rgen::uniform_int<unsigned>(1, 10);
    for (unsigned i = 0; i != nof_preambles; ++i) {
      rach_ind.occasions[0].preambles.push_back({i, to_rnti((uint16_t)rnti + 1 + i), phy_time_unit{}});
    }
    this->sched->handle_rach_indication(rach_ind);
  }

  conres_test_params params;
};

TEST_P(scheduler_con_res_msg4_test,
       when_conres_ce_and_srb_pdu_are_enqueued_then_tc_rnti_is_used_and_multiplexing_with_csi_rs_is_avoided)
{
  const static unsigned msg4_size = 128;

  // Enqueue several RACH indications, so that RARs that need to be scheduled may fight for RB space with the Msg4.
  enqueue_random_number_of_rach_indications();

  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Enqueue Msg4 in SRB0/SRB1.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Ensure the Msg4 is scheduled together with the ConRes CE.
  ASSERT_TRUE(this->run_slot_until(
      [this]() { return find_ue_pdsch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]) != nullptr; }));
  const dl_msg_alloc* msg4_alloc = find_ue_pdsch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]);
  ASSERT_EQ(msg4_alloc->tb_list.size(), 1);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched.size(), 2);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[0].lcid, lcid_dl_sch_t::UE_CON_RES_ID);
  ASSERT_EQ(msg4_alloc->tb_list[0].lc_chs_to_sched[1].lcid, params.msg4_lcid);

  // In case of Msg4 with SRB0, ensure that enough bytes are scheduled to fit the full Msg4, as RLC-TM doesn't support
  // segmentation.
  if (params.msg4_lcid == LCID_SRB0) {
    ASSERT_GE(msg4_alloc->tb_list[0].lc_chs_to_sched[1].sched_bytes, msg4_size);
  }

  // Ensure TC-RNTI is used for Msg4.
  ASSERT_EQ(find_ue_dl_pdcch(rnti)->dci.type, dci_dl_rnti_config_type::tc_rnti_f1_0);

  // Ensure no PDSCH multiplexing with CSI-RS.
  ASSERT_TRUE(this->last_sched_res_list[to_du_cell_index(0)]->dl.csi_rs.empty());
}

TEST_P(scheduler_con_res_msg4_test, while_ue_is_in_fallback_then_common_pucch_is_used)
{
  const static unsigned msg4_size = 128;
  // TODO: Increase the crnti message size, once PUCCH scheduler handles multiple HARQ-ACKs falling in the same slot
  //  in fallback mode.
  const static unsigned crnti_msg_size = 8;

  // Enqueue ConRes CE + Msg4.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Wait for ConRes + Msg4 PDCCH, PDSCH and PUCCH to be scheduled.
  ASSERT_TRUE(this->run_slot_until([this]() {
    auto* pucch = find_ue_pucch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]);
    return pucch != nullptr and pucch->format == pucch_format::FORMAT_1 and pucch->format_1.harq_ack_nof_bits > 0;
  }));

  // Enqueue SRB1 data.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, LCID_SRB1, crnti_msg_size});

  // Ensure common resources for PDCCH and PDSCH are used rather than UE-dedicated.
  ASSERT_TRUE(this->run_slot_until(
      [this]() { return find_ue_pdsch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]) != nullptr; }));
  const pdcch_dl_information& dl_pdcch = *find_ue_dl_pdcch(rnti);
  ASSERT_EQ(dl_pdcch.dci.type, dci_dl_rnti_config_type::c_rnti_f1_0) << "Invalid format used for UE in fallback mode";
  ASSERT_EQ(dl_pdcch.ctx.coreset_cfg->id, to_coreset_id(0));
  const dl_msg_alloc& pdsch = *find_ue_pdsch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]);
  ASSERT_EQ(pdsch.pdsch_cfg.dci_fmt, dci_dl_format::f1_0);

  // Ensure common PUCCH resources are used.
  ASSERT_TRUE(this->run_slot_until([this]() {
    auto* pucch = find_ue_pucch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]);
    if (pucch == nullptr) {
      return false;
    }
    return pucch->format == pucch_format::FORMAT_1 and pucch->format_1.harq_ack_nof_bits > 0;
  }));
  // TODO: Once PUCCH scheduler avoids multiplexing SR and HARQ-ACK for common PUCCH resources, uncomment the following.
  //  ASSERT_EQ(std::count_if(this->last_sched_res->ul.pucchs.begin(),
  //                          this->last_sched_res->ul.pucchs.end(),
  //                          [this](const pucch_info& pucch) { return pucch.crnti == rnti; }),
  //            1)
  //      << "In case of common PUCCH scheduling, multiplexing with SR or CSI should be avoided";
  const pucch_info& pucch = *find_ue_pucch(rnti, *this->last_sched_res_list[to_du_cell_index(0)]);
  ASSERT_EQ(pucch.format, pucch_format::FORMAT_1);
  ASSERT_EQ(pucch.format_1.sr_bits, sr_nof_bits::no_sr);
  ASSERT_FALSE(pucch.resources.second_hop_prbs.empty()) << "For common PUCCH resources, second hop is used";
}

INSTANTIATE_TEST_SUITE_P(scheduler_con_res_msg4_test,
                         scheduler_con_res_msg4_test,
                         ::testing::Values(conres_test_params{LCID_SRB0, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB0, duplex_mode::TDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::TDD}));
