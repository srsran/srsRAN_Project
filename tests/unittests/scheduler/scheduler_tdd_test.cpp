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

/// \file
/// \brief Unit test for scheduler using different TDD patterns.

#include "test_utils/indication_generators.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/ran/tdd/tdd_ul_dl_config_formatters.h"
#include <gtest/gtest.h>
#include <ostream>

using namespace srsran;

struct tdd_test_params {
  bool                    csi_rs_enabled;
  tdd_ul_dl_config_common tdd_cfg;
  unsigned                min_k = 4;
};

class base_scheduler_tdd_tester : public scheduler_test_simulator
{
protected:
  base_scheduler_tdd_tester(const tdd_test_params& testparams) :
    scheduler_test_simulator(
        scheduler_test_sim_config{.max_scs = testparams.tdd_cfg.ref_scs, .auto_uci = true, .auto_crc = true})
  {
    params                      = cell_config_builder_profiles::tdd(testparams.tdd_cfg.ref_scs);
    params.csi_rs_enabled       = testparams.csi_rs_enabled;
    params.tdd_ul_dl_cfg_common = testparams.tdd_cfg;
    params.min_k1               = testparams.min_k;
    params.min_k2               = testparams.min_k;

    // Add Cell.
    this->add_cell(sched_config_helper::make_default_sched_cell_configuration_request(params));

    // Add UE
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(params, {ue_drb_lcid});
    ue_cfg.ue_index = ue_idx;
    ue_cfg.crnti    = ue_rnti;
    // Increase PUCCH Format 2 code rate to support TDD configuration of DDDDDDDDSU.
    srsran_assert((*ue_cfg.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.has_value(),
                  "The PUCCH config is not set");
    auto& pucch_cfg = (*ue_cfg.cfg.cells)[0].serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value();
    pucch_cfg.format_2_common_param->max_c_rate = max_pucch_code_rate::dot_35;
    pucch_res_id_t any_res_f2_id                = pucch_cfg.pucch_res_set[1].pucch_res_id_list.front();
    auto*          res_f2                       = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                pucch_cfg.pucch_res_list.end(),
                                [any_res_f2_id](const auto& res) { return res.res_id == any_res_f2_id; });
    srsran_assert(res_f2 != pucch_cfg.pucch_res_list.end(), "PUCCH resource F2 not found");
    pucch_cfg.format_max_payload[pucch_format_to_uint(pucch_format::FORMAT_2)] =
        get_pucch_format2_max_payload(std::get<pucch_format_2_3_cfg>(res_f2->format_params).nof_prbs,
                                      res_f2->nof_symbols,
                                      to_max_code_rate_float(pucch_cfg.format_2_common_param.value().max_c_rate));

    this->add_ue(ue_cfg);
  }

  const du_ue_index_t ue_idx      = to_du_ue_index(0);
  const rnti_t        ue_rnti     = to_rnti(0x4601);
  const lcid_t        ue_drb_lcid = LCID_MIN_DRB;

  cell_config_builder_params params;
};

/// Formatter for test params.
void PrintTo(const tdd_test_params& value, ::std::ostream* os)
{
  *os << fmt::format(
      "csi={} tdd={} min_k={}", value.csi_rs_enabled ? "enabled" : "disabled", value.tdd_cfg, value.min_k);
}

class scheduler_dl_tdd_tester : public base_scheduler_tdd_tester, public ::testing::TestWithParam<tdd_test_params>
{
public:
  scheduler_dl_tdd_tester() : base_scheduler_tdd_tester(GetParam()) {}
};

TEST_P(scheduler_dl_tdd_tester, all_dl_slots_are_scheduled)
{
  // Enqueue enough bytes for continuous DL tx.
  dl_buffer_state_indication_message dl_buf_st{ue_idx, ue_drb_lcid, 10000000};
  this->push_dl_buffer_state(dl_buf_st);

  static constexpr unsigned MAX_COUNT = 1000;
  for (unsigned count = 0; count != MAX_COUNT; ++count) {
    this->run_slot();

    // For every DL slot.
    if (cell_cfg(to_du_cell_index(0)).is_dl_enabled(this->last_result_slot())) {
      // Ensure UE PDSCH allocations are made.
      ASSERT_FALSE(this->last_sched_result(to_du_cell_index(0))->dl.ue_grants.empty()) << fmt::format(
          "The UE configuration is leading to slot {} not having DL UE grant scheduled", this->last_result_slot());
    }
  }
}

