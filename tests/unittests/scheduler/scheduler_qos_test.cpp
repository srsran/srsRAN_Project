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

#include "test_utils/indication_generators.h"
#include "test_utils/result_test_helpers.h"
#include "test_utils/scheduler_test_simulator.h"
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include <gtest/gtest.h>

using namespace srsran;

static logical_channel_config::qos_info make_qos(qos_prio_level_t          qos_prio = qos_prio_level_t::max(),
                                                 arp_prio_level_t          arp_prio = arp_prio_level_t::max(),
                                                 std::chrono::milliseconds pdb      = std::chrono::milliseconds{2000},
                                                 unsigned                  dl_gbr   = 0,
                                                 unsigned                  ul_gbr   = 0)
{
  logical_channel_config::qos_info qos{};
  qos.qos.average_window_ms      = 100;
  qos.qos.priority               = qos_prio;
  qos.qos.packet_delay_budget_ms = pdb.count();

  if (dl_gbr != 0 and ul_gbr != 0) {
    qos.gbr_qos_info.emplace();
    qos.gbr_qos_info.value().gbr_dl = dl_gbr;
    qos.gbr_qos_info.value().gbr_ul = dl_gbr;
  }
  qos.arp_priority = arp_prio;

  return qos;
}

static std::string format_bitrates(span<const double> rates)
{
  fmt::memory_buffer fmtbuf;
  for (unsigned i = 0; i != rates.size(); ++i) {
    fmt::format_to(std::back_inserter(fmtbuf), " {:.3}", rates[i]);
  }
  return fmt::to_string(fmtbuf);
}

class scheduler_qos_test : public scheduler_test_simulator, public ::testing::Test
{
  struct ue_stats {
    uint64_t dl_bytes_sum = 0;
    uint64_t ul_bytes_sum = 0;
  };

public:
  scheduler_qos_test() :
    scheduler_test_simulator(
        scheduler_test_sim_config{.max_scs = subcarrier_spacing::kHz30, .auto_uci = true, .auto_crc = true})
  {
    params = cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30);

    // Add Cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    cell_cfg_req.rrm_policy_members.resize(1);
    cell_cfg_req.rrm_policy_members[0].rrc_member.s_nssai.sst = slice_service_type{1};
    cell_cfg_req.rrm_policy_members[0].policy_sched_cfg =
        time_qos_scheduler_expert_config{time_qos_scheduler_expert_config::weight_function::gbr_prioritized, 2.0};
    this->add_cell(cell_cfg_req);

