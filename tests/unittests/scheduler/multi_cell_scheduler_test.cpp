/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_bench.h"
#include "srsran/ran/duplex_mode.h"
#include <gtest/gtest.h>

using namespace srsran;

struct multi_cell_scheduler_test_params {
  duplex_mode dplx_mode;
  unsigned    nof_cells = 2;
};

class base_multi_cell_scheduler_tester : public scheduler_test_bench
{
protected:
  base_multi_cell_scheduler_tester(const multi_cell_scheduler_test_params& test_params) :
    scheduler_test_bench(4,
                         test_params.dplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15
                                                                   : subcarrier_spacing::kHz30)
  {
    cell_config_builder_params params{};
    params.scs_common =
        test_params.dplx_mode == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30;
    params.dl_arfcn         = test_params.dplx_mode == duplex_mode::FDD ? 530000 : 520002;
    params.band             = band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
    params.channel_bw_mhz   = bs_channel_bandwidth_fr1::MHz20;
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
    params.offset_to_point_a   = (*ssb_freq_loc).offset_to_point_A;
    params.k_ssb               = (*ssb_freq_loc).k_ssb;
    params.coreset0_index      = (*ssb_freq_loc).coreset0_idx;
    params.search_space0_index = ss0_idx;

    // Add Cells.
    for (unsigned cell_idx = 0; cell_idx < test_params.nof_cells; ++cell_idx) {
      cell_cfg_builder_params_list.emplace_back(params);
      cell_cfg_builder_params_list.back().pci = cell_idx + 1;

      auto sched_cell_cfg_req             = test_helpers::make_default_sched_cell_configuration_request(params);
      sched_cell_cfg_req.cell_group_index = static_cast<du_cell_group_index_t>(cell_idx);
      sched_cell_cfg_req.cell_index       = to_du_cell_index(cell_idx);
      this->add_cell(sched_cell_cfg_req);
    }
  }

  void run_slot_all_cells()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      last_sched_res_list[cell_idx] = &sched->slot_indication(next_slot, to_du_cell_index(cell_idx));
      test_scheduler_result_consistency(cell_cfg_list[cell_idx], next_slot, *last_sched_res_list[cell_idx]);
    }
    ++next_slot;
  }

  void add_ue(uint16_t cell_idx, uint16_t ue_idx)
  {
    // Add UE
    auto ue_cfg =
        test_helpers::create_default_sched_ue_creation_request(cell_cfg_builder_params_list[cell_idx], {LCID_MIN_DRB});
    ue_cfg.ue_index                                 = to_du_ue_index(ue_idx);
    ue_cfg.crnti                                    = to_rnti(0x4601 + ue_idx);
    (*ue_cfg.cfg.cells)[0].serv_cell_cfg.cell_index = to_du_cell_index(cell_idx);
    (*ue_cfg.cfg.cells)[0].serv_cell_idx            = to_serv_cell_index(cell_idx);

    scheduler_test_bench::add_ue(ue_cfg);
  }

  rach_indication_message::preamble create_preamble()
  {
    static unsigned       next_rnti = test_rgen::uniform_int<unsigned>(MIN_CRNTI, MAX_CRNTI);
    static const unsigned rnti_inc  = test_rgen::uniform_int<unsigned>(1, 5);

    rach_indication_message::preamble preamble{};
    preamble.preamble_id = test_rgen::uniform_int<unsigned>(0, 63);
    preamble.time_advance =
        phy_time_unit::from_seconds(std::uniform_real_distribution<double>{0, 2005e-6}(test_rgen::get()));
    preamble.tc_rnti = to_rnti(next_rnti);
    next_rnti += rnti_inc;
    return preamble;
  }

  rach_indication_message create_rach_indication(unsigned nof_preambles, du_cell_index_t cell_idx)
  {
    rach_indication_message rach_ind{};
    rach_ind.cell_index = cell_idx;
    rach_ind.slot_rx    = next_slot_rx() - 1;
    if (nof_preambles == 0) {
      return rach_ind;
    }
    rach_ind.occasions.emplace_back();
    rach_ind.occasions.back().start_symbol    = 0;
    rach_ind.occasions.back().frequency_index = 0;

    for (unsigned i = 0; i != nof_preambles; ++i) {
      rach_ind.occasions.back().preambles.emplace_back(create_preamble());
    }
    return rach_ind;
  }

  std::vector<cell_config_builder_params> cell_cfg_builder_params_list;
};

/// Formatter for test params.
void PrintTo(const multi_cell_scheduler_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "duplex_mode={} nof_cells={}", value.dplx_mode == duplex_mode::FDD ? "FDD" : "TDD", value.nof_cells);
}

