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

#include "lib/scheduler/support/sch_pdu_builder.h"
#include "lib/scheduler/ue_context/ue_cell.h"
#include "lib/scheduler/ue_context/ue_drx_controller.h"
#include "lib/scheduler/ue_context/ul_logical_channel_manager.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test suit to hold context which are required to test a UE cell implementation.
class ue_cell_tester : public ::testing::Test
{
protected:
  ue_cell_tester() :
    expert_cfg(config_helpers::make_default_scheduler_expert_config()),
    sched_cfg(sched_config_helper::make_default_sched_cell_configuration_request()),
    serv_cell_cfg(config_helpers::create_default_initial_ue_serving_cell_config()),
    ue_cc_cfg(to_rnti(0x4601), cell_cfg, cfg_pool.update_ue(serv_cell_cfg)),
    ul_lc_ch_mng(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, {})
  {
  }

  pdsch_config_params get_pdsch_cfg_params(const ue_cell&                               ue_cc,
                                           const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                           dci_dl_rnti_config_type                      dci_type)
  {
    switch (dci_type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        return sched_helper::get_pdsch_config_f1_0_c_rnti(cell_cfg, ue_cc.cfg().pdsch_serving_cell_cfg(), pdsch_td_cfg);
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        return sched_helper::get_pdsch_config_f1_1_c_rnti(
            cell_cfg,
            ue_cc.cfg().bwp(to_bwp_id(0)).dl_ded.value()->pdsch_cfg.value(),
            ue_cc.cfg().pdsch_serving_cell_cfg(),
            pdsch_td_cfg,
            ue_cc.channel_state_manager().get_nof_dl_layers());
      default:
        report_fatal_error("Unsupported PDCCH DCI DL format");
    }
  }

  pusch_config_params get_pusch_cfg_params(const ue_cell&                               ue_cc,
                                           const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                           dci_ul_rnti_config_type                      dci_type)
  {
    // In the following, we allocate extra bits to account for the possible UCI overhead. At this point, we don't
    // differentiate between HARQ-ACK bits and CSI bits, which would be necessary to compute the beta-offset values.
    // Here, we only need to allocate some extra space.
    const unsigned uci_bits_overallocation = 20U;
    const bool     is_csi_report_slot      = false;

    switch (dci_type) {
      case dci_ul_rnti_config_type::c_rnti_f0_0:
        return get_pusch_config_f0_0_c_rnti(cell_cfg,
                                            &ue_cc.cfg(),
                                            *ue_cc.cfg().bwp(ue_cc.active_bwp_id()).ul_common.value(),
                                            pusch_td_cfg,
                                            uci_bits_overallocation,
                                            is_csi_report_slot);
        break;
      case dci_ul_rnti_config_type::c_rnti_f0_1:
        return get_pusch_config_f0_1_c_rnti(ue_cc.cfg(),
                                            pusch_td_cfg,
                                            ue_cc.channel_state_manager().get_nof_ul_layers(),
                                            uci_bits_overallocation,
                                            is_csi_report_slot);
        break;
      default:
        report_fatal_error("Unsupported PDCCH DCI UL format");
    }
  }

  scheduler_expert_config                  expert_cfg;
  sched_cell_configuration_request_message sched_cfg;
  cell_configuration                       cell_cfg{expert_cfg, sched_cfg};
  serving_cell_config                      serv_cell_cfg;
  du_cell_config_pool                      cfg_pool{sched_cfg};
  ue_cell_configuration                    ue_cc_cfg;
  cell_harq_manager                        cell_harqs{1, MAX_NOF_HARQS};
  ul_logical_channel_manager               ul_lc_ch_mng;
  srslog::basic_logger&                    logger = srslog::fetch_basic_logger("SCHED");
  ue_drx_controller                        drx_controller{cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs,
                                   cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer,
                                   std::nullopt,
                                   ul_lc_ch_mng,
                                                          {},
                                   logger};
};

