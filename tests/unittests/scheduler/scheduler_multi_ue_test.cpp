/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include <gtest/gtest.h>

using namespace srsran;

struct multi_ue_test_params {
  unsigned nof_ues           = 1;
  unsigned auto_dl_bs        = std::numeric_limits<uint32_t>::max() / 2;
  unsigned auto_ul_bs        = std::numeric_limits<uint32_t>::max() / 2;
  unsigned max_pdsch_latency = 20;
  unsigned max_pusch_latency = 20;
};

class scheduler_multi_ue_test : public scheduler_test_simulator
{
public:
  scheduler_multi_ue_test(const multi_ue_test_params& tparams) :
    scheduler_test_simulator(
        scheduler_test_sim_config{.max_scs = subcarrier_spacing::kHz30, .auto_uci = true, .auto_crc = true}),
    test_params(tparams),
    params(cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30))
  {
    // Add Cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    this->add_cell(cell_cfg_req);

    // Create PUCCH builder that will be used to add UEs.
    pucch_builder_params pucch_basic_params{.nof_ue_pucch_f0_or_f1_res_harq       = 8,
                                            .nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8,
                                            .nof_sr_resources                     = 32,
                                            .nof_csi_resources                    = 32};
    auto&                f1_params = pucch_basic_params.f0_or_f1_params.emplace<pucch_f1_params>();
    f1_params.nof_cyc_shifts       = pucch_nof_cyclic_shifts::twelve;
    f1_params.occ_supported        = true;
    pucch_cfg_builder.setup(cell_cfg_list[0], pucch_basic_params);

    // Add UEs.
    for (unsigned i = 0, sz = test_params.nof_ues; i != sz; ++i) {
      add_ue(to_rnti(0x4601 + i));
    }
  }

  void add_ue(rnti_t rnti)
  {
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(params, {LCID_MIN_DRB});
    ue_cfg.ue_index = to_du_ue_index((unsigned)rnti - 0x4601);
    ue_cfg.crnti    = rnti;
    report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                              "Failed to allocate PUCCH resources");
    scheduler_test_simulator::add_ue(ue_cfg, true);

    // Enqueue BSR.
    const unsigned            BSR_VALUE = std::numeric_limits<uint32_t>::max() / 2;
    ul_bsr_indication_message bsr{to_du_cell_index(0),
                                  ue_cfg.ue_index,
                                  ue_cfg.crnti,
                                  bsr_format::LONG_BSR,
                                  ul_bsr_lcg_report_list{{uint_to_lcg_id(2), BSR_VALUE}}};
    this->push_bsr(bsr);
  }

  void run_slot()
  {
    if (test_params.auto_dl_bs > 0) {
      for (const auto& u : this->rnti_to_ue_index_mapping()) {
        // Enqueue enough bytes for continuous DL tx.
        const unsigned dl_bs_val = std::min(test_params.auto_dl_bs, std::numeric_limits<unsigned>::max() / 2);
        dl_buffer_state_indication_message dl_buf_st{u.second, LCID_MIN_DRB, dl_bs_val, this->next_slot_rx()};
        this->push_dl_buffer_state(dl_buf_st);
      }
    }

    if (test_params.auto_ul_bs > 0) {
      for (const auto& u : this->rnti_to_ue_index_mapping()) {
        const unsigned            ul_bsr = std::min(test_params.auto_ul_bs, std::numeric_limits<uint32_t>::max() / 2);
        ul_bsr_indication_message bsr{to_du_cell_index(0),
                                      u.second,
                                      u.first,
                                      bsr_format::LONG_BSR,
                                      ul_bsr_lcg_report_list{{uint_to_lcg_id(2), ul_bsr}}};
        this->push_bsr(bsr);
      }
    }

    scheduler_test_simulator::run_slot();
  }

  multi_ue_test_params          test_params;
  cell_config_builder_params    params;
  pucch_res_builder_test_helper pucch_cfg_builder;
};

class scheduler_multi_ue_latency_test : public ::testing::TestWithParam<multi_ue_test_params>,
                                        public scheduler_multi_ue_test
{
public:
  scheduler_multi_ue_latency_test() : scheduler_multi_ue_test(GetParam()) {}
};

TEST_P(scheduler_multi_ue_latency_test, pxsch_latency_within_provided_limits) {}

INSTANTIATE_TEST_SUITE_P(scheduler_multi_ue_latency_test,
                         scheduler_multi_ue_latency_test,
                         ::testing::Values(multi_ue_test_params{300}));
