/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../test_utils/config_generators.h"
#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_harq_link_adaptation_test : public ::testing::Test
{
protected:
  ue_harq_link_adaptation_test() : logger(srslog::fetch_basic_logger("SCHED", true))
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    // Set nof. DL ports to 4.
    cell_config_builder_params params{};
    params.nof_dl_ports = 4;
    const sched_cell_configuration_request_message sched_cell_cfg_req =
        test_helpers::make_default_sched_cell_configuration_request(params);

    cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, sched_cell_cfg_req));
    cell_cfg = cell_cfg_list[to_du_cell_index(0)].get();

    next_slot = test_helpers::generate_random_slot_point(cell_cfg->dl_cfg_common.init_dl_bwp.generic_params.scs);

    // Create UE.
    sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request(params);
    ue_creation_req.ue_index                          = to_du_ue_index(0);
    ue_creation_req.crnti                             = to_rnti(0x4601 + (unsigned)ue_creation_req.ue_index);
    for (const lcid_t lcid : std::array<lcid_t, 3>{uint_to_lcid(1), uint_to_lcid(2), uint_to_lcid(4)}) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    ue_ded_cfg.emplace(ue_creation_req.ue_index, ue_creation_req.crnti, cell_cfg_list, ue_creation_req.cfg);
    ue_ptr = std::make_unique<ue>(
        ue_creation_command{*ue_ded_cfg, ue_creation_req.starts_in_fallback, harq_timeout_handler});
    ue_cc = &ue_ptr->get_cell(to_ue_cell_index(0));
  }

  void run_slot()
  {
    next_slot++;
    ue_ptr->slot_indication(next_slot);
  }

  void handle_harq_newtx(harq_id_t harq_id, unsigned k1 = 4)
  {
    const search_space_info& ss = ue_cc->cfg().search_space(to_search_space_id(2));

    const pdsch_information pdsch{ue_ptr->crnti,
                                  &ss.bwp->dl_common->generic_params,
                                  ss.coreset,
                                  vrb_alloc{vrb_interval{0, 5}},
                                  ss.pdsch_time_domain_list[0].symbols,
                                  {},
                                  {},
                                  ue_cc->cfg().cell_cfg_common.pci,
                                  2,
                                  false,
                                  search_space_set_type::ue_specific,
                                  dci_dl_format::f1_1,
                                  harq_id,
                                  nullopt};

    ue_cc->harqs.dl_harq(harq_id).new_tx(next_slot, k1, 4, 0, 15, 2);
    ue_cc->harqs.dl_harq(harq_id).save_alloc_params(dci_dl_rnti_config_type::c_rnti_f1_1, pdsch);
  }

  const scheduler_expert_config        sched_cfg = config_helpers::make_default_scheduler_expert_config();
  cell_common_configuration_list       cell_cfg_list;
  cell_configuration*                  cell_cfg = nullptr;
  optional<ue_configuration>           ue_ded_cfg;
  scheduler_harq_timeout_dummy_handler harq_timeout_handler;

  srslog::basic_logger& logger;

  std::unique_ptr<ue> ue_ptr;
  ue_cell*            ue_cc = nullptr;

  slot_point next_slot;
};

TEST_F(ue_harq_link_adaptation_test, harq_not_retx_when_cqi_drops_below_threshold)
{
  // Action: UE reports CQI value of 15.
  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{15};
  ue_cc->handle_csi_report(csi);

  static const harq_id_t harq_id = to_harq_id(0);
  handle_harq_newtx(harq_id);

  const dl_harq_process& h = ue_cc->harqs.dl_harq(harq_id);

  while (h.slot_ack() != next_slot) {
    // Run until ACK slot.
    run_slot();
  }

  // Action: UE reports CQI value of 10 (CQI during new Tx was 15).
  csi.first_tb_wideband_cqi = cqi_value{10};
  ue_cc->handle_csi_report(csi);

  // Action: NACK the HARQ.
  ue_cc->harqs.dl_harq(harq_id).ack_info(0, mac_harq_ack_report_status::nack, nullopt);

  // Result: There should not be retx for HARQ.
  ASSERT_EQ(ue_cc->harqs.find_pending_dl_retx(), nullptr) << "HARQ must not be retransmitted due to drop in CQI";
}

TEST_F(ue_harq_link_adaptation_test, harq_not_retx_when_ri_drops_below_threshold)
{
  // Action: UE reports RI value of 2.
  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{15};
  csi.ri                    = 2;
  ue_cc->handle_csi_report(csi);

  static const harq_id_t harq_id = to_harq_id(0);
  handle_harq_newtx(harq_id);

  const dl_harq_process& h = ue_cc->harqs.dl_harq(harq_id);

  while (h.slot_ack() != next_slot) {
    // Run until ACK slot.
    run_slot();
  }

  // Action: UE reports RI value of 1.
  csi.ri = 1;
  ue_cc->handle_csi_report(csi);

  // Action: NACK the HARQ.
  ue_cc->harqs.dl_harq(harq_id).ack_info(0, mac_harq_ack_report_status::nack, nullopt);

  // Result: There should not be retx for HARQ.
  ASSERT_EQ(ue_cc->harqs.find_pending_dl_retx(), nullptr) << "HARQ must not be retransmitted due to drop in RI";
}
