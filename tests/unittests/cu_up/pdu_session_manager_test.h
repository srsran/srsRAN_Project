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

#pragma once

#include "cu_up_test_helpers.h"
#include "lib/cu_up/pdu_session_manager_impl.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;

/// Fixture class for UE manager tests
class pdu_session_manager_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create required objects
    security_info    = get_dummy_up_security_info();
    gtpu_rx_demux    = std::make_unique<dummy_gtpu_demux_ctrl>();
    gtpu_tx_notifier = std::make_unique<dummy_gtpu_network_gateway_adapter>();
    f1u_gw           = std::make_unique<dummy_f1u_gateway>(f1u_bearer);
    f1u_allocator    = std::make_unique<dummy_gtpu_teid_pool>();

    // create DUT object
    ue_inactivity_timer = timers_factory.create_timer();
    ue_inactivity_timer.set(std::chrono::milliseconds(10000), [](timer_id_t) {});
    std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos;
    qos[uint_to_five_qi(9)] = {};

    manual_task_worker teid_worker{128};

    pdu_session_mng = std::make_unique<pdu_session_manager_impl>(MIN_UE_INDEX,
                                                                 qos,
                                                                 security_info,
                                                                 net_config,
                                                                 n3_config,
                                                                 logger,
                                                                 ue_inactivity_timer,
                                                                 timers_factory,
                                                                 timers_factory,
                                                                 timers_factory,
                                                                 *f1u_gw,
                                                                 *f1u_allocator,
                                                                 *gtpu_tx_notifier,
                                                                 *gtpu_rx_demux,
                                                                 teid_worker,
                                                                 teid_worker,
                                                                 teid_worker,
                                                                 gtpu_pcap);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  timer_manager                                        timers_manager;
  manual_task_worker                                   worker{64};
  timer_factory                                        timers_factory{timers_manager, worker};
  unique_timer                                         ue_inactivity_timer;
  std::unique_ptr<dummy_gtpu_demux_ctrl>               gtpu_rx_demux;
  std::unique_ptr<gtpu_tunnel_tx_upper_layer_notifier> gtpu_tx_notifier;
  dummy_inner_f1u_bearer                               f1u_bearer;
  std::unique_ptr<dummy_f1u_gateway>                   f1u_gw;
  std::unique_ptr<dummy_gtpu_teid_pool>                f1u_allocator;
  std::unique_ptr<pdu_session_manager_ctrl>            pdu_session_mng;
  null_dlt_pcap                                        gtpu_pcap;
  security::sec_as_config                              security_info;
  network_interface_config                             net_config;
  n3_interface_config                                  n3_config = {};
  cu_up_ue_logger                                      logger{"CU-UP", {MIN_UE_INDEX}};
};

inline e1ap_pdu_session_res_to_setup_item
generate_pdu_session_res_to_setup_item(pdu_session_id_t psi, drb_id_t drb_id, qos_flow_id_t qfi, five_qi_t five_qi)
{
  // prepare request
  e1ap_pdu_session_res_to_setup_item pdu_session_setup_item;
  pdu_session_setup_item.pdu_session_id                        = psi;
  pdu_session_setup_item.pdu_session_type                      = "ipv4";
  pdu_session_setup_item.snssai.sst                            = 1;
  pdu_session_setup_item.snssai.sd                             = 10203;
  pdu_session_setup_item.security_ind.integrity_protection_ind = integrity_protection_indication_t::not_needed;
  pdu_session_setup_item.security_ind.confidentiality_protection_ind =
      confidentiality_protection_indication_t::not_needed;
  pdu_session_setup_item.pdu_session_res_dl_ambr = 330000000;
  pdu_session_setup_item.ng_ul_up_tnl_info.tp_address =
      transport_layer_address::create_from_bitstring("01111111000000000000000000000001");
  pdu_session_setup_item.ng_ul_up_tnl_info.gtp_teid = int_to_gtpu_teid(0x12345678);
  pdu_session_setup_item.ng_ul_up_tnl_info          = {transport_layer_address::create_from_string("0.0.0.0"),
                                                       int_to_gtpu_teid(0)};

  e1ap_drb_to_setup_item_ng_ran drb_to_setup_item;
  drb_to_setup_item.drb_id                      = drb_id;
  drb_to_setup_item.sdap_cfg.default_drb        = true;
  drb_to_setup_item.sdap_cfg.sdap_hdr_ul        = sdap_hdr_ul_cfg::present;
  drb_to_setup_item.sdap_cfg.sdap_hdr_dl        = sdap_hdr_dl_cfg::present;
  drb_to_setup_item.pdcp_cfg.pdcp_sn_size_ul    = pdcp_sn_size::size18bits;
  drb_to_setup_item.pdcp_cfg.pdcp_sn_size_dl    = pdcp_sn_size::size18bits;
  drb_to_setup_item.pdcp_cfg.rlc_mod            = pdcp_rlc_mode::am;
  drb_to_setup_item.pdcp_cfg.t_reordering_timer = pdcp_t_reordering::ms100;
  drb_to_setup_item.pdcp_cfg.discard_timer      = pdcp_discard_timer::infinity;

  e1ap_cell_group_info_item cell_group_info_item;
  cell_group_info_item.cell_group_id = 0;
  drb_to_setup_item.cell_group_info.push_back(cell_group_info_item);

  e1ap_qos_flow_qos_param_item qos_flow_info;
  qos_flow_info.qos_flow_id = qfi;
  non_dyn_5qi_descriptor_t non_dyn_5qi;
  non_dyn_5qi.five_qi                                                                 = five_qi;
  qos_flow_info.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi             = non_dyn_5qi;
  qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level      = 1;
  qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap = "shall-not-trigger-pre-emption";
  qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability = "not-pre-emptable";
  drb_to_setup_item.qos_flow_info_to_be_setup.emplace(qos_flow_info.qos_flow_id, qos_flow_info);

  pdu_session_setup_item.drb_to_setup_list_ng_ran.emplace(drb_to_setup_item.drb_id, drb_to_setup_item);

  return pdu_session_setup_item;
}

