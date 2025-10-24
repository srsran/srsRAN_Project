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

#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

// Change if you want to see logs.
srslog::basic_levels sched_log_level = srslog::basic_levels::warning;

struct multi_ue_test_params {
  unsigned nof_ues            = 1;
  unsigned nof_no_traffic_ues = 0;
  unsigned auto_dl_bs         = std::numeric_limits<uint32_t>::max() / 2;
  unsigned auto_ul_bs         = std::numeric_limits<uint32_t>::max() / 2;
};

void PrintTo(const multi_ue_test_params& value, ::std::ostream* os)
{
  *os << fmt::format("nof_ues={}, nof_no_traffic_ues={}, auto_dl_bs={}, auto_ul_bs={}",
                     value.nof_ues,
                     value.nof_no_traffic_ues,
                     value.auto_dl_bs,
                     value.auto_ul_bs);
}

class scheduler_multi_ue_test : public scheduler_test_simulator
{
public:
  scheduler_multi_ue_test(const multi_ue_test_params& tparams) :
    scheduler_test_simulator(scheduler_test_sim_config{.sched_cfg =
                                                           []() {
                                                             scheduler_expert_config cfg =
                                                                 config_helpers::make_default_scheduler_expert_config();
                                                             cfg.ue.max_pucchs_per_slot    = 120;
                                                             cfg.ue.max_ul_grants_per_slot = 128;
                                                             return cfg;
                                                           }(),
                                                       .max_scs  = subcarrier_spacing::kHz30,
                                                       .auto_uci = true,
                                                       .auto_crc = true}),
    test_params(tparams),
    params(cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30, bs_channel_bandwidth::MHz100))
  {
    this->logger.set_level(sched_log_level);

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

    // Fill no traffic UE set.
    {
      std::vector<unsigned> tmp(test_params.nof_ues, 0);
      std::iota(tmp.begin(), tmp.end(), 0);
      std::shuffle(tmp.begin(), tmp.end(), test_rgen::get());
      for (auto it = tmp.begin(); it != tmp.begin() + test_params.nof_no_traffic_ues; ++it) {
        no_traffic_ues.insert(to_rnti(0x4601 + *it));
      }
    }

    // Wait for all CQI reports to be received.
    for (unsigned i = 0; i != (unsigned)csi_period - 1; ++i) {
      run_slot();
    }
    // Extract metrics to start metrics from scratch in the test.
    this->request_metrics_on_next_slot();
    run_slot();

    srslog::flush();
  }

  void add_ue(rnti_t rnti)
  {
    auto ue_cfg     = sched_config_helper::create_default_sched_ue_creation_request(params, {LCID_MIN_DRB});
    ue_cfg.ue_index = to_du_ue_index((unsigned)rnti - 0x4601);
    ue_cfg.crnti    = rnti;
    report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                              "Failed to allocate PUCCH resources");
    csi_period = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                     ue_cfg.cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type)
                     .report_slot_period;
    scheduler_test_simulator::add_ue(ue_cfg, true);
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
        if (no_traffic_ues.count(it->first) > 0) {
          continue;
        }
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
        if (no_traffic_ues.count(it->first) > 0) {
          continue;
        }
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
  csi_report_periodicity        csi_period = csi_report_periodicity::slots320;

  std::set<rnti_t> no_traffic_ues;
};

class scheduler_rb_distribution_test : public ::testing::TestWithParam<multi_ue_test_params>,
                                       public scheduler_multi_ue_test
{
public:
  scheduler_rb_distribution_test() : scheduler_multi_ue_test(GetParam()) {}
};

template <typename T>
T sum(span<const scheduler_ue_metrics> ues, T scheduler_ue_metrics::*member)
{
  T sum = 0;
  for (const auto& u : ues) {
    sum += u.*member;
  }
  return sum;
}

template <typename T>
double jain_index(span<const scheduler_ue_metrics> ues, T scheduler_ue_metrics::*member)
{
  T      sum_val = sum(ues, member);
  double sq      = 0;
  for (const auto& u : ues) {
    sq += std::pow(u.*member, 2);
  }
  return pow(sum_val, 2) / static_cast<double>(ues.size() * sq);
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

} // namespace