    // Create PUCCH builder that will be used to add UEs.
    pucch_builder_params pucch_basic_params{.nof_ue_pucch_f0_or_f1_res_harq       = 8,
                                            .nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8,
                                            .nof_sr_resources                     = 8,
                                            .nof_csi_resources                    = 8};
    auto&                f1_params = pucch_basic_params.f0_or_f1_params.emplace<pucch_f1_params>();
    f1_params.nof_cyc_shifts       = pucch_nof_cyclic_shifts::twelve;
    f1_params.occ_supported        = true;
    pucch_cfg_builder.setup(cell_cfg(), pucch_basic_params);
  }

  void add_ue_with_drb_qos(logical_channel_config::qos_info drb_qos)
  {
    // Add UE to scheduler.
    du_ue_index_t next_ue_idx = to_du_ue_index(ue_stats_map.size());
    auto          ue_cfg      = sched_config_helper::create_default_sched_ue_creation_request(params, {LCID_MIN_DRB});
    ue_cfg.ue_index           = next_ue_idx;
    ue_cfg.crnti              = to_rnti(0x4601 + (unsigned)next_ue_idx);
    ue_cfg.cfg.lc_config_list.value()[2].rrm_policy.s_nssai.sst = slice_service_type{1};
    ue_cfg.cfg.lc_config_list.value()[2].qos                    = drb_qos;
    report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                              "Failed to allocate PUCCH resources");
    scheduler_test_simulator::add_ue(ue_cfg);

    // Increase the metrics map.
    ue_stats_map.push_back({});

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

  void run_slot()
  {
    scheduler_test_simulator::run_slot();

    // Register DL sched bytes.
    span<const dl_msg_alloc> ue_grants = this->last_sched_result()->dl.ue_grants;
    for (const dl_msg_alloc& grant : ue_grants) {
      ue_stats_map[grant.context.ue_index].dl_bytes_sum += grant.pdsch_cfg.codewords[0].tb_size_bytes;
    }
    // Register UL sched bytes.
    span<const ul_sched_info> ul_grants = this->last_sched_result()->ul.puschs;
    for (const ul_sched_info& grant : ul_grants) {
      ue_stats_map[grant.context.ue_index].ul_bytes_sum += grant.pusch_cfg.tb_size_bytes;
    }

    metric_slot_count++;
  }

  void reset_stats()
  {
    metric_slot_count = 0;
    for (auto& ue_st : ue_stats_map) {
      ue_st.dl_bytes_sum = 0;
      ue_st.ul_bytes_sum = 0;
    }
  }

  std::vector<double> get_ue_dl_bitrate_mbps() const
  {
    double nof_sec_elapsed = (metric_slot_count / (double)get_nof_slots_per_subframe(params.scs_common)) / 1000;
    std::vector<double> ue_dl_rate_mbps(ue_stats_map.size(), 0);
    for (unsigned i = 0; i != ue_stats_map.size(); ++i) {
      ue_dl_rate_mbps[i] = ue_stats_map[i].dl_bytes_sum * 8U * 1e-6 / nof_sec_elapsed;
    }
    return ue_dl_rate_mbps;
  }

  std::vector<double> get_ue_ul_bitrate_mbps() const
  {
    double nof_sec_elapsed = (metric_slot_count / (double)get_nof_slots_per_subframe(params.scs_common)) / 1000;
    std::vector<double> ue_ul_rate_mbps(ue_stats_map.size(), 0);
    for (unsigned i = 0; i != ue_stats_map.size(); ++i) {
      ue_ul_rate_mbps[i] = ue_stats_map[i].ul_bytes_sum * 8U * 1e-6 / nof_sec_elapsed;
    }
    return ue_ul_rate_mbps;
  }

  cell_config_builder_params params;

  pucch_res_builder_test_helper pucch_cfg_builder;

  std::vector<ue_stats> ue_stats_map;

  unsigned metric_slot_count = 0;
};

/// Test case when 1 UE has a GBR bearer and the remaining UEs do not.
class scheduler_1_gbr_ue_qos_test : public scheduler_qos_test
{
public:
  scheduler_1_gbr_ue_qos_test()
  {
    auto gbr_qos     = make_qos(qos_prio_level_t::max(),
                            arp_prio_level_t::max(),
                            std::chrono::milliseconds(2000),
                            gbr_dl_bitrate_bps,
                            gbr_ul_bitrate_bps);
    auto non_gbr_qos = make_qos(qos_prio_level_t::max(), arp_prio_level_t::max(), std::chrono::milliseconds(2000));

    add_ue_with_drb_qos(gbr_qos);

    const unsigned non_gbr_ues = 7;
    for (unsigned i = 0; i != non_gbr_ues; ++i) {
      add_ue_with_drb_qos(non_gbr_qos);
    }
  }

  const unsigned gbr_dl_bitrate_bps = 10e6;
  const unsigned gbr_ul_bitrate_bps = 4e6;
};

