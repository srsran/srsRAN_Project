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

#include "test_utils/config_generators.h"
#include "test_utils/indication_generators.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

scheduler_expert_config create_custom_sched_cfg(unsigned max_ul_grants = 32)
{
  scheduler_expert_config cfg   = config_helpers::make_default_scheduler_expert_config();
  cfg.ue.max_ul_grants_per_slot = max_ul_grants;
  return cfg;
}

class base_sched_grants_per_slot_test : public scheduler_test_simulator
{
protected:
  base_sched_grants_per_slot_test(unsigned max_ul_grants_per_slot = 32) :
    scheduler_test_simulator(scheduler_test_sim_config{create_custom_sched_cfg(max_ul_grants_per_slot),
                                                       4,
                                                       subcarrier_spacing::kHz30,
                                                       true,
                                                       true}),
    params(cell_config_builder_profiles::tdd())
  {
    auto sched_cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    add_cell(sched_cell_cfg_req);

    // Create PUCCH builder that will be used to add UEs.
    pucch_builder_params pucch_basic_params{.nof_ue_pucch_f0_or_f1_res_harq       = 8,
                                            .nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8,
                                            .nof_sr_resources                     = 8,
                                            .nof_csi_resources                    = 8};
    auto&                f1_params = pucch_basic_params.f0_or_f1_params.emplace<pucch_f1_params>();
    f1_params.nof_cyc_shifts       = pucch_nof_cyclic_shifts::twelve;
    f1_params.occ_supported        = true;
    pucch_cfg_builder.setup(cell_cfg(to_du_cell_index(0)), pucch_basic_params);
  }

  void add_ue()
  {
    // Add UE to scheduler.
    du_ue_index_t next_ue_idx = to_du_ue_index(nof_ues++);
    auto          ue_cfg      = sched_config_helper::create_default_sched_ue_creation_request(params, {LCID_MIN_DRB});
    ue_cfg.ue_index           = next_ue_idx;
    ue_cfg.crnti              = to_rnti(0x4601 + (unsigned)next_ue_idx);
    report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                              "Failed to allocate PUCCH resources");
    ue_cfg.starts_in_fallback = false;
    scheduler_test_simulator::add_ue(ue_cfg);

    // Enqueue enough bytes for continuous DL tx.
    const unsigned                     DL_BS_VALUE = std::numeric_limits<unsigned>::max() / 2;
    dl_buffer_state_indication_message dl_buf_st{next_ue_idx, LCID_MIN_DRB, DL_BS_VALUE, this->next_slot_rx()};
    this->push_dl_buffer_state(dl_buf_st);

    // Enqueue BSR.
    const unsigned            BSR_VALUE = std::numeric_limits<uint32_t>::max() / 2;
    ul_bsr_indication_message bsr{to_du_cell_index(0),
                                  next_ue_idx,
                                  ue_cfg.crnti,
                                  bsr_format::LONG_BSR,
                                  ul_bsr_lcg_report_list{{uint_to_lcg_id(2), BSR_VALUE}}};
    this->push_bsr(bsr);
  }

  cell_config_builder_params    params;
  pucch_res_builder_test_helper pucch_cfg_builder;
  unsigned                      nof_ues = 0;
};

} // namespace

class sched_limited_grants_per_slot_test : public base_sched_grants_per_slot_test, public ::testing::Test
{
public:
  sched_limited_grants_per_slot_test() : base_sched_grants_per_slot_test(8) {}
};

TEST_F(sched_limited_grants_per_slot_test, test_max_ul_grants_per_slot_lower_than_limit)
{
  for (unsigned i = 0; i != 32; ++i) {
    add_ue();
  }

  unsigned nof_test_slots = 100;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    run_slot();

    unsigned ul_grants = this->last_sched_result(to_du_cell_index(0))->ul.pucchs.size() +
                         this->last_sched_result(to_du_cell_index(0))->ul.puschs.size();
    EXPECT_LE(ul_grants, sched_cfg.ue.max_ul_grants_per_slot) << "UL grants exceeded the limit of grants per slot";
  }
}
