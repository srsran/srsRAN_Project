/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

class scheduler_qos_test : public scheduler_test_simulator
{
  struct ue_stats {
    uint64_t dl_bytes_sum = 0;
    uint64_t ul_bytes_sum = 0;
  };

public:
  scheduler_qos_test(unsigned nof_gbr_ues_         = 1,
                     unsigned nof_non_gbr_ues_     = 7,
                     unsigned gbr_dl_bitrate_mbps_ = 10e6,
                     unsigned gbr_ul_bitrate_mbps_ = 5e6) :
    scheduler_test_simulator(4, subcarrier_spacing::kHz30),
    nof_gbr_ues(nof_gbr_ues_),
    nof_non_gbr_ues(nof_non_gbr_ues_),
    gbr_dl_bitrate_mbps(gbr_dl_bitrate_mbps_),
    gbr_ul_bitrate_mbps(gbr_ul_bitrate_mbps_)
  {
    const unsigned TEST_NOF_UES = nof_gbr_ues + nof_non_gbr_ues;
    ue_stats_map.resize(TEST_NOF_UES);

    params = cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30);

    // Add Cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    cell_cfg_req.rrm_policy_members.resize(1);
    cell_cfg_req.rrm_policy_members[0].rrc_member.s_nssai.sst = slice_service_type{1};
    cell_cfg_req.rrm_policy_members[0].policy_sched_cfg =
        time_pf_scheduler_expert_config{2.0, time_pf_scheduler_expert_config::weight_function::gbr_prioritized};
    this->add_cell(cell_cfg_req);

    srs_du::pucch_builder_params pucch_basic_params{.nof_ue_pucch_f0_or_f1_res_harq       = 8,
                                                    .nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8,
                                                    .nof_sr_resources                     = 8,
                                                    .nof_csi_resources                    = 8};
    auto&                        f1_params = pucch_basic_params.f0_or_f1_params.emplace<srs_du::pucch_f1_params>();
    f1_params.nof_cyc_shifts               = srs_du::nof_cyclic_shifts::twelve;
    f1_params.occ_supported                = true;
    pucch_cfg_builder.setup(cell_cfg_list[0], pucch_basic_params);

    du_ue_index_t ue_idx;
    auto          ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(params, {LCID_MIN_DRB});
    // Add UEs with GBR
    for (unsigned i = 0; i != nof_gbr_ues; ++i) {
      ue_idx                                                      = to_du_ue_index(i);
      ue_cfg.ue_index                                             = ue_idx;
      ue_cfg.crnti                                                = to_rnti(0x4601 + (unsigned)ue_idx);
      ue_cfg.cfg.lc_config_list.value()[2].rrm_policy.s_nssai.sst = slice_service_type{1};
      auto& qos_info                                              = ue_cfg.cfg.lc_config_list.value()[2].qos.emplace();
      qos_info.qos.average_window_ms                              = 100;
      qos_info.gbr_qos_info.emplace();
      qos_info.gbr_qos_info.value().gbr_dl = gbr_dl_bitrate_mbps;
      qos_info.gbr_qos_info.value().gbr_ul = 5e6;
      report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                                "Failed to allocate PUCCH resources");
      this->add_ue(ue_cfg);
    }

    // Add UEs without GBR.
    ue_cfg.cfg.lc_config_list.value()[2].qos.reset();
    for (unsigned i = 0; i != nof_non_gbr_ues; ++i) {
      ue_idx          = to_du_ue_index(i + nof_gbr_ues);
      ue_cfg.ue_index = ue_idx;
      ue_cfg.crnti    = to_rnti(0x4601 + (unsigned)ue_idx);
      report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                                "Failed to allocate PUCCH resources");
      this->add_ue(ue_cfg);
    }

    // Enqueue enough bytes for continuous DL tx.
    const unsigned DL_BS_VALUE = std::numeric_limits<unsigned>::max() / 2;
    for (unsigned i = 0; i != TEST_NOF_UES; ++i) {
      dl_buffer_state_indication_message dl_buf_st{to_du_ue_index(i), LCID_MIN_DRB, DL_BS_VALUE};
      this->push_dl_buffer_state(dl_buf_st);
    }

    // Enqueue BSR.
    const unsigned BSR_VALUE = std::numeric_limits<u_int32_t>::max() / 2;
    for (unsigned i = 0; i != TEST_NOF_UES; ++i) {
      ul_bsr_indication_message bsr{to_du_cell_index(0),
                                    to_du_ue_index(i),
                                    to_rnti(0x4601 + i),
                                    bsr_format::LONG_BSR,
                                    ul_bsr_lcg_report_list{{uint_to_lcg_id(2), BSR_VALUE}}};
      this->push_bsr(bsr);
    }
  }

  void run_slot()
  {
    scheduler_test_simulator::run_slot();

    // Handle UCI and CRC indications.
    uci_indication uci_ind;
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.slot_rx    = this->last_result_slot();
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = last_result_slot();

    span<const dl_msg_alloc>  ue_grants = this->last_sched_res_list[0]->dl.ue_grants;
    span<const ul_sched_info> ul_grants = this->last_sched_res_list[0]->ul.puschs;

    // Handle PUCCHs.
    for (const pucch_info& pucch : this->last_sched_res_list[to_du_cell_index(0)]->ul.pucchs) {
      if (pucch.format == pucch_format::FORMAT_1 and pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
        // Skip SRs for this test.
        continue;
      }
      du_ue_index_t ue_idx = to_du_ue_index((unsigned)pucch.crnti - 0x4601);
      uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_idx, pucch));
    }

    // Handle CRCs and PUSCH UCIs.
    for (const ul_sched_info& grant : ul_grants) {
      crc_ind.crcs.emplace_back(test_helper::create_crc_pdu_indication(grant));
      if (grant.uci.has_value()) {
        uci_ind.ucis.push_back(
            test_helper::create_uci_indication_pdu(grant.pusch_cfg.rnti, grant.context.ue_index, grant.uci.value()));
      }
    }

    if (not uci_ind.ucis.empty()) {
      this->sched->handle_uci_indication(uci_ind);
    }
    if (not crc_ind.crcs.empty()) {
      this->sched->handle_crc_indication(crc_ind);
    }

    // Register DL sched bytes.
    for (const dl_msg_alloc& grant : ue_grants) {
      ue_stats_map[grant.context.ue_index].dl_bytes_sum += grant.pdsch_cfg.codewords[0].tb_size_bytes;
    }
    // Register UL sched bytes.
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

  const unsigned nof_gbr_ues;
  const unsigned nof_non_gbr_ues;
  const unsigned gbr_dl_bitrate_mbps;
  const unsigned gbr_ul_bitrate_mbps;

  cell_config_builder_params params;

  pucch_res_builder_test_helper pucch_cfg_builder;

  std::vector<ue_stats> ue_stats_map;

  unsigned metric_slot_count = 0;
};