TEST_F(scheduler_1_gbr_ue_qos_test, when_ue_has_gbr_drb_it_gets_higher_priority)
{
  // number of slots discarded that were used to fill average window.
  const unsigned PREAMBLE_SLOT_RUNS = 200;
  const unsigned MAX_NOF_SLOT_RUNS  = 1000;

  for (unsigned r = 0; r != MAX_NOF_SLOT_RUNS + PREAMBLE_SLOT_RUNS; ++r) {
    this->run_slot();

    if (r == PREAMBLE_SLOT_RUNS) {
      // Reset stats when we pass the preamble.
      this->reset_stats();
    }
  }

  std::vector<double> ue_dl_rate_mbps = this->get_ue_dl_bitrate_mbps();
  std::vector<double> ue_ul_rate_mbps = this->get_ue_ul_bitrate_mbps();

  test_logger.info("DL bit rates [Mbps]: [{}]", format_bitrates(ue_dl_rate_mbps));
  test_logger.info("UL bit rates [Mbps]: [{}]", format_bitrates(ue_ul_rate_mbps));

  const unsigned GBR_UE_INDEX = 0;
  for (unsigned i = 1; i != ue_stats_map.size(); ++i) {
    ASSERT_GT(ue_dl_rate_mbps[GBR_UE_INDEX], ue_dl_rate_mbps[i]) << "UE DL GBR rate < UE DL non-GBR rate";
    ASSERT_GT(ue_ul_rate_mbps[GBR_UE_INDEX], ue_ul_rate_mbps[i]) << "UE UL GBR rate < UE UL non-GBR rate";
  }
  ASSERT_GT(ue_dl_rate_mbps[GBR_UE_INDEX], static_cast<double>(gbr_dl_bitrate_bps) * 1e-6 * 0.9)
      << "UE DL GBR rate < expected DL GBR";
  ASSERT_GT(ue_ul_rate_mbps[GBR_UE_INDEX], static_cast<double>(gbr_ul_bitrate_bps) * 1e-6 * 0.9)
      << "UE UL GBR rate < expected UL GBR";
}

class scheduler_saturated_gbr_ue_qos_test : public scheduler_qos_test
{
public:
  scheduler_saturated_gbr_ue_qos_test()
  {
    auto gbr_qos     = make_qos(qos_prio_level_t::max(),
                            arp_prio_level_t::max(),
                            std::chrono::milliseconds(2000),
                            gbr_dl_bitrate_bps,
                            gbr_ul_bitrate_bps);
    auto non_gbr_qos = make_qos(qos_prio_level_t::max(), arp_prio_level_t::max(), std::chrono::milliseconds(2000));

    for (unsigned i = 0; i != nof_gbr_ues; ++i) {
      add_ue_with_drb_qos(gbr_qos);
    }

    for (unsigned i = 0; i != nof_non_gbr_ues; ++i) {
      add_ue_with_drb_qos(non_gbr_qos);
    }
  }

  const unsigned nof_gbr_ues        = 2;
  const unsigned nof_non_gbr_ues    = 6;
  const unsigned gbr_dl_bitrate_bps = 25e6;
  const unsigned gbr_ul_bitrate_bps = 10e6;
};

TEST_F(scheduler_saturated_gbr_ue_qos_test, when_gbrs_saturate_channel_then_non_gbr_still_get_some_traffic)
{
  // number of slots discarded that were used to fill average window.
  const unsigned PREAMBLE_SLOT_RUNS = 200;
  const unsigned MAX_NOF_SLOT_RUNS  = 1000;

  for (unsigned r = 0; r != MAX_NOF_SLOT_RUNS + PREAMBLE_SLOT_RUNS; ++r) {
    this->run_slot();

    if (r == PREAMBLE_SLOT_RUNS) {
      // Reset stats when we pass the preamble.
      this->reset_stats();
    }
  }

  std::vector<double> ue_dl_rate_mbps = this->get_ue_dl_bitrate_mbps();
  std::vector<double> ue_ul_rate_mbps = this->get_ue_ul_bitrate_mbps();

  test_logger.info("DL bit rates [Mbps]: [{}]", format_bitrates(ue_dl_rate_mbps));
  test_logger.info("UL bit rates [Mbps]: [{}]", format_bitrates(ue_ul_rate_mbps));

  double min_dl_gbr_rate     = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.begin() + nof_gbr_ues);
  double min_ul_gbr_rate     = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.begin() + nof_gbr_ues);
  double max_dl_non_gbr_rate = *std::max_element(ue_dl_rate_mbps.begin() + nof_gbr_ues, ue_dl_rate_mbps.end());
  double max_ul_non_gbr_rate = *std::max_element(ue_ul_rate_mbps.begin() + nof_gbr_ues, ue_ul_rate_mbps.end());
  double min_dl_rate         = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.end());
  double min_ul_rate         = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.end());
  double max_dl_rate         = *std::max_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.end());
  double max_ul_rate         = *std::max_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.end());

  ASSERT_LT(max_dl_rate, gbr_dl_bitrate_bps) << "The DL GBR is not saturated";
  ASSERT_LT(max_ul_rate, gbr_ul_bitrate_bps) << "The UL GBR is not saturated";

  ASSERT_GT(min_dl_gbr_rate, max_dl_non_gbr_rate) << "UE DL GBR rate < UE DL non-GBR rate";
  ASSERT_GT(min_ul_gbr_rate, max_ul_non_gbr_rate) << "UE DL GBR rate < UE DL non-GBR rate";

  ASSERT_GT(min_dl_rate, 0.1) << "UEs are starved";
  ASSERT_GT(min_ul_rate, 0.1) << "UEs are starved";
}

