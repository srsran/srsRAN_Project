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

#include "lib/scheduler/common_scheduling/prach_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include <gtest/gtest.h>

using namespace srsran;

// We assume normal cyclic prefix here.
const unsigned NOF_SYM_PER_SLOT = 14;

namespace prach_test {

struct prach_test_params {
  subcarrier_spacing scs;
  nr_band            band;
  unsigned           prach_cfg_index;
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const prach_test_params& value, ::std::ostream* os)
{
  return;
}

static sched_cell_configuration_request_message
make_custom_sched_cell_configuration_request(const prach_test_params test_params)
{
  cell_config_builder_params params = {.scs_common     = test_params.scs,
                                       .channel_bw_mhz = srsran::bs_channel_bandwidth_fr1::MHz20,
                                       .band           = test_params.band};
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
} // namespace prach_test

using namespace prach_test;

class prach_tester : public ::testing::TestWithParam<prach_test_params>
{
protected:
  prach_tester() :
    cell_cfg(sched_cfg, make_custom_sched_cell_configuration_request(GetParam())),
    prach_sch(cell_cfg),
    sl(to_numerology_value(GetParam().scs), 0),
    prach_cfg(prach_configuration_get(
        frequency_range::FR1,
        cell_cfg.paired_spectrum ? duplex_mode::FDD : duplex_mode::TDD,
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)),
    res_grid(cell_cfg)
  {
    prach_symbols_slots_duration prach_duration_info =
        get_prach_duration_info(prach_cfg, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs);
    nof_symbols        = prach_duration_info.nof_symbols;
    prach_length_slots = prach_duration_info.prach_length_slots;
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

    if (is_long_preamble(prach_cfg.format)) {
      // With long Format PRACH, the starting_symbol refers to the SCS 15kHz. We need to map this starting symbol into
      // the slot of the SCS used by the system to know whether this is the slot with the PRACH opportunity.
      const unsigned start_slot_offset =
          prach_cfg.starting_symbol *
          (1U << to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs)) / NOF_SYM_PER_SLOT;
      if (sl.subframe_slot_index() != start_slot_offset) {
        return false;
      }
    } else {
      // With short Format PRACH, with 15kHz SCS, there is only 1 slot per subframe. This slot contrains a burst PRACH
      // occasions. With 30kHz SCS, there are 2 slots per subframe; depending on the whether "number of PRACH slots
      // within a subframe" 1 or 2 (as per Tables 6.3.3.2-2 and 6.3.3.2-3, TS 38.211), the occasions (and the
      // transmission in one of the occasions) are expected in the second slot of the subframe (if number of PRACH
      // slots within a subframe = 1) or in both slots (number of PRACH slots within a subframe = 2).
      if (to_ra_subcarrier_spacing(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs) ==
              prach_subcarrier_spacing::kHz30 and
          prach_cfg.nof_prach_slots_within_subframe == 1 and sl.subframe_slot_index() == 0U) {
        return false;
      }
    }

    return true;
  }

  unsigned nof_prach_occasions_allocated() const { return res_grid[0].result.ul.prachs.size(); }

  grant_info get_prach_grant(const prach_occasion_info& occasion, unsigned prach_slot_idx) const
  {
    // The information we need are not related to whether it is the last PRACH occasion.
    const bool                 is_last_prach_occasion = false;
    prach_preamble_information info =
        is_long_preamble(prach_cfg.format)
            ? get_prach_preamble_long_info(prach_cfg.format)
            : get_prach_preamble_short_info(
                  prach_cfg.format,
                  to_ra_subcarrier_spacing(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs),
                  is_last_prach_occasion);
    // Compute the grant PRBs
    const unsigned prach_nof_prbs =
        prach_frequency_mapping_get(info.scs, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs).nof_rb_ra;
    const uint8_t prb_start =
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.msg1_frequency_start +
        occasion.index_fd_ra * prach_nof_prbs;
    const prb_interval prach_prbs{prb_start, prb_start + prach_nof_prbs};
    const crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, prach_prbs);

