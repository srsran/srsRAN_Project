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

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

class f1ap_cu_ue_context_modification_test : public f1ap_cu_test
{
protected:
  f1ap_cu_ue_context_modification_test() {}

  /// \brief Generates dummy common type CU-CP UE CONTEXT MODIFICATION REQUEST message.
  static f1ap_ue_context_modification_request generate_ue_context_modification_request(ue_index_t ue_index)
  {
    f1ap_ue_context_modification_request msg;

    msg.ue_index = ue_index;

    // sp cell id
    nr_cell_global_id_t sp_cell_id;
    sp_cell_id.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    sp_cell_id.plmn_id = plmn_identity::test_value();
    msg.sp_cell_id     = sp_cell_id;

    // serv cell idx
    msg.serv_cell_idx = 1;

    // sp cell ul cfg
    msg.sp_cell_ul_cfg = f1ap_cell_ul_cfg::ul;

    // drx cycle
    f1ap_drx_cycle drx_cycle;
    drx_cycle.long_drx_cycle_len    = 5;
    drx_cycle.short_drx_cycle_len   = 1;
    drx_cycle.short_drx_cycle_timer = 10;
    msg.drx_cycle                   = drx_cycle;

    // cu to du to rrc info
    f1ap_cu_to_du_rrc_info cu_to_du_rrc_info;
    cu_to_du_rrc_info.cg_cfg_info               = make_byte_buffer("deadbeef").value();
    cu_to_du_rrc_info.ue_cap_rat_container_list = make_byte_buffer("deadbeef").value();
    cu_to_du_rrc_info.meas_cfg                  = make_byte_buffer("deadbeef").value();
    msg.cu_to_du_rrc_info                       = cu_to_du_rrc_info;

    // tx action ind
    msg.tx_action_ind = f1ap_tx_action_ind::stop;

    // res coordination transfer container
    msg.res_coordination_transfer_container = make_byte_buffer("deadbeef").value();

    // rrc recfg complete ind
    msg.rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind::true_value;

    // rrc container
    msg.rrc_container = make_byte_buffer("deadbeef").value();

    // scell to be setup mod list
    f1ap_scell_to_be_setup_mod_item scell_to_be_setup_mod_item;
    scell_to_be_setup_mod_item.scell_id.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    scell_to_be_setup_mod_item.scell_id.plmn_id = plmn_identity::test_value();
    scell_to_be_setup_mod_item.scell_idx        = 1;
    scell_to_be_setup_mod_item.scell_ul_cfg     = f1ap_cell_ul_cfg::ul;
    msg.scell_to_be_setup_mod_list.push_back(scell_to_be_setup_mod_item);

    // scell to be remd list
    f1ap_scell_to_be_remd_item scell_to_be_remd_item;
    scell_to_be_remd_item.scell_id.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    scell_to_be_remd_item.scell_id.plmn_id = plmn_identity::test_value();
    msg.scell_to_be_remd_list.push_back(scell_to_be_remd_item);

    // srbs to be setup mod list
    f1ap_srb_to_setup srbs_to_be_setup_mod_item;
    srbs_to_be_setup_mod_item.srb_id = int_to_srb_id(1);
    msg.srbs_to_be_setup_mod_list.push_back(srbs_to_be_setup_mod_item);

    // drbs to be setup mod list
    f1ap_drb_to_setup drbs_to_be_setup_mod_item;
    drbs_to_be_setup_mod_item.drb_id = uint_to_drb_id(1);
    // qos info

    // drb qos
    // qos flow level qos params
    // qos characteristics
    non_dyn_5qi_descriptor non_dyn_5qi;
    non_dyn_5qi.five_qi                                 = uint_to_five_qi(8);
    non_dyn_5qi.qos_prio_level                          = qos_prio_level_t{1};
    non_dyn_5qi.averaging_win                           = 3;
    non_dyn_5qi.max_data_burst_volume                   = 1000;
    drbs_to_be_setup_mod_item.qos_info.drb_qos.qos_desc = non_dyn_5qi;

    // ng ran alloc retention prio
    drbs_to_be_setup_mod_item.qos_info.drb_qos.alloc_retention_prio.prio_level_arp         = 1;
    drbs_to_be_setup_mod_item.qos_info.drb_qos.alloc_retention_prio.may_trigger_preemption = false;
    drbs_to_be_setup_mod_item.qos_info.drb_qos.alloc_retention_prio.is_preemptable         = false;

    // gbr qos flow info
    auto& gbr_qos_info                   = drbs_to_be_setup_mod_item.qos_info.drb_qos.gbr_qos_info.emplace();
    gbr_qos_info.max_br_dl               = 100000;
    gbr_qos_info.max_br_ul               = 100000;
    gbr_qos_info.gbr_dl                  = 100000;
    gbr_qos_info.gbr_ul                  = 100000;
    gbr_qos_info.max_packet_loss_rate_dl = 30;
    gbr_qos_info.max_packet_loss_rate_ul = 30;

    // reflective qos attribute
    drbs_to_be_setup_mod_item.qos_info.drb_qos.reflective_qos_attribute_subject_to = true;

    // s nssai
    drbs_to_be_setup_mod_item.qos_info.s_nssai.sst = slice_service_type{1};
    drbs_to_be_setup_mod_item.qos_info.s_nssai.sd  = slice_differentiator::create(128).value();

    // notif ctrl
    drbs_to_be_setup_mod_item.qos_info.notif_ctrl = drb_notification_control::active;

    // flows mapped to drb list
    flow_mapped_to_drb flows_mapped_to_drb_item;
    flows_mapped_to_drb_item.qos_flow_id = uint_to_qos_flow_id(1);
    // qos characteristics
    flows_mapped_to_drb_item.qos_flow_level_qos_params.qos_desc = non_dyn_5qi;
    // ng ran alloc retention prio
    flows_mapped_to_drb_item.qos_flow_level_qos_params.alloc_retention_prio.prio_level_arp         = 1;
    flows_mapped_to_drb_item.qos_flow_level_qos_params.alloc_retention_prio.may_trigger_preemption = false;
    flows_mapped_to_drb_item.qos_flow_level_qos_params.alloc_retention_prio.is_preemptable         = false;
    // gbr qos flow info
    flows_mapped_to_drb_item.qos_flow_level_qos_params.gbr_qos_info = gbr_qos_info;
    // reflective qos attribute
    flows_mapped_to_drb_item.qos_flow_level_qos_params.reflective_qos_attribute_subject_to = true;

    drbs_to_be_setup_mod_item.qos_info.flows_mapped_to_drb_list.push_back(flows_mapped_to_drb_item);

    // ul up tnl info to be setup list
    up_transport_layer_info ul_up_tnl_info_item = {transport_layer_address::create_from_string("127.0.0.1"),
                                                   int_to_gtpu_teid(1)};
    drbs_to_be_setup_mod_item.uluptnl_info_list.push_back(ul_up_tnl_info_item);

    // rlc mode
    drbs_to_be_setup_mod_item.mode        = rlc_mode::am;
    drbs_to_be_setup_mod_item.pdcp_sn_len = pdcp_sn_size::size12bits;

    msg.drbs_to_be_setup_mod_list.push_back(drbs_to_be_setup_mod_item);

    // drbs to be modified list
    f1ap_drb_to_modify drbs_to_be_modified_item;
    drbs_to_be_modified_item.drb_id = uint_to_drb_id(1);

    // ul up tnl info to be setup list
    drbs_to_be_modified_item.uluptnl_info_list.push_back(ul_up_tnl_info_item);

    msg.drbs_to_be_modified_list.push_back(drbs_to_be_modified_item);

    // srbs to be released list
    msg.srbs_to_be_released_list.push_back(int_to_srb_id(1));

    // drbs to be released list
    msg.drbs_to_be_released_list.push_back(uint_to_drb_id(1));

    // inactivity monitoring request
    msg.inactivity_monitoring_request = true;

    // rat freq prio info
    f1ap_rat_freq_prio_info rat_freq_prio_info;
    rat_freq_prio_info.ngran.emplace();
    rat_freq_prio_info.ngran = 1;
    msg.rat_freq_prio_info   = rat_freq_prio_info;

    // drx cfg ind
    msg.drx_cfg_ind = true;

    // rlc fail ind
    f1ap_rlc_fail_ind rlc_fail_ind;
    rlc_fail_ind.assocated_lcid = lcid_t::LCID_SRB1;
    msg.rlc_fail_ind            = rlc_fail_ind;

    // ul tx direct current list info
    msg.ul_tx_direct_current_list_info = make_byte_buffer("deadbeef").value();

    // gnb du cfg query
    msg.gnb_du_cfg_query = true;

    // gnb du ue ambr ul
    msg.gnb_du_ue_ambr_ul = 200000000;

    // execute dupl
    msg.execute_dupl = true;

    // rrc delivery status request
    msg.rrc_delivery_status_request = true;

    // res coordination transfer info
    f1ap_res_coordination_transfer_info res_coordination_transfer_info;
    res_coordination_transfer_info.m_enb_cell_id = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    msg.res_coordination_transfer_info           = res_coordination_transfer_info;

    // serving cell mo
    msg.serving_cell_mo = 1;

    // need for gap
    msg.need_for_gap = true;

    // full cfg
    msg.full_cfg = true;

    return msg;
  }

