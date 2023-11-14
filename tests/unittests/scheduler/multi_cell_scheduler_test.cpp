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
    params.dl_arfcn         = test_params.dplx_mode == duplex_mode::FDD ? 365000 : 520002;
    params.band             = test_params.dplx_mode == duplex_mode::FDD ? nr_band::n3 : nr_band::n41;
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

TEST_P(multi_cell_scheduler_tester, test_slot_indication_for_multiple_cells)
{
  // Run for arbitrary nof. slots and ensure all cells are running.
  for (unsigned slot_idx; slot_idx < 100; ++slot_idx) {
    run_slot_all_cells();
  }
}

TEST_P(multi_cell_scheduler_tester, test_ssb_allocation_for_multiple_cells)
{
  std::vector<bool> is_ssb_scheduled_atleast_once(cell_cfg_builder_params_list.size(), false);
  // Run for arbitrary nof. slots and ensure all cells are running.
  for (unsigned slot_count = 0; slot_count < 100; ++slot_count) {
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

INSTANTIATE_TEST_SUITE_P(multi_cell_scheduler_test,
                         multi_cell_scheduler_tester,
                         testing::Values(multi_cell_scheduler_test_params{srsran::duplex_mode::FDD, 3},
                                         multi_cell_scheduler_test_params{srsran::duplex_mode::TDD, 2}));