class scheduler_qos_priority_test : public scheduler_qos_test
{
public:
  scheduler_qos_priority_test()
  {
    auto prio_qos     = make_qos(high_prio);
    auto non_prio_qos = make_qos(low_prio);

    for (unsigned i = 0; i != nof_prio_ues; ++i) {
      add_ue_with_drb_qos(prio_qos);
    }

    for (unsigned i = 0; i != nof_non_prio_ues; ++i) {
      add_ue_with_drb_qos(non_prio_qos);
    }
  }

  const unsigned         nof_prio_ues     = 2;
  const unsigned         nof_non_prio_ues = 6;
  const qos_prio_level_t high_prio        = qos_prio_level_t{0};
  const qos_prio_level_t low_prio         = qos_prio_level_t::max();
};

TEST_F(scheduler_qos_priority_test, when_channel_is_saturated_then_lc_with_higher_qos_prio_get_higher_traffic)
{
  const unsigned MAX_NOF_SLOT_RUNS = 1000;

  for (unsigned r = 0; r != MAX_NOF_SLOT_RUNS; ++r) {
    this->run_slot();
  }

  std::vector<double> ue_dl_rate_mbps = this->get_ue_dl_bitrate_mbps();
  std::vector<double> ue_ul_rate_mbps = this->get_ue_ul_bitrate_mbps();

  test_logger.info("DL bit rates [Mbps]: [{}]", format_bitrates(ue_dl_rate_mbps));
  test_logger.info("UL bit rates [Mbps]: [{}]", format_bitrates(ue_ul_rate_mbps));

  double min_dl_rate          = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.end());
  double min_ul_rate          = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.end());
  double min_dl_prio_rate     = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.begin() + nof_prio_ues);
  double min_ul_prio_rate     = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.begin() + nof_prio_ues);
  double max_dl_non_prio_rate = *std::max_element(ue_dl_rate_mbps.begin() + nof_prio_ues, ue_dl_rate_mbps.end());
  double max_ul_non_prio_rate = *std::max_element(ue_ul_rate_mbps.begin() + nof_prio_ues, ue_ul_rate_mbps.end());

  ASSERT_GT(min_dl_prio_rate, max_dl_non_prio_rate) << "Priorities are not respected in DL";
  ASSERT_GT(min_ul_prio_rate, max_ul_non_prio_rate) << "Priorities are not respected in DL";

  ASSERT_GT(min_dl_rate, 0.1) << "UEs are starved";
  ASSERT_GT(min_ul_rate, 0.1) << "UEs are starved";
}

class scheduler_arp_priority_test : public scheduler_qos_test
{
public:
  scheduler_arp_priority_test()
  {
    auto prio_qos     = make_qos(qos_prio_level_t::max(), high_prio);
    auto non_prio_qos = make_qos(qos_prio_level_t::max(), low_prio);

    for (unsigned i = 0; i != nof_prio_ues; ++i) {
      add_ue_with_drb_qos(prio_qos);
    }

    for (unsigned i = 0; i != nof_non_prio_ues; ++i) {
      add_ue_with_drb_qos(non_prio_qos);
    }
  }

