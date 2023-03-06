/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/common_scheduling/prach_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include <gtest/gtest.h>

using namespace srsran;

// We assume normal cyclic prefix here.
const unsigned NOF_SYM_PER_SLOT = 14;

struct prach_test_params {
  subcarrier_spacing scs;
  nr_band            band;
  unsigned           prach_cfg_index;
};

static sched_cell_configuration_request_message
make_custom_sched_cell_configuration_request(const prach_test_params test_params)
{
  cell_config_builder_params params = {.scs_common = test_params.scs, .band = test_params.band};
  // For TDD, set DL ARFCN according to the band.
  if (not band_helper::is_paired_spectrum(test_params.band)) {
    params.dl_arfcn = 520002;
  }
  sched_cell_configuration_request_message sched_req =
      test_helpers::make_default_sched_cell_configuration_request(params);

  // Change Carrier parameters when SCS is 15kHz.
  if (test_params.scs == subcarrier_spacing::kHz15) {
    sched_req.dl_cfg_common.freq_info_dl.scs_carrier_list.front().carrier_bandwidth = 106;
    sched_req.dl_cfg_common.init_dl_bwp.generic_params.crbs =
        crb_interval{0, sched_req.dl_cfg_common.freq_info_dl.scs_carrier_list.front().carrier_bandwidth};
  }
  // Change Carrier parameters when SCS is 30kHz.
  else if (test_params.scs == subcarrier_spacing::kHz30) {
    sched_req.dl_cfg_common.freq_info_dl.scs_carrier_list.emplace_back(
        scs_specific_carrier{0, subcarrier_spacing::kHz30, 51});
    sched_req.dl_cfg_common.init_dl_bwp.generic_params.crbs = {
        0, sched_req.dl_cfg_common.freq_info_dl.scs_carrier_list[1].carrier_bandwidth};
  }
  sched_req.dl_carrier.carrier_bw_mhz = 20;

  sched_req.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index =
      test_params.prach_cfg_index;

  // NOTE: For this test we modify the TDD pattern so that we can test several PRACH configuration indices.
  if (not band_helper::is_paired_spectrum(test_params.band)) {
    sched_req.tdd_ul_dl_cfg_common.value().pattern1.nof_dl_slots = 2;
    sched_req.tdd_ul_dl_cfg_common.value().pattern1.nof_ul_slots = 8;
  }

  return sched_req;
}

class prach_tester : public ::testing::TestWithParam<prach_test_params>
{
protected:
  prach_tester() :
    cell_cfg(make_custom_sched_cell_configuration_request(GetParam())),
    prach_sch(cell_cfg),
    sl(to_numerology_value(GetParam().scs), 0),
    prach_cfg(prach_configuration_get(
        frequency_range::FR1,
        cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)),
    res_grid(cell_cfg)
  {
    prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);
    static const double        symbol_duration_msec =
        (double)SUBFRAME_DURATION_MSEC /
        (double)(get_nof_slots_per_subframe(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs) * NOF_SYM_PER_SLOT);
    const double length_msecs = (info.cp_length.to_seconds() + info.symbol_length.to_seconds()) * 1000;
    nof_symbols               = ceil(length_msecs / symbol_duration_msec);
    // Map the starting symbol with from the SCS 15kHz FR1 reference for PRACH into PUSCH SCS.
    unsigned starting_symbol_pusch_scs =
        prach_cfg.starting_symbol * (1U << to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs));
    prach_length_slots = static_cast<unsigned>(
        ceil(static_cast<double>(nof_symbols + starting_symbol_pusch_scs) / (static_cast<double>(NOF_SYM_PER_SLOT))));
  }

  void slot_indication()
  {
    ++sl;
    res_grid.slot_indication(sl);
    prach_sch.run_slot(res_grid);
    prach_counter += res_grid[0].result.ul.prachs.size();
  }

  bool is_prach_slot()
  {
    if (sl.sfn() % prach_cfg.x != prach_cfg.y) {
      return false;
    }
    if (std::find(prach_cfg.subframe.begin(), prach_cfg.subframe.end(), sl.subframe_index()) ==
        prach_cfg.subframe.end()) {
      return false;
    }
    // Start slot within the subframe. For FR1, the starting_symbol refers to the SCS 15kHz.
    unsigned start_slot_offset = prach_cfg.starting_symbol *
                                 (1U << to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs)) /
                                 NOF_SYM_PER_SLOT;
    if (sl.subframe_slot_index() != start_slot_offset) {
      return false;
    }
    return true;
  }

  unsigned nof_prach_occasions_allocated() const { return res_grid[0].result.ul.prachs.size(); }

  grant_info get_prach_grant(const prach_occasion_info& occasion, unsigned prach_slot_idx) const
  {
    // Compute the grant PRBs
    prach_preamble_information info = get_prach_preamble_long_info(prach_cfg.format);
    const unsigned             prach_nof_prbs =
        prach_frequency_mapping_get(info.scs, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
    const uint8_t prb_start =
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start +
        occasion.index_fd_ra * prach_nof_prbs;
    const prb_interval prach_prbs{prb_start, prb_start + prach_nof_prbs};
    const crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

    // Compute the grant symbols.
    const ofdm_symbol_range prach_symbols{prach_slot_idx == 0 ? occasion.start_symbol : 0,
                                          prach_slot_idx < prach_length_slots - 1
                                              ? NOF_SYM_PER_SLOT
                                              : (occasion.start_symbol + nof_symbols) % NOF_SYM_PER_SLOT};

    return grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs};
  }

  static const unsigned nof_slots_run = 1000;

  cell_configuration      cell_cfg;
  prach_scheduler         prach_sch;
  slot_point              sl;
  prach_configuration     prach_cfg;
  cell_resource_allocator res_grid;
  // Helper variables.
  unsigned prach_length_slots;
  unsigned nof_symbols;

  unsigned prach_counter = 0;
};