class multi_cell_scheduler_tester : public base_multi_cell_scheduler_tester,
                                    public ::testing::TestWithParam<multi_cell_scheduler_test_params>
{
public:
  multi_cell_scheduler_tester() : base_multi_cell_scheduler_tester(GetParam()) {}
};

TEST_P(multi_cell_scheduler_tester, test_ssb_allocation_for_multiple_cells)
{
  const auto ssb_period_slots =
      ssb_periodicity_to_value(cell_cfg_list[to_du_cell_index(0)].ssb_cfg.ssb_period) *
      get_nof_slots_per_subframe(cell_cfg_builder_params_list[to_du_cell_index(0)].scs_common);

  std::vector<bool> is_ssb_scheduled_atleast_once(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < ssb_period_slots * 10; ++slot_count) {
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      if (last_sched_res_list[cell_idx] != nullptr and not is_ssb_scheduled_atleast_once[cell_idx]) {
        is_ssb_scheduled_atleast_once[cell_idx] = not last_sched_res_list[cell_idx]->dl.bc.ssb_info.empty();
      }
    }
    run_slot_all_cells();
  }
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    ASSERT_TRUE(is_ssb_scheduled_atleast_once[cell_idx])
        << fmt::format("SSB not scheduled for cell with index={}", cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_sib1_allocation_for_multiple_cells)
{
  const auto sib1_period_slots =
      std::max(ssb_periodicity_to_value(cell_cfg_list[to_du_cell_index(0)].ssb_cfg.ssb_period),
               sib1_rtx_periodicity_to_value(sched_cfg.si.sib1_retx_period)) *
      get_nof_slots_per_subframe(cell_cfg_builder_params_list[to_du_cell_index(0)].scs_common);

  std::vector<bool> is_sib1_scheduled_atleast_once(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < sib1_period_slots * 2; ++slot_count) {
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      if (last_sched_res_list[cell_idx] != nullptr and not is_sib1_scheduled_atleast_once[cell_idx]) {
        is_sib1_scheduled_atleast_once[cell_idx] = not last_sched_res_list[cell_idx]->dl.bc.sibs.empty();
      }
    }
    run_slot_all_cells();
  }
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    ASSERT_TRUE(is_sib1_scheduled_atleast_once[cell_idx])
        << fmt::format("SIB1 not scheduled for cell with index={}", cell_idx);
  }
}

TEST_P(multi_cell_scheduler_tester, test_rar_scheduling_for_ues_in_different_cells)
{
  // Number of slots to run the test.
  static const unsigned test_run_nof_slots                = 200;
  static const unsigned nof_preambles_per_rach_indication = 1;

  auto run_slot_until_next_rach_opportunity_condition = [this]() {
    return not cell_cfg_list[to_du_cell_index(0)].is_fully_ul_enabled(next_slot_rx() - 1);
  };
  run_slot_until(run_slot_until_next_rach_opportunity_condition);

  // Send one RACH indication per cell.
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    sched->handle_rach_indication(
        create_rach_indication(nof_preambles_per_rach_indication, to_du_cell_index(cell_idx)));
  }

  std::vector<bool> is_rar_scheduled(cell_cfg_builder_params_list.size(), false);
  for (unsigned slot_count = 0; slot_count < test_run_nof_slots; ++slot_count) {
    for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
      if (last_sched_res_list[cell_idx] != nullptr and not is_rar_scheduled[cell_idx]) {
        is_rar_scheduled[cell_idx] = std::any_of(last_sched_res_list[cell_idx]->dl.dl_pdcchs.begin(),
                                                 last_sched_res_list[cell_idx]->dl.dl_pdcchs.end(),
                                                 [](const pdcch_dl_information& dl_pdcch) {
                                                   return dl_pdcch.dci.type == srsran::dci_dl_rnti_config_type::ra_f1_0;
                                                 });
      }
    }
    run_slot_all_cells();
  }
  for (unsigned cell_idx = 0; cell_idx < cell_cfg_builder_params_list.size(); ++cell_idx) {
    ASSERT_TRUE(is_rar_scheduled[cell_idx]) << fmt::format("RAR not scheduled for cell with index={}", cell_idx);
  }
}

INSTANTIATE_TEST_SUITE_P(multi_cell_scheduler_test,
                         multi_cell_scheduler_tester,
                         testing::Values(multi_cell_scheduler_test_params{srsran::duplex_mode::FDD, 3},
                                         multi_cell_scheduler_test_params{srsran::duplex_mode::TDD, 2}));