  void start_procedure(const f1ap_ue_context_modification_request& req)
  {
    t = f1ap->handle_ue_context_modification_request(req);
    t_launcher.emplace(t);
  }

  bool was_ue_context_modification_msg_sent(gnb_du_ue_f1ap_id_t du_ue_id) const
  {
    if (this->f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != f1ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value !=
        asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request) {
      return false;
    }
    auto& req = this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.ue_context_mod_request();

    return req->gnb_du_ue_f1ap_id == (unsigned)du_ue_id;
  }

  async_task<f1ap_ue_context_modification_response>                        t;
  std::optional<lazy_task_launcher<f1ap_ue_context_modification_response>> t_launcher;
};

TEST_F(f1ap_cu_ue_context_modification_test, when_f1ap_receives_response_then_procedure_completes)
{
  // Preamble.
  test_ue& ue = run_ue_context_setup();

  // Start UE CONTEXT MODIFICATION procedure and return back the response from the DU.
  this->start_procedure(generate_ue_context_modification_request(ue.ue_index));
  f1ap_message ue_context_modification_response = test_helpers::generate_ue_context_modification_response(
      ue.du_ue_id.value(), ue.cu_ue_id.value(), to_rnti(0x4601));
  f1ap->handle_message(ue_context_modification_response);

  // The UE CONTEXT MODIFICATION procedure finished successfully.
  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().drbs_setup_list.size(), 1);
  EXPECT_EQ(t.get().srbs_setup_mod_list.size(), 0);
}

