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
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

struct multi_ue_test_params {
  unsigned nof_ues    = 1;
  unsigned auto_dl_bs = std::numeric_limits<uint32_t>::max() / 2;
  unsigned auto_ul_bs = std::numeric_limits<uint32_t>::max() / 2;
};

void PrintTo(const multi_ue_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("nof_ues={}, auto_dl_bs={}, auto_ul_bs={}", value.nof_ues, value.auto_dl_bs, value.auto_ul_bs);
}

class scheduler_multi_ue_test : public scheduler_test_simulator
{
public:
  scheduler_multi_ue_test(const multi_ue_test_params& tparams) :
    scheduler_test_simulator(scheduler_test_sim_config{.sched_cfg =
                                                           []() {
                                                             scheduler_expert_config cfg =
                                                                 config_helpers::make_default_scheduler_expert_config();
                                                             cfg.ue.max_pucchs_per_slot    = 63;
                                                             cfg.ue.max_ul_grants_per_slot = 64;
                                                             return cfg;
                                                           }(),
                                                       .max_scs  = subcarrier_spacing::kHz30,
                                                       .auto_uci = true,
                                                       .auto_crc = true}),
    test_params(tparams),
    params(cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30))
  {
    // Add Cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    this->add_cell(cell_cfg_req);

    // Create PUCCH builder that will be used to add UEs.
    pucch_builder_params pucch_basic_params{.nof_ue_pucch_f0_or_f1_res_harq       = 8,
                                            .nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8,
                                            .nof_cell_harq_pucch_res_sets         = 2,
                                            .nof_sr_resources                     = 80,
                                            .nof_csi_resources                    = 80};
    auto&                f1_params = pucch_basic_params.f0_or_f1_params.emplace<pucch_f1_params>();
    f1_params.nof_cyc_shifts       = pucch_nof_cyclic_shifts::twelve;
    f1_params.occ_supported        = true;
    pucch_cfg_builder.setup(cell_cfg(), pucch_basic_params);

    // Add UEs.
    for (unsigned i = 0, sz = test_params.nof_ues; i != sz; ++i) {
      add_ue(to_rnti(0x4601 + i));
    }

    srslog::flush();
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
    // Push BSRs and DL buffer occupancy updates for a subset of UEs.
    const unsigned bsr_step = 16;
    auto&          rnti_map = this->rnti_to_ue_index_mapping();
    // Round-robin groups of UEs.
    auto start_it = rnti_map.begin();
    bsr_offset    = bsr_offset % rnti_map.size();
    std::advance(start_it, bsr_offset);
    bsr_offset += bsr_step;

    if (test_params.auto_dl_bs > 0) {
      unsigned count = 0;
      for (auto it = start_it; it != rnti_map.end() and count < bsr_step; ++it) {
        // Enqueue enough bytes for continuous DL tx.
        const unsigned dl_bs_val = std::min(test_params.auto_dl_bs, std::numeric_limits<unsigned>::max() / 2);
        dl_buffer_state_indication_message dl_buf_st{it->second, LCID_MIN_DRB, dl_bs_val, this->next_slot_rx()};
        this->push_dl_buffer_state(dl_buf_st);
        ++count;
      }
    }

    if (test_params.auto_ul_bs > 0) {
      unsigned count = 0;
      for (auto it = start_it; it != rnti_map.end() and count < bsr_step; ++it) {
        // Enqueue BSR.
        const unsigned            ul_bsr = std::min(test_params.auto_ul_bs, std::numeric_limits<uint32_t>::max() / 2);
        ul_bsr_indication_message bsr{to_du_cell_index(0),
                                      it->second,
                                      it->first,
                                      bsr_format::LONG_BSR,
                                      ul_bsr_lcg_report_list{{uint_to_lcg_id(2), ul_bsr}}};
        this->push_bsr(bsr);
        ++count;
      }
    }

    scheduler_test_simulator::run_slot();
  }

  multi_ue_test_params          test_params;
  cell_config_builder_params    params;
  pucch_res_builder_test_helper pucch_cfg_builder;
  unsigned                      bsr_offset = 0;
};

class scheduler_rb_distribution_test : public ::testing::TestWithParam<multi_ue_test_params>,
                                       public scheduler_multi_ue_test
{
public:
  scheduler_rb_distribution_test() : scheduler_multi_ue_test(GetParam())
  {
    // Extract metrics to start from scratch in the test.
    this->request_metrics_on_next_slot();
    run_slot();
  }
};

template <typename T>
double jain_index(span<const scheduler_ue_metrics> ues, T scheduler_ue_metrics::*member)
{
  T sum = 0;
  T sq  = 0;
  for (const auto& u : ues) {
    sum += u.*member;
    sq += std::pow(u.*member, 2);
  }
  return pow(sum, 2) / static_cast<double>(ues.size() * sq);
}

template <typename T>
std::vector<T> extract(span<const scheduler_ue_metrics> ues, T scheduler_ue_metrics::*member)
{
  std::vector<T> res;
  for (const auto& u : ues) {
    res.push_back(u.*member);
  }
  return res;
}

TEST_P(scheduler_rb_distribution_test, rbs_fairly_distributed)
{
  test_logger.info("STATUS: All UEs were created. Starting to compute metrics...");
  unsigned max_slots = std::max(test_params.nof_ues * 4, 256U);
  for (unsigned i = 0; i != max_slots; ++i) {
    run_slot();
  }
  this->request_metrics_on_next_slot();
  run_slot();
  const scheduler_cell_metrics& metrics = *this->last_metrics();

  ASSERT_EQ(metrics.ue_metrics.size(), test_params.nof_ues);
  double pdsch_rbs_jain = jain_index(metrics.ue_metrics, &scheduler_ue_metrics::tot_pdsch_prbs_used);
  double pusch_rbs_jain = jain_index(metrics.ue_metrics, &scheduler_ue_metrics::tot_pusch_prbs_used);
  srslog::flush();
  EXPECT_GE(pdsch_rbs_jain, 0.75) << fmt::format(
      "Low PDSCH fairness index in {} UL slots, with #PDSCHs-per-slot={:.2}. UEs: {}",
      metrics.nof_dl_slots,
      metrics.dl_grants_count / static_cast<double>(metrics.nof_dl_slots),
      fmt::join(extract(metrics.ue_metrics, &scheduler_ue_metrics::tot_pdsch_prbs_used), ", "));
  EXPECT_GE(pusch_rbs_jain, 0.75) << fmt::format(
      "Low PUSCH fairness index in {} UL slots, with #PUSCHs-per-slot={:.2}. UEs: {}",
      metrics.nof_ul_slots,
      metrics.ul_grants_count / static_cast<double>(metrics.nof_ul_slots),
      fmt::join(extract(metrics.ue_metrics, &scheduler_ue_metrics::tot_pusch_prbs_used), ", "));
}

INSTANTIATE_TEST_SUITE_P(scheduler_rb_distribution_test,
                         scheduler_rb_distribution_test,
                         ::testing::Values(multi_ue_test_params{16},
                                           multi_ue_test_params{64},
                                           multi_ue_test_params{512}));