  const unsigned nof_prio_ues     = 2;
  const unsigned nof_non_prio_ues = 6;
  // Note: ARP Priority Level 0 is reserved.
  const arp_prio_level_t high_prio = arp_prio_level_t{1};
  const arp_prio_level_t low_prio  = arp_prio_level_t::max();
};

TEST_F(scheduler_arp_priority_test, when_channel_is_saturated_then_lc_with_higher_arp_prio_get_higher_traffic)
{
  const unsigned MAX_NOF_SLOT_RUNS = 1000;

  for (unsigned r = 0; r != MAX_NOF_SLOT_RUNS; ++r) {
    this->run_slot();
  }

  std::vector<double> ue_dl_rate_mbps = this->get_ue_dl_bitrate_mbps();
  std::vector<double> ue_ul_rate_mbps = this->get_ue_ul_bitrate_mbps();

  test_logger.info("DL bit rates [Mbps]: [{}]", format_bitrates(ue_dl_rate_mbps));
  test_logger.info("UL bit rates [Mbps]: [{}]", format_bitrates(ue_ul_rate_mbps));

  double min_dl_rate          = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.end());
  double min_ul_rate          = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.end());
  double min_dl_prio_rate     = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.begin() + nof_prio_ues);
  double min_ul_prio_rate     = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.begin() + nof_prio_ues);
  double max_dl_non_prio_rate = *std::max_element(ue_dl_rate_mbps.begin() + nof_prio_ues, ue_dl_rate_mbps.end());
  double max_ul_non_prio_rate = *std::max_element(ue_ul_rate_mbps.begin() + nof_prio_ues, ue_ul_rate_mbps.end());

  ASSERT_GT(min_dl_prio_rate, max_dl_non_prio_rate) << "Priorities are not respected in DL";
  ASSERT_GT(min_ul_prio_rate, max_ul_non_prio_rate) << "Priorities are not respected in DL";

  ASSERT_GT(min_dl_rate, 0.1) << "UEs are starved";
  ASSERT_GT(min_ul_rate, 0.1) << "UEs are starved";
}

class scheduler_pdb_qos_test : public scheduler_qos_test
{
public:
  scheduler_pdb_qos_test()
  {
    auto low_pdb_qos  = make_qos(qos_prio_level_t::max(), arp_prio_level_t::max(), low_pdb);
    auto high_pdb_qos = make_qos(qos_prio_level_t::max(), arp_prio_level_t::max(), hi_pdb);

    for (unsigned i = 0; i != nof_low_pdb_ues; ++i) {
      add_ue_with_drb_qos(low_pdb_qos);
    }

    for (unsigned i = 0; i != nof_hi_pdb_ues; ++i) {
      add_ue_with_drb_qos(high_pdb_qos);
    }
  }

  const unsigned            nof_low_pdb_ues = 2;
  const unsigned            nof_hi_pdb_ues  = 6;
  std::chrono::milliseconds low_pdb         = std::chrono::milliseconds(100);
  std::chrono::milliseconds hi_pdb          = std::chrono::milliseconds(2000);
};

TEST_F(scheduler_pdb_qos_test, low_qos_pdb_logical_channels_get_higher_priority)
{
  const unsigned MAX_NOF_SLOT_RUNS = 1000;

  for (unsigned r = 0; r != MAX_NOF_SLOT_RUNS; ++r) {
    this->run_slot();
  }

  std::vector<double> ue_dl_rate_mbps = this->get_ue_dl_bitrate_mbps();

  test_logger.info("DL bit rates [Mbps]: [{}]", format_bitrates(ue_dl_rate_mbps));

  double min_dl_rate          = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.end());
  double min_dl_prio_rate     = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.begin() + nof_low_pdb_ues);
  double max_dl_non_prio_rate = *std::max_element(ue_dl_rate_mbps.begin() + nof_low_pdb_ues, ue_dl_rate_mbps.end());

  ASSERT_GT(min_dl_prio_rate, max_dl_non_prio_rate) << "Priorities are not respected in DL";

  ASSERT_GT(min_dl_rate, 0.1) << "UEs are starved";
}
