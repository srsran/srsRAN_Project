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

class scheduler_qos_test : public scheduler_test_simulator, public ::testing::Test
{
  struct ue_stats {
    uint64_t dl_bytes_sum = 0;
    uint64_t ul_bytes_sum = 0;
  };

public:
  scheduler_qos_test() : scheduler_test_simulator(4, subcarrier_spacing::kHz30)
  {
    const unsigned TEST_NOF_UES = 8;
    ue_stats_map.resize(TEST_NOF_UES);

    params = cell_config_builder_profiles::tdd(subcarrier_spacing::kHz30);

    // Add Cell.
    auto cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(params);
    cell_cfg_req.rrm_policy_members.resize(1);
    cell_cfg_req.rrm_policy_members[0].rrc_member.s_nssai.sst = slice_service_type{1};
    cell_cfg_req.rrm_policy_members[0].policy_sched_cfg       = time_pf_scheduler_expert_config{};
    this->add_cell(cell_cfg_req);

    srs_du::pucch_builder_params pucch_basic_params{.nof_ue_pucch_f0_or_f1_res_harq       = 8,
                                                    .nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8,
                                                    .nof_sr_resources                     = 8,
                                                    .nof_csi_resources                    = 8};
    auto&                        f1_params = pucch_basic_params.f0_or_f1_params.emplace<srs_du::pucch_f1_params>();
    f1_params.nof_cyc_shifts               = srs_du::nof_cyclic_shifts::twelve;
    f1_params.occ_supported                = true;
    pucch_cfg_builder.setup(cell_cfg_list[0], pucch_basic_params);

    // Add UE0 with GBR
    du_ue_index_t ue_idx = to_du_ue_index(0);
    auto          ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(params, {LCID_MIN_DRB});
    ue_cfg.ue_index      = ue_idx;
    ue_cfg.crnti         = to_rnti(0x4601 + (unsigned)ue_idx);
    ue_cfg.cfg.lc_config_list.value()[2].rrm_policy.s_nssai.sst = slice_service_type{1};
    ue_cfg.cfg.lc_config_list.value()[2].qos.emplace();
    ue_cfg.cfg.lc_config_list.value()[2].qos.value().gbr_qos_info.emplace();
    ue_cfg.cfg.lc_config_list.value()[2].qos.value().gbr_qos_info.value().gbr_dl = 10e6;
    ue_cfg.cfg.lc_config_list.value()[2].qos.value().gbr_qos_info.value().gbr_ul = 5e6;
    report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                              "Failed to allocate PUCCH resources");
    this->add_ue(ue_cfg);

    // Add UEs without GBR.
    ue_cfg.cfg.lc_config_list.value()[2].qos.reset();
    for (unsigned i = 1; i != TEST_NOF_UES; ++i) {
      ue_idx          = to_du_ue_index(i);
      ue_cfg.ue_index = ue_idx;
      ue_cfg.crnti    = to_rnti(0x4601 + (unsigned)ue_idx);
      report_fatal_error_if_not(pucch_cfg_builder.add_build_new_ue_pucch_cfg(ue_cfg.cfg.cells.value()[0].serv_cell_cfg),
                                "Failed to allocate PUCCH resources");
      this->add_ue(ue_cfg);
    }

    // Enqueue enough bytes for continuous DL tx.
    for (unsigned i = 0; i != TEST_NOF_UES; ++i) {
      dl_buffer_state_indication_message dl_buf_st{to_du_ue_index(i), LCID_MIN_DRB, 10000000};
      this->push_dl_buffer_state(dl_buf_st);
    }

    // Enqueue BSR.
    for (unsigned i = 0; i != TEST_NOF_UES; ++i) {
      ul_bsr_indication_message bsr{to_du_cell_index(0),
                                    to_du_ue_index(i),
                                    to_rnti(0x4601 + i),
                                    bsr_format::LONG_BSR,
                                    ul_bsr_lcg_report_list{{uint_to_lcg_id(2), 100000}}};
      this->push_bsr(bsr);
    }
  }

  cell_config_builder_params params;

  pucch_res_builder_test_helper pucch_cfg_builder;

  std::vector<ue_stats> ue_stats_map;
};

TEST_F(scheduler_qos_test, when_ue_has_gbr_drb_it_gets_higher_priority)
{
  const unsigned MAX_NOF_SLOT_RUNS = 1000;

  for (unsigned i = 0; i != MAX_NOF_SLOT_RUNS; ++i) {
    this->run_slot();

    uci_indication uci_ind;
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.slot_rx    = this->last_result_slot();
    ul_crc_indication crc_ind;
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = last_result_slot();

    // Handle DL result.
    span<const dl_msg_alloc> ue_grants = this->last_sched_res_list[0]->dl.ue_grants;
    for (const dl_msg_alloc& grant : ue_grants) {
      ue_stats_map[grant.context.ue_index].dl_bytes_sum += grant.pdsch_cfg.codewords[0].tb_size_bytes;
    }

    // Handle PUCCHs.
    for (const pucch_info& pucch : this->last_sched_res_list[to_du_cell_index(0)]->ul.pucchs) {
      if (pucch.format == pucch_format::FORMAT_1 and pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
        // Skip SRs for this test.
        continue;
      }

      du_ue_index_t ue_idx = to_du_ue_index((unsigned)pucch.crnti - 0x4601);
      uci_ind.ucis.push_back(test_helper::create_uci_indication_pdu(ue_idx, pucch));
    }

    // Handle UL result.
    span<const ul_sched_info> ul_grants = this->last_sched_res_list[0]->ul.puschs;
    for (const ul_sched_info& grant : ul_grants) {
      ue_stats_map[grant.context.ue_index].ul_bytes_sum += grant.pusch_cfg.tb_size_bytes;
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
  }

  double nof_sec_elapsed = (MAX_NOF_SLOT_RUNS / (double)get_nof_slots_per_subframe(params.scs_common)) / 1000;
  std::vector<double> ue_dl_rate_mbps(ue_stats_map.size(), 0);
  std::vector<double> ue_ul_rate_mbps(ue_stats_map.size(), 0);
  for (unsigned i = 0; i != ue_stats_map.size(); ++i) {
    ue_dl_rate_mbps[i] = ue_stats_map[i].dl_bytes_sum * 8U * 1e-6 / nof_sec_elapsed;
    ue_ul_rate_mbps[i] = ue_stats_map[i].ul_bytes_sum * 8U * 1e-6 / nof_sec_elapsed;
  }

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
}