    if (is_long_preamble(prach_cfg.format)) {
      // Compute the grant symbols.
      const unsigned starting_symbol_pusch_scs =
          (occasion.start_symbol * (1U << to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs))) %
          NOF_SYM_PER_SLOT;
      const ofdm_symbol_range prach_symbols{prach_slot_idx == 0 ? starting_symbol_pusch_scs : 0,
                                            prach_slot_idx < prach_length_slots - 1
                                                ? NOF_SYM_PER_SLOT
                                                : (starting_symbol_pusch_scs + nof_symbols) % NOF_SYM_PER_SLOT};

      return grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs};
    } else {
      const unsigned          starting_symbol_pusch_scs = occasion.start_symbol;
      const ofdm_symbol_range prach_symbols{starting_symbol_pusch_scs,
                                            starting_symbol_pusch_scs +
                                                prach_cfg.duration * prach_cfg.nof_occasions_within_slot};

      return grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, prach_symbols, crbs};
    }
  }

  static const unsigned nof_slots_run = 1000;

  const scheduler_expert_config sched_cfg{config_helpers::make_default_scheduler_expert_config()};
  cell_configuration            cell_cfg;
  prach_scheduler               prach_sch;
  slot_point                    sl;
  const prach_configuration     prach_cfg;
  cell_resource_allocator       res_grid;
  // Helper variables.
  unsigned prach_length_slots{1};
  unsigned nof_symbols{0};

  unsigned prach_counter = 0;
};

TEST_P(prach_tester, prach_sched_allocates_in_prach_configured_slots)
{
  for (unsigned i = 0; i != nof_slots_run; ++i) {
    slot_indication();
    if (is_prach_slot()) {
      ASSERT_GE(1, nof_prach_occasions_allocated());
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
      ASSERT_GE(1, nof_prach_occasions_allocated());
      for (const auto& prach_pdu : res_grid[0].result.ul.prachs) {
        // For long PRACHs, we have 1 PRACH PDU and up to 8 grid grants allocated per PRACH occasion; these are
        // allocated by the scheduler at the slot where the PRACH pramble starts.
        // For short PRACHs, we have 1 or 2 PRACH PDU and grid grant allocated per burst of PRACH occasions; the
        // occasion extents over 1 or 2 slots, each slot containing 1 PRACH PDU and 1 grid grant.
        for (unsigned prach_slot_idx = 0; prach_slot_idx < prach_length_slots; ++prach_slot_idx) {
          // Note that prach_slot_idx is only used for long PRACH. For short PRACHs, the function below return t  he
          // same grant regardless of the prach_slot_idx; this means the operation gets repeated twice.
          const grant_info grant = get_prach_grant(prach_pdu, prach_slot_idx);
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
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz15, .band = srsran::nr_band::n3, .prach_cfg_index = 86},
        // Format A1.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 87},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 88},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 99},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 100},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 106},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 107}),
    [](const testing::TestParamInfo<prach_tester::ParamType>& info_) {
      return fmt::format("fdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });
//
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
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 86},
        // Format A1.
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 87},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 88},
        prach_test_params{.scs = srsran::subcarrier_spacing::kHz30, .band = srsran::nr_band::n3, .prach_cfg_index = 99},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 100},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 106},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n3,
                          .prach_cfg_index = 107}),
    [](const testing::TestParamInfo<prach_tester::ParamType>& info_) {
      return fmt::format("fdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });

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
                          .prach_cfg_index = 64},
        // Format A1.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 67},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 69},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 71},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 73},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz15,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 86}),
    [](const testing::TestParamInfo<prach_tester::ParamType>& info_) {
      return fmt::format("tdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });

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
                          .prach_cfg_index = 65},
        // Format A1.
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 67},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 69},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 71},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 73},
        prach_test_params{.scs             = srsran::subcarrier_spacing::kHz30,
                          .band            = srsran::nr_band::n41,
                          .prach_cfg_index = 86}),
    [](const testing::TestParamInfo<prach_tester::ParamType>& info_) {
      return fmt::format("tdd_scs_{}_prach_cfg_idx_{}", to_string(info_.param.scs), info_.param.prach_cfg_index);
    });