inline e1ap_pdu_session_res_to_modify_item generate_pdu_session_res_to_modify_item_to_remove_drb(pdu_session_id_t psi,
                                                                                                 drb_id_t drb_id)
{
  // prepare modification request (to remove bearers)
  e1ap_pdu_session_res_to_modify_item pdu_session_modify_item;
  pdu_session_modify_item.pdu_session_id = psi;

  pdu_session_modify_item.drb_to_rem_list_ng_ran.push_back(drb_id);

  return pdu_session_modify_item;
}

inline e1ap_pdu_session_res_to_modify_item generate_pdu_session_res_to_modify_item_to_modify_drb(pdu_session_id_t psi,
                                                                                                 drb_id_t drb_id)
{
  // prepare modification request (to remove bearers)
  e1ap_pdu_session_res_to_modify_item pdu_session_modify_item;
  pdu_session_modify_item.pdu_session_id = psi;

  e1ap_drb_to_modify_item_ng_ran drb_to_mod = {};
  drb_to_mod.drb_id                         = drb_id;
  pdu_session_modify_item.drb_to_modify_list_ng_ran.insert(drb_to_mod.drb_id, drb_to_mod);
  return pdu_session_modify_item;
}

inline e1ap_pdu_session_res_to_modify_item
generate_pdu_session_res_to_modify_item_to_setup_drb(pdu_session_id_t           psi,
                                                     drb_id_t                   drb_id,
                                                     std::vector<qos_flow_id_t> qfi_list,
                                                     five_qi_t                  five_qi)
{
  // prepare modification request (to add further bearers)
  e1ap_pdu_session_res_to_modify_item pdu_session_modify_item;
  pdu_session_modify_item.pdu_session_id = psi;

  e1ap_drb_to_setup_item_ng_ran drb_to_setup_item;
  drb_to_setup_item.drb_id                      = drb_id;
  drb_to_setup_item.sdap_cfg.default_drb        = true;
  drb_to_setup_item.sdap_cfg.sdap_hdr_ul        = sdap_hdr_ul_cfg::present;
  drb_to_setup_item.sdap_cfg.sdap_hdr_dl        = sdap_hdr_dl_cfg::present;
  drb_to_setup_item.pdcp_cfg.pdcp_sn_size_ul    = pdcp_sn_size::size18bits;
  drb_to_setup_item.pdcp_cfg.pdcp_sn_size_dl    = pdcp_sn_size::size18bits;
  drb_to_setup_item.pdcp_cfg.rlc_mod            = pdcp_rlc_mode::am;
  drb_to_setup_item.pdcp_cfg.t_reordering_timer = pdcp_t_reordering::ms100;
  drb_to_setup_item.pdcp_cfg.discard_timer      = pdcp_discard_timer::infinity;

  e1ap_cell_group_info_item cell_group_info_item;
  cell_group_info_item.cell_group_id = 0;
  drb_to_setup_item.cell_group_info.push_back(cell_group_info_item);

  for (const auto& qfi : qfi_list) {
    e1ap_qos_flow_qos_param_item qos_flow_info;
    qos_flow_info.qos_flow_id = qfi;
    non_dyn_5qi_descriptor_t non_dyn_5qi;
    non_dyn_5qi.five_qi                                                            = five_qi;
    qos_flow_info.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi        = non_dyn_5qi;
    qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level = 1;
    qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
        "shall-not-trigger-pre-emption";
    qos_flow_info.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability = "not-pre-emptable";
    drb_to_setup_item.qos_flow_info_to_be_setup.emplace(qos_flow_info.qos_flow_id, qos_flow_info);
  }

  pdu_session_modify_item.drb_to_setup_list_ng_ran.emplace(drb_to_setup_item.drb_id, drb_to_setup_item);

  return pdu_session_modify_item;
}