TEST_P(scheduler_rb_distribution_test, when_equal_ue_cfgs_then_rbs_are_fully_utilized_and_fairly_distributed)
{
  const unsigned max_slots = 4096;
  test_logger.info("STATUS: {} UEs were created. {} UEs have traffic. Running for {} slots...",
                   test_params.nof_ues,
                   test_params.nof_ues - test_params.nof_no_traffic_ues,
                   max_slots);
  for (unsigned i = 0; i != max_slots; ++i) {
    run_slot();
  }
  this->request_metrics_on_next_slot();
  run_slot();
  const scheduler_cell_metrics& metrics = *this->last_metrics();

  ASSERT_EQ(metrics.ue_metrics.size(), test_params.nof_ues) << "Not all UEs reported metrics";

  const double pdsch_rbs_per_slot =
      sum(metrics.ue_metrics, &scheduler_ue_metrics::tot_pdsch_prbs_used) / static_cast<double>(metrics.nof_dl_slots);
  const double pusch_rbs_per_slot =
      sum(metrics.ue_metrics, &scheduler_ue_metrics::tot_pusch_prbs_used) / static_cast<double>(metrics.nof_ul_slots);

  std::vector<scheduler_ue_metrics> ues_with_traffic;
  std::vector<scheduler_ue_metrics> ues_with_no_traffic;
  for (const auto& u : metrics.ue_metrics) {
    if (no_traffic_ues.count(u.rnti) > 0) {
      ues_with_no_traffic.push_back(u);
    } else {
      ues_with_traffic.push_back(u);
    }
  }

  // Fairness metrics for UEs with traffic only.
  const double pdsch_rbs_jain   = jain_index(ues_with_traffic, &scheduler_ue_metrics::tot_pdsch_prbs_used);
  const double pdsch_brate_jain = jain_index(ues_with_traffic, &scheduler_ue_metrics::dl_brate_kbps);
  const double pusch_rbs_jain   = jain_index(ues_with_traffic, &scheduler_ue_metrics::tot_pusch_prbs_used);
  const double pusch_brate_jain = jain_index(ues_with_traffic, &scheduler_ue_metrics::ul_brate_kbps);

  test_logger.info(
      "Results for UEs with traffic:"
      "\n- DL: #slots={}, #pdschs_per_slot={:.3}, #rbs_per_slot={:.3}, brate={:.3}Mbps, jain_rb_fairness={:.3}, "
      "jain_brate_fairness={:.3}"
      "\n- UL: #slots={}, #puschs_per_slot={:.3}, #rbs_per_slot={:.3}, brate={:.3}Mbps, jain_rb_fairness={:.3}, "
      "jain_brate_fairness={:.3}",
      metrics.nof_dl_slots,
      metrics.dl_grants_count / static_cast<double>(metrics.nof_dl_slots),
      pdsch_rbs_per_slot,
      sum(metrics.ue_metrics, &scheduler_ue_metrics::dl_brate_kbps) / 1000,
      pdsch_rbs_jain,
      pdsch_brate_jain,
      metrics.nof_ul_slots,
      metrics.ul_grants_count / static_cast<double>(metrics.nof_ul_slots),
      pusch_rbs_per_slot,
      sum(metrics.ue_metrics, &scheduler_ue_metrics::ul_brate_kbps) / 1000,
      pusch_rbs_jain,
      pusch_brate_jain);

  // TEST CASE: Check that the UEs with no traffic have no PDSCH/PUSCH usage.
  ASSERT_LT(sum(ues_with_no_traffic, &scheduler_ue_metrics::tot_pdsch_prbs_used), 0.1 * metrics.nof_dl_slots);
  ASSERT_LT(sum(ues_with_no_traffic, &scheduler_ue_metrics::tot_pusch_prbs_used), 0.1 * metrics.nof_ul_slots);

  // TEST CASE: The RB usage should be close to the number of cell RBs.
  EXPECT_GE(pdsch_rbs_per_slot, 0.95 * cell_cfg().dl_cfg_common.init_dl_bwp.generic_params.crbs.length());
  EXPECT_GE(pusch_rbs_per_slot, 0.95 * cell_cfg().ul_cfg_common.init_ul_bwp.generic_params.crbs.length());

  // TEST CASE: Jain index for PDSCH and PUSCH RBs and bitrates should be very high.
  EXPECT_GE(pdsch_rbs_jain, 0.95) << fmt::format(
      "Low PDSCH fairness index in {} UL slots, with #PDSCHs-per-slot={:.2}. UEs: {}",
      metrics.nof_dl_slots,
      metrics.dl_grants_count / static_cast<double>(metrics.nof_dl_slots),
      fmt::join(extract(metrics.ue_metrics, &scheduler_ue_metrics::tot_pdsch_prbs_used), ", "));
  EXPECT_GE(pusch_rbs_jain, 0.95) << fmt::format(
      "Low PUSCH fairness index in {} UL slots, with #PUSCHs-per-slot={:.2}. UEs: {}",
      metrics.nof_ul_slots,
      metrics.ul_grants_count / static_cast<double>(metrics.nof_ul_slots),
      fmt::join(extract(metrics.ue_metrics, &scheduler_ue_metrics::tot_pusch_prbs_used), ", "));
  EXPECT_GE(pdsch_brate_jain, 0.95) << fmt::format("Low DL bitrate fairness index");
  EXPECT_GE(pusch_brate_jain, 0.95) << fmt::format("Low UL bitrate fairness index");
}

