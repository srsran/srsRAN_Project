/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    cell_config_builder_params builder_params{};
    if (duplx_mode == duplex_mode::TDD) {
      builder_params.dl_arfcn       = 520002;
      builder_params.scs_common     = subcarrier_spacing::kHz30;
      builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);
      builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
      const unsigned nof_crbs       = band_helper::get_n_rbs_from_bw(
          builder_params.channel_bw_mhz, builder_params.scs_common, band_helper::get_freq_range(*builder_params.band));
      static const uint8_t                              ss0_idx = 0;
      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location(builder_params.dl_arfcn,
                                                      *builder_params.band,
                                                      nof_crbs,
                                                      builder_params.scs_common,
                                                      builder_params.scs_common,
                                                      ss0_idx);
      if (!ssb_freq_loc.has_value()) {
        report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", builder_params.pci);
      }
      builder_params.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      builder_params.k_ssb             = ssb_freq_loc->k_ssb;
      builder_params.coreset0_index    = ssb_freq_loc->coreset0_idx;
    }
    add_cell(test_helpers::make_default_sched_cell_configuration_request(builder_params));

    // Create a UE with a DRB active.
    auto ue_cfg     = test_helpers::create_default_sched_ue_creation_request({}, {});
    ue_cfg.ue_index = ue_index;
    ue_cfg.crnti    = rnti;
    scheduler_test_bench::add_ue(ue_cfg, true);
  }

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
  ASSERT_FALSE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_res) != nullptr; }));
}

// ------------------------------------------------------------------------------------------------------------------ //

struct conres_test_params {
  lcid_t      msg4_lcid;
  duplex_mode duplx_mode;
};

class scheduler_con_res_msg4_test : public base_scheduler_conres_test,
                                    public ::testing::TestWithParam<conres_test_params>
{
public:
  scheduler_con_res_msg4_test() : base_scheduler_conres_test(GetParam().duplx_mode), params(GetParam()) {}

  conres_test_params params;
};

TEST_P(scheduler_con_res_msg4_test,
       when_conres_ce_and_srb_pdu_are_enqueued_then_tc_rnti_is_used_and_multiplexing_with_csi_rs_is_avoided)
{
  const static unsigned msg4_size = 128;
  ASSERT_FALSE(this->cell_cfg_list[0].nzp_csi_rs_list.empty()) << "This test assumes a setup with NZP CSI-RS enabled";
  ASSERT_FALSE(this->cell_cfg_list[0].zp_csi_rs_list.empty()) << "This test assumes a setup with ZP CSI-RS enabled";

  // Enqueue ConRes CE.
  this->sched->handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_index, lcid_dl_sch_t::UE_CON_RES_ID});

  // Enqueue Msg4 in SRB0/SRB1.
  this->push_dl_buffer_state(dl_buffer_state_indication_message{this->ue_index, params.msg4_lcid, msg4_size});

  // Ensure the Msg4 is scheduled together with the ConRes CE.
  ASSERT_TRUE(this->run_slot_until([this]() { return find_ue_pdsch(rnti, *this->last_sched_res) != nullptr; }));
  const dl_msg_alloc* msg4_alloc = find_ue_pdsch(rnti, *this->last_sched_res);
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
  ASSERT_TRUE(this->last_sched_res->dl.csi_rs.empty());
}

INSTANTIATE_TEST_SUITE_P(scheduler_con_res_msg4_test,
                         scheduler_con_res_msg4_test,
                         ::testing::Values(conres_test_params{LCID_SRB0, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB0, duplex_mode::TDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::FDD},
                                           conres_test_params{LCID_SRB1, duplex_mode::TDD}));