TEST_F(ue_cell_tester, when_dl_nof_prb_allocated_increases_estimated_dl_rate_increases)
{
  ue_cell ue_cc{to_du_ue_index(0), to_rnti(0x4601), ue_cc_cfg, cell_harqs, drx_controller, std::nullopt};

  double current_rate = 0;

  // We keep MCS constant for this test.
  const sch_mcs_index ue_mcs = 28;
  // Assume the DCI type to be format 1_1.
  const dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::c_rnti_f1_1;

  for (unsigned nof_prbs = 0; nof_prbs < MAX_NOF_PRBS; ++nof_prbs) {
    for (const auto& pdsch_td_cfg :
         ue_cc.cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list) {
      const pdsch_config_params pdsch_cfg      = get_pdsch_cfg_params(ue_cc, pdsch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_dl_rate(pdsch_cfg, ue_mcs, nof_prbs);
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

TEST_F(ue_cell_tester, when_mcs_increases_estimated_dl_rate_increases)
{
  // Maximum MCS value for 64QAM MCS table.
  const sch_mcs_index max_mcs = 28;

  ue_cell ue_cc{to_du_ue_index(0), to_rnti(0x4601), ue_cc_cfg, cell_harqs, drx_controller, std::nullopt};

  double current_rate = 0;

  // We keep nof. PRBs allocated constant for this test.
  const unsigned nof_prbs = 20;
  // Assume the DCI type to be format 1_1.
  const dci_dl_rnti_config_type dci_type = dci_dl_rnti_config_type::c_rnti_f1_1;

  for (sch_mcs_index ue_mcs = 1; ue_mcs < max_mcs; ++ue_mcs) {
    for (const auto& pdsch_td_cfg :
         ue_cc.cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list) {
      const pdsch_config_params pdsch_cfg      = get_pdsch_cfg_params(ue_cc, pdsch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_dl_rate(pdsch_cfg, ue_mcs, nof_prbs);
      // NOTE: In case of 64QAM MCS table MCS 17 has lower spectral density than MCS 16 but the estimated bitrate will
      // remain equal hence its required to use the check greater than or equal below.
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

TEST_F(ue_cell_tester, when_ul_nof_prb_allocated_increases_estimated_ul_rate_increases)
{
  ue_cell ue_cc{to_du_ue_index(0), to_rnti(0x4601), ue_cc_cfg, cell_harqs, drx_controller, std::nullopt};

  double current_rate = 0;

  // We keep MCS constant for this test.
  const sch_mcs_index ue_mcs = 15;
  // Assume the DCI type to be format 0_1.
  const dci_ul_rnti_config_type dci_type = dci_ul_rnti_config_type::c_rnti_f0_1;

  for (unsigned nof_prbs = 0; nof_prbs < MAX_NOF_PRBS; ++nof_prbs) {
    for (const auto& pusch_td_cfg :
         ue_cc.cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
      const pusch_config_params pusch_cfg      = get_pusch_cfg_params(ue_cc, pusch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_ul_rate(pusch_cfg, ue_mcs, nof_prbs);
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}

TEST_F(ue_cell_tester, when_mcs_increases_estimated_ul_rate_increases)
{
  // Maximum MCS value for 64QAM MCS table.
  const sch_mcs_index max_mcs = 28;

  ue_cell ue_cc{to_du_ue_index(0), to_rnti(0x4601), ue_cc_cfg, cell_harqs, drx_controller, std::nullopt};

  double current_rate = 0;

  // We keep nof. PRBs allocated constant for this test.
  const unsigned nof_prbs = 20;
  // Assume the DCI type to be format 0_1.
  const dci_ul_rnti_config_type dci_type = dci_ul_rnti_config_type::c_rnti_f0_1;

  for (sch_mcs_index ue_mcs = 1; ue_mcs < max_mcs; ++ue_mcs) {
    for (const auto& pusch_td_cfg :
         ue_cc.cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list) {
      const pusch_config_params pusch_cfg      = get_pusch_cfg_params(ue_cc, pusch_td_cfg, dci_type);
      const double              estimated_rate = ue_cc.get_estimated_ul_rate(pusch_cfg, ue_mcs, nof_prbs);
      // NOTE: In case of 64QAM MCS table MCS 17 has lower spectral density than MCS 16 but the estimated bitrate will
      // remain equal hence its required to use the check greater than or equal below.
      ASSERT_GE(estimated_rate, current_rate);
      current_rate = estimated_rate;
    }
  }
}