INSTANTIATE_TEST_SUITE_P(scheduler_rb_distribution_test,
                         scheduler_rb_distribution_test,
                         ::testing::Values(multi_ue_test_params{16},
                                           multi_ue_test_params{64},
                                           multi_ue_test_params{512},
                                           multi_ue_test_params{16, 4},
                                           multi_ue_test_params{512, 128}));

class scheduler_buffer_occupancy_test : public scheduler_multi_ue_test, public ::testing::Test
{
public:
  scheduler_buffer_occupancy_test() : scheduler_multi_ue_test(multi_ue_test_params{16, 0, 0, 0}) {}
};

TEST_F(scheduler_buffer_occupancy_test, when_dl_bo_is_set_then_enough_dl_tbs_is_scheduled)
{
  ASSERT_FALSE(run_slot_until([this]() { return this->last_sched_result()->dl.ue_grants.size() > 0; }, 1000))
      << "If no DL BO is forwarded, no DL grant should be scheduled";

  // Push DL BO for all UEs.
  std::vector<unsigned> ue_dl_bos(test_params.nof_ues);
  std::vector<unsigned> ue_dl_drb_sched(test_params.nof_ues);
  for (unsigned i = 0; i != test_params.nof_ues; ++i) {
    ue_dl_bos[i] = test_rgen::uniform_int<unsigned>(1, 20000);
    dl_buffer_state_indication_message dl_buf_st{to_du_ue_index(i), LCID_MIN_DRB, ue_dl_bos[i], this->next_slot_rx()};
    this->push_dl_buffer_state(dl_buf_st);
  }

  // The scheduler should schedule enough bytes for each UE.
  unsigned max_nof_slots = 10000;
  unsigned ues_completed = 0;
  for (unsigned i = 0; i < max_nof_slots and ues_completed < test_params.nof_ues; ++i) {
    run_slot();

    for (const auto& grant : last_sched_result()->dl.ue_grants) {
      for (auto& tb : grant.tb_list[0].lc_chs_to_sched) {
        if (tb.lcid == LCID_MIN_DRB) {
          auto& ue_sched_bytes = ue_dl_drb_sched[grant.context.ue_index];
          if (ue_sched_bytes < ue_dl_bos[grant.context.ue_index] and
              ue_sched_bytes + tb.sched_bytes >= ue_dl_bos[grant.context.ue_index]) {
            ues_completed++;
            if (ues_completed == test_params.nof_ues) {
              test_logger.info("All UEs have received enough DL bytes after {} slots", i + 1);
              // Run some extra slots to ensure that any overhead is scheduled.
              max_nof_slots = i + 100;
            }
          }
          ue_sched_bytes += tb.sched_bytes;
        }
      }
    }
  }
  ASSERT_EQ(ues_completed, test_params.nof_ues) << "Not enough bytes scheduled for some UEs";

  // We have the guarantee now that all UEs have been scheduled sufficiently.
  // However, we also want to ensure they were not over-scheduled.
  for (unsigned i = 0; i != test_params.nof_ues; ++i) {
    ASSERT_LE(ue_dl_drb_sched[i], (ue_dl_bos[i] * 1.1 + 10));
  }
}