class scheduler_ul_tdd_tester : public base_scheduler_tdd_tester, public ::testing::TestWithParam<tdd_test_params>
{
public:
  scheduler_ul_tdd_tester() : base_scheduler_tdd_tester(GetParam())
  {
    // Enqueue enough bytes for continuous UL tx.
    ul_bsr_indication_message bsr{
        to_du_cell_index(0), ue_idx, ue_rnti, bsr_format::SHORT_BSR, {ul_bsr_lcg_report{uint_to_lcg_id(0), 10000000}}};
    this->push_bsr(bsr);

    // Run some slots to ensure that there is space for PDCCH to be scheduled.
    unsigned tdd_period = nof_slots_per_tdd_period(*cell_cfg(to_du_cell_index(0)).tdd_cfg_common);
    for (unsigned i = 0; i != 2 * tdd_period; ++i) {
      run_slot();
    }
  }
};

TEST_P(scheduler_ul_tdd_tester, all_ul_slots_are_scheduled)
{
  static constexpr unsigned MAX_COUNT = 1000;
  for (unsigned count = 0; count != MAX_COUNT; ++count) {
    this->run_slot();

    // For every UL slot.
    // Note: Skip special slots in test for now.
    if (cell_cfg().is_fully_ul_enabled(this->last_result_slot())) {
      // Ensure UE PUSCH allocations are made.
      ASSERT_FALSE(this->last_sched_result()->ul.puschs.empty()) << fmt::format(
          "The UE configuration is leading to slot {} not having UL UE grant scheduled", this->last_result_slot());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_dl_tdd_tester,
    testing::Values(
        // clang-format off
// csi_enabled, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}, min_k}
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}, 1},
  tdd_test_params{false, {subcarrier_spacing::kHz30, {10, 8, 0, 1, 0}}},
  tdd_test_params{false, {subcarrier_spacing::kHz30, {10, 8, 0, 1, 0}}, 1},
  tdd_test_params{false, {subcarrier_spacing::kHz30, {6,  3, 5, 2, 0}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}},
  // DDDSUUDDDD, TS 38.101-4, Table A.1.2-2, Configuration FR1.30-4.
  tdd_test_params{false, {subcarrier_spacing::kHz30, {6,  3, 6, 2, 4}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {5,  3, 9, 1, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {5,  3, 9, 1, 0}}, 1},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}, 1},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 13, 3, 0}}, 4} // DDDDDDSUUU, with 13 DL symbols in special slot
  // TODO: Support more TDD patterns.
// Note: The params below lead to a failure due to "Not enough space in PUCCH". However, I don't think there is no valid
// k1 candidate list that accommodates all DL slots.
  //tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}
));
// clang-format on

INSTANTIATE_TEST_SUITE_P(
    scheduler_tdd_test,
    scheduler_ul_tdd_tester,
    testing::Values(
        // clang-format off
// csi_enabled, {ref_scs, pattern1={slot_period, DL_slots, DL_symbols, UL_slots, UL_symbols}, pattern2={...}}
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 5, 3, 4}}}, // DDDDDDSUUU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 7, 5, 2, 4}}}, // DDDDDDDSUU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}}}, // DDDDDDDDSU
  tdd_test_params{false, {subcarrier_spacing::kHz30, {6,  3, 5, 2, 0}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}},
  // DDDSUUDDDD, TS 38.101-4, Table A.1.2-2, Configuration FR1.30-4.
  tdd_test_params{false, {subcarrier_spacing::kHz30, {6,  3, 6, 2, 4}, tdd_ul_dl_pattern{4, 4, 0, 0, 0}}},
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {4,  2, 9, 1, 0}}},  // DDSU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 4, 5, 5, 0}}, 5}, // DDDDSUUUUU
  tdd_test_params{true,  {subcarrier_spacing::kHz30, {10, 6, 13, 3, 0}}, 4}, // DDDDDDSUUU, with 13 DL symbols in special slot
  // UL heavy
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 3,  5, 6, 0}}},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}, tdd_ul_dl_pattern{5, 1, 10, 3, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {6,  2, 10, 3, 0}, tdd_ul_dl_pattern{4, 1, 0, 3, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {4,  1, 10, 2, 0}, tdd_ul_dl_pattern{6, 1, 10, 4, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2, 10, 7, 0}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2,  6, 7, 4}}, 2},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {10, 2,  6, 7, 4}}},
  tdd_test_params{true, {subcarrier_spacing::kHz30, {5,  1, 10, 3, 0}}, 2}
));
// clang-format on
