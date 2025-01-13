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

#include "lib/scheduler/support/pusch/pusch_default_time_allocation.h"
#include "lib/scheduler/support/pusch/pusch_td_resource_indices.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/ran/tdd/tdd_ul_dl_config_formatters.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"
#include "srsran/support/format/custom_formattable.h"
#include "fmt/std.h"
#include <gtest/gtest.h>
#include <ostream>

using namespace srsran;

namespace {

struct test_params {
  unsigned min_k = 4;
  /// If present duplex mode is TDD, FDD otherwise.
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
};

class pusch_td_resource_indices_test : public ::testing::TestWithParam<test_params>
{
protected:
  pusch_td_resource_indices_test()
  {
    test_params testparams = GetParam();
    if (testparams.tdd_cfg.has_value()) {
      params.scs_common     = testparams.tdd_cfg->ref_scs;
      params.dl_f_ref_arfcn = 520002;
      params.channel_bw_mhz = bs_channel_bandwidth::MHz20;
    }
    params.band             = band_helper::get_band_from_dl_arfcn(params.dl_f_ref_arfcn);
    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        params.channel_bw_mhz, params.scs_common, band_helper::get_freq_range(*params.band));
    static const uint8_t                                   ss0_idx = 0;
    std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
        band_helper::get_ssb_coreset0_freq_location(params.dl_f_ref_arfcn,
                                                    *params.band,
                                                    nof_crbs,
                                                    params.scs_common,
                                                    params.scs_common,
                                                    ss0_idx,
                                                    params.max_coreset0_duration);
    if (!ssb_freq_loc.has_value()) {
      report_error("Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", params.pci);
    }
    params.offset_to_point_a    = (*ssb_freq_loc).offset_to_point_A;
    params.k_ssb                = (*ssb_freq_loc).k_ssb;
    params.coreset0_index       = (*ssb_freq_loc).coreset0_idx;
    params.search_space0_index  = ss0_idx;
    params.tdd_ul_dl_cfg_common = testparams.tdd_cfg;
    params.min_k1               = testparams.min_k;
    params.min_k2               = testparams.min_k;

    sched_cell_configuration_request_message sched_cell_cfg_req =
        sched_config_helper::make_default_sched_cell_configuration_request(params);

    // Generate cell configuration.
    cell_cfg = std::make_unique<cell_configuration>(expert_cfg, sched_cell_cfg_req);

    // Generate the list to verify.
    pusch_td_res_indxes_list_per_slot = get_fairly_distributed_pusch_td_resource_indices(*cell_cfg);

    // Populate slot indexes.
    if (cell_cfg->is_tdd()) {
      for (unsigned slot_idx = 0, e = nof_slots_per_tdd_period(*cell_cfg->tdd_cfg_common); slot_idx != e; ++slot_idx) {
        if (has_active_tdd_dl_symbols(*cell_cfg->tdd_cfg_common, slot_idx)) {
          dl_slot_indexes.push_back(slot_idx);
        }
        if (is_tdd_full_ul_slot(*cell_cfg->tdd_cfg_common, slot_idx)) {
          ul_slot_indexes.push_back(slot_idx);
        }
      }
    } else {
      // NOTE: [Implementation-defined] In case of FDD, we consider only one slot as all slots are similar unlike in TDD
      // where there can be DL/UL full or partial slots.
      dl_slot_indexes.push_back(0);
      ul_slot_indexes.push_back(params.min_k2);
    }
  }

  scheduler_expert_config    expert_cfg;
  cell_config_builder_params params{};

  std::unique_ptr<cell_configuration> cell_cfg;
  // Contains slot indexes of the DL enabled slots.
  std::vector<unsigned> dl_slot_indexes;
  // Contains slot indexes of fully UL enabled slots.
  std::vector<unsigned> ul_slot_indexes;

  // List circularly indexed by slot with the list of applicable PUSCH Time Domain resource indexes per slot.
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> pusch_td_res_indxes_list_per_slot;
};