TEST_F(f1ap_cu_ue_context_modification_test,
       when_f1ap_receives_response_with_failed_drb_then_it_forwards_the_failed_drb_outwards)
{
  // Preamble.
  test_ue& ue = run_ue_context_setup();

  // Start UE CONTEXT MODIFICATION procedure and return back the Failure Response from the DU.
  this->start_procedure(generate_ue_context_modification_request(ue.ue_index));
  f1ap_message ue_context_modification_response = test_helpers::generate_ue_context_modification_response(
      ue.du_ue_id.value(), ue.cu_ue_id.value(), to_rnti(0x4601));
  auto& resp = *ue_context_modification_response.pdu.successful_outcome().value.ue_context_mod_resp();
  resp.drbs_failed_to_be_setup_mod_list_present = true;
  resp.drbs_failed_to_be_setup_mod_list.resize(1);
  resp.drbs_failed_to_be_setup_mod_list[0].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_ITEM);
  resp.drbs_failed_to_be_setup_mod_list[0]->drbs_failed_to_be_setup_mod_item().drb_id = 1;
  resp.drbs_setup_mod_list_present                                                    = false;
  resp.drbs_setup_mod_list.clear();
  f1ap->handle_message(ue_context_modification_response);

  // The UE CONTEXT MODIFICATION procedure finished unsuccessfully.
  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().drbs_setup_list.size(), 0);
  EXPECT_EQ(t.get().srbs_setup_mod_list.size(), 0);
  EXPECT_EQ(t.get().drbs_failed_to_be_setup_list.size(), 1);
  EXPECT_EQ(t.get().drbs_failed_to_be_setup_list[0].drb_id, drb_id_t::drb1);
}

TEST_F(f1ap_cu_ue_context_modification_test, when_ue_modification_failure_received_then_procedure_is_unsuccessful)
{
  // Preamble.
  test_ue& ue = run_ue_context_setup();

  // Start UE CONTEXT MODIFICATION procedure and return back the Failure Response from the DU.
  this->start_procedure(generate_ue_context_modification_request(ue.ue_index));
  f1ap_message ue_context_modification_failure =
      test_helpers::generate_ue_context_modification_failure(ue.cu_ue_id.value(), ue.du_ue_id.value());
  f1ap->handle_message(ue_context_modification_failure);

  // The UE CONTEXT MODIFICATION procedure finished unsuccessfully.
  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
  EXPECT_EQ(t.get().drbs_setup_list.size(), 0);
  EXPECT_EQ(t.get().srbs_setup_mod_list.size(), 0);
}