/// Test case when 1 UE has a GBR bearer and the remaining UEs do not.
class scheduler_1_gbr_ue_qos_test : public scheduler_qos_test, public ::testing::Test
{
public:
  scheduler_1_gbr_ue_qos_test() : scheduler_qos_test(1) {}
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

  fmt::memory_buffer dl_fmtbuf;
  fmt::memory_buffer ul_fmtbuf;
  for (unsigned i = 0; i != ue_dl_rate_mbps.size(); ++i) {
    fmt::format_to(std::back_inserter(dl_fmtbuf), " {:.3}", ue_dl_rate_mbps[i]);
    fmt::format_to(std::back_inserter(ul_fmtbuf), " {:.3}", ue_ul_rate_mbps[i]);
  }
  test_logger.info("DL bit rates [Mbps]: [{}]", fmt::to_string(dl_fmtbuf));
  test_logger.info("UL bit rates [Mbps]: [{}]", fmt::to_string(ul_fmtbuf));

  const unsigned GBR_UE_INDEX = 0;
  for (unsigned i = 1; i != ue_stats_map.size(); ++i) {
    ASSERT_GT(ue_dl_rate_mbps[GBR_UE_INDEX], ue_dl_rate_mbps[i]) << "UE DL GBR rate < UE DL non-GBR rate";
    ASSERT_GT(ue_ul_rate_mbps[GBR_UE_INDEX], ue_ul_rate_mbps[i]) << "UE UL GBR rate < UE UL non-GBR rate";
  }
  ASSERT_GT(ue_dl_rate_mbps[GBR_UE_INDEX], static_cast<double>(gbr_dl_bitrate_mbps) * 1e-6 * 0.95)
      << "UE DL GBR rate < expected DL GBR";
  ASSERT_GT(ue_ul_rate_mbps[GBR_UE_INDEX], static_cast<double>(gbr_ul_bitrate_mbps) * 1e-6 * 0.95)
      << "UE UL GBR rate < expected UL GBR";
}

class scheduler_saturated_gbr_ue_qos_test : public scheduler_qos_test, public ::testing::Test
{
public:
  scheduler_saturated_gbr_ue_qos_test() : scheduler_qos_test(2, 6, 25e6, 10e6) {}
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

  fmt::memory_buffer dl_fmtbuf;
  fmt::memory_buffer ul_fmtbuf;
  for (unsigned i = 0; i != ue_dl_rate_mbps.size(); ++i) {
    fmt::format_to(std::back_inserter(dl_fmtbuf), " {:.3}", ue_dl_rate_mbps[i]);
    fmt::format_to(std::back_inserter(ul_fmtbuf), " {:.3}", ue_ul_rate_mbps[i]);
  }
  test_logger.info("DL bit rates [Mbps]: [{}]", fmt::to_string(dl_fmtbuf));
  test_logger.info("UL bit rates [Mbps]: [{}]", fmt::to_string(ul_fmtbuf));

  double min_dl_gbr_rate = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.begin() + nof_gbr_ues);
  double min_ul_gbr_rate = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.begin() + nof_gbr_ues);
  for (unsigned i = nof_gbr_ues; i != ue_stats_map.size(); ++i) {
    ASSERT_GT(min_dl_gbr_rate, ue_dl_rate_mbps[i]) << "UE DL GBR rate < UE DL non-GBR rate";
    ASSERT_GT(min_ul_gbr_rate, ue_ul_rate_mbps[i]) << "UE UL GBR rate < UE UL non-GBR rate";
  }
  double min_dl_rate = *std::min_element(ue_dl_rate_mbps.begin(), ue_dl_rate_mbps.end());
  double min_ul_rate = *std::min_element(ue_ul_rate_mbps.begin(), ue_ul_rate_mbps.end());
  ASSERT_GT(min_dl_rate, 0.5);
  ASSERT_GT(min_ul_rate, 0.5);
}