TEST_P(pusch_td_resource_indices_test, all_ul_slots_have_one_pdcch_slot_to_schedule_pusch)
{
  // Fetch the relevant PUSCH time domain resource list.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      get_c_rnti_pusch_time_domain_list(true, to_coreset_id(0), cell_cfg->ul_cfg_common.init_ul_bwp, nullptr);

  unsigned slot_mod = cell_cfg->tdd_cfg_common.has_value() ? nof_slots_per_tdd_period(cell_cfg->tdd_cfg_common.value())
                                                           : SCHEDULER_MAX_K2;

  auto ul_slot_idx_it = ul_slot_indexes.begin();
  while (ul_slot_idx_it != ul_slot_indexes.end()) {
    bool pdcch_slot_found = false;
    for (const auto dl_slot_idx : dl_slot_indexes) {
      span<const unsigned> pusch_td_res_indxes_list = pusch_td_res_indxes_list_per_slot[dl_slot_idx];
      pdcch_slot_found =
          std::any_of(pusch_td_res_indxes_list.begin(),
                      pusch_td_res_indxes_list.end(),
                      [&pusch_time_domain_list, dl_slot_idx, ul_slot_idx = *ul_slot_idx_it, slot_mod](
                          const unsigned pusch_td_res_idx) {
                        return ul_slot_idx == (dl_slot_idx + pusch_time_domain_list[pusch_td_res_idx].k2) % slot_mod;
                      });
      if (pdcch_slot_found) {
        break;
      }
    }
    ASSERT_TRUE(pdcch_slot_found) << fmt::format("No PDCCH slot found for UL slot={}", *ul_slot_idx_it);
    ul_slot_idx_it = ul_slot_indexes.erase(ul_slot_idx_it);
  }
}

TEST_P(pusch_td_resource_indices_test, pusch_td_resources_are_fairly_distributed_across_pdcch_slots)
{
  // For FDD and DL heavy TDD pattern, only one UL PDCCH is allowed per PDCCH slot.
  auto est_max_nof_ul_pdcchs_per_dl_slot = 1;
  if (cell_cfg->is_tdd() and ul_slot_indexes.size() > dl_slot_indexes.size()) {
    // Estimate the nof. UL PDCCHs that can be scheduled in each PDCCH slot in UL heavy TDD pattern.
    est_max_nof_ul_pdcchs_per_dl_slot = static_cast<unsigned>(
        std::round(static_cast<double>(ul_slot_indexes.size()) / static_cast<double>(dl_slot_indexes.size())));
    // Adjustment for corner case such as DDSUUUUUUU, where there are uneven nof. DL and UL slots resulting in one DL
    // slot having to schedule more than previously estimated nof. nof. UL PDCCHs per PDCCH slot.
    if ((est_max_nof_ul_pdcchs_per_dl_slot * dl_slot_indexes.size()) < ul_slot_indexes.size()) {
      est_max_nof_ul_pdcchs_per_dl_slot +=
          (ul_slot_indexes.size() - (est_max_nof_ul_pdcchs_per_dl_slot * dl_slot_indexes.size()));
    }
  };

  for (const auto dl_slot_idx : dl_slot_indexes) {
    ASSERT_LE(pusch_td_res_indxes_list_per_slot[dl_slot_idx].size(), est_max_nof_ul_pdcchs_per_dl_slot)
        << fmt::format("Nof. PUSCH TD resource indices for PDCCH slot={} exceeds max. value={}",
                       dl_slot_idx,
                       est_max_nof_ul_pdcchs_per_dl_slot);
  }
}

/// Formatter for test params.
void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("min_k={} {}{}",
                     value.min_k,
                     value.tdd_cfg.has_value() ? "tdd" : "fdd",
                     add_prefix_if_set(" pattern=", value.tdd_cfg));
}

} // namespace

INSTANTIATE_TEST_SUITE_P(
    pusch_td_resource_indices_test,
    pusch_td_resource_indices_test,
    testing::Values(
        // clang-format off
        // min_k, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}}
        test_params{2,  {}}, // FDD
        test_params{4,  {}}, // FDD
        test_params{4,  tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
        test_params{2,  tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
        test_params{2,  tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}, // DDDDDDDDSU
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {6,  3, 5, 2, 0}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}},
        test_params{2,  tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},  // DDSU
        test_params{5, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}}, // DDDDSUUUUU
        // UL heavy
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 3, 5, 6, 0}}},
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5, 1, 10, 3, 0}, tdd_ul_dl_pattern{5, 1, 10, 3, 0}}},
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {6, 2, 10, 3, 0}, tdd_ul_dl_pattern{4, 1, 0, 3, 0}}},
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {4, 1, 10, 2, 0}, tdd_ul_dl_pattern{6, 1, 10, 4, 0}}},
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 2, 10, 7, 0}}},
        test_params{2, tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}}} // clang-format on
        ));