TEST_P(prach_tester, prach_sched_allocates_in_prach_configured_slots)
{
  for (unsigned i = 0; i != nof_slots_run; ++i) {
    slot_indication();
    if (is_prach_slot()) {
      ASSERT_GE(nof_prach_occasions_allocated(), 1);
    } else {
      ASSERT_EQ(0, nof_prach_occasions_allocated());
    }
  }
  ASSERT_GT(prach_counter, 0);
}

TEST_P(prach_tester, prach_sched_allocates_in_sched_grid)
{
  for (unsigned i = 0; i != nof_slots_run; ++i) {
    slot_indication();
    if (is_prach_slot()) {
      for (const auto& prach_pdu : res_grid[0].result.ul.prachs) {
        for (unsigned prach_slot_idx = 0; prach_slot_idx < prach_length_slots; ++prach_slot_idx) {
          grant_info grant = get_prach_grant(prach_pdu, prach_slot_idx);
          test_res_grid_has_re_set(res_grid, grant, 0);
        }
      }
    }
  }
}

TEST_P(prach_tester, prach_sched_results_matches_config)
{
  for (unsigned i = 0; i != nof_slots_run; ++i) {
    slot_indication();
    test_scheduler_result_consistency(cell_cfg, res_grid[0].slot, res_grid[0].result);
  }
}

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_fdd_15kHz,
    prach_tester,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 0},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 15},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 26},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 27},
        // Format 1.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 34},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 44},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 51},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 52},
        // Format 2.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 55},
        // Format 3.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 73},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 86}));

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_fdd_30kHz,
    prach_tester,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 0},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 15},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 26},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 27},
        // Format 1.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 34},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 44},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 51},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 52},
        // Format 2.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 55},
        // Format 3.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 73},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 86}));

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_tdd_15kHz,
    prach_tester,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n41, .prach_cfg_index = 0},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n41, .prach_cfg_index = 9},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 12},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 14},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 17},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 25},
        // Format 1.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 29},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 33},
        // Format 2.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 34},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 38},
        // Format 3.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 40},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 57},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 64}));

INSTANTIATE_TEST_SUITE_P(
    prach_scheduler_tdd_30kHz,
    prach_tester,
    testing::Values(
        // Format 0.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n41, .prach_cfg_index = 0},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n41, .prach_cfg_index = 9},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 12},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 14},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 16},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 17},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 26},
        // Format 1.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 29},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 33},
        // Format 2.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 34},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 38},
        // Format 3.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 40},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 55},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 56},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 65}));
