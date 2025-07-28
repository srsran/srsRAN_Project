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

#include "f1ap_du_test_helpers.h"
#include "test_doubles/f1ap/f1ap_test_message_validators.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class f1ap_du_ue_context_modification_test : public f1ap_du_test
{
protected:
  f1ap_du_ue_context_modification_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    run_f1ap_ue_create(test_ue_index);
    f1ap_message msg = test_helpers::generate_ue_context_setup_request(
        gnb_cu_ue_f1ap_id_t{0}, gnb_du_ue_f1ap_id_t{0}, 1, {}, config_helpers::make_default_du_cell_config().nr_cgi);
    run_ue_context_setup_procedure(test_ue_index, msg);

    this->f1c_gw.clear_tx_pdus();
  }

  void start_procedure(const std::initializer_list<drb_id_t>& drbs,
                       byte_buffer                            rrc_container        = {},
                       bool                                   rrc_delivery_request = false)
  {
    this->f1c_gw.clear_tx_pdus();

    // Prepare DU manager response to F1AP.
    this->f1ap_du_cfg_handler.next_ue_context_update_response.result = true;
    for (drb_id_t drb_id : drbs) {
      this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup.emplace_back();
      auto& drb  = this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup.back();
      drb.drb_id = drb_id;
      drb.dluptnl_info_list.resize(1);
      drb.dluptnl_info_list[0].gtp_teid   = int_to_gtpu_teid(test_rgen::uniform_int<uint32_t>());
      drb.dluptnl_info_list[0].tp_address = transport_layer_address::create_from_string("127.0.0.1");
    }
    this->f1ap_du_cfg_handler.next_ue_context_update_response.cell_group_cfg =
        byte_buffer::create({0x1, 0x2, 0x3}).value();

    // Initiate procedure in F1AP.
    f1ap_message msg = test_helpers::generate_ue_context_modification_request(
        int_to_gnb_du_ue_f1ap_id(0), int_to_gnb_cu_ue_f1ap_id(0), drbs, {}, {}, std::move(rrc_container));
    if (rrc_delivery_request) {
      msg.pdu.init_msg().value.ue_context_mod_request()->rrc_delivery_status_request_present = true;
      msg.pdu.init_msg().value.ue_context_mod_request()->rrc_delivery_status_request.value =
          asn1::f1ap::rrc_delivery_status_request_opts::true_value;
    }
    f1ap->handle_message(msg);

    last_ue_ctxt_mod_req = msg;
  }

  void rrc_container_was_delivered(uint32_t pdcp_sn)
  {
    ue_test_context& ue = test_ues[test_ue_index];
    ue.f1c_bearers[LCID_SRB1].bearer->handle_transmit_notification(pdcp_sn);
    ue.f1c_bearers[LCID_SRB1].bearer->handle_delivery_notification(pdcp_sn);
    ctrl_worker.run_pending_tasks();
  }

  bool is_ue_context_modification_response_valid(const f1ap_message& resp)
  {
    return test_helpers::is_valid_ue_context_modification_response(resp, last_ue_ctxt_mod_req);
  }

  du_ue_index_t test_ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));

  f1ap_message last_ue_ctxt_mod_req;
};

TEST_F(f1ap_du_ue_context_modification_test, when_f1ap_receives_request_then_f1ap_notifies_du_of_ue_context_update)
{
  start_procedure({drb_id_t::drb1});

  // DU manager receives UE Context Update Request.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ue_context_update_req.has_value());
  const f1ap_ue_context_update_request& req = *this->f1ap_du_cfg_handler.last_ue_context_update_req;
  ASSERT_EQ(req.ue_index, test_ue_index);
  ASSERT_EQ(req.srbs_to_setup.size(), 0);
  ASSERT_EQ(req.drbs_to_setup.size(), 1);
  ASSERT_EQ(req.drbs_to_setup[0].drb_id, drb_id_t::drb1);
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_ue_context_mod_req_contains_rrc_container_then_rrc_container_is_sent_to_lower_layers)
{
  byte_buffer dl_dcch_msg   = byte_buffer::create(test_rgen::random_vector<uint8_t>(100)).value();
  byte_buffer rrc_container = test_helpers::create_dl_dcch_rrc_container(2, dl_dcch_msg);
  start_procedure({drb_id_t::drb1}, rrc_container.copy());

  // Check if RRC container is sent to lower layers.
  ASSERT_EQ(this->test_ues[test_ue_index].f1c_bearers[1].rx_sdu_notifier.last_pdu, rrc_container);
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_ue_context_mod_has_rrc_container_then_f1ap_waits_for_delivery_notification)
{
  uint32_t pdcp_sn = 2;
  start_procedure({drb_id_t::drb1}, test_helpers::create_dl_dcch_rrc_container(pdcp_sn, {0x1, 0x2, 0x3}));

  ASSERT_FALSE(this->f1c_gw.pop_tx_pdu().has_value());
  rrc_container_was_delivered(pdcp_sn);
  auto sent_pdu = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(sent_pdu.has_value());
  ASSERT_TRUE(is_ue_context_modification_response_valid(sent_pdu.value()));
  ASSERT_FALSE(this->f1c_gw.pop_tx_pdu().has_value()) << "Only UEContextModificationResponse should have been sent";
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_f1ap_receives_request_then_f1ap_responds_to_cu_with_ue_context_modification_response)
{
  uint32_t pdcp_sn = 2;
  start_procedure({drb_id_t::drb1}, test_helpers::create_dl_dcch_rrc_container(pdcp_sn, {0x1, 0x2, 0x3}));
  rrc_container_was_delivered(pdcp_sn);

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  auto sent_pdu = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(is_ue_context_modification_response_valid(sent_pdu.value()));
  const ue_context_mod_resp_s& resp = sent_pdu.value().pdu.successful_outcome().value.ue_context_mod_resp();
  ASSERT_FALSE(resp->srbs_failed_to_be_setup_mod_list_present);
  ASSERT_FALSE(resp->srbs_modified_list_present);
  ASSERT_FALSE(resp->srbs_setup_mod_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_modified_list_present);
  ASSERT_FALSE(resp->drbs_modified_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_setup_mod_list_present);
  ASSERT_TRUE(resp->drbs_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_setup_mod_list.size(), 1);
  auto& drb_setup = resp->drbs_setup_mod_list[0].value().drbs_setup_mod_item();
  ASSERT_EQ(drb_setup.drb_id, 1);
  ASSERT_EQ(drb_setup.dl_up_tnl_info_to_be_setup_list.size(), 1);
  ASSERT_EQ(
      int_to_gtpu_teid(drb_setup.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.gtp_tunnel().gtp_teid.to_number()),
      this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup[0].dluptnl_info_list[0].gtp_teid);
  ASSERT_EQ(
      drb_setup.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.gtp_tunnel().transport_layer_address.to_string(),
      this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup[0]
          .dluptnl_info_list[0]
          .tp_address.to_bitstring());
  ASSERT_EQ(resp->du_to_cu_rrc_info.cell_group_cfg,
            this->f1ap_du_cfg_handler.next_ue_context_update_response.cell_group_cfg);
}

TEST_F(
    f1ap_du_ue_context_modification_test,
    when_ue_context_mod_has_rrc_container_and_rrc_delivery_status_request_then_f1ap_waits_for_delivery_notification_before_sending_rrc_delivery_request)
{
  uint32_t pdcp_sn = 2;
  start_procedure({drb_id_t::drb1}, test_helpers::create_dl_dcch_rrc_container(pdcp_sn, {0x1, 0x2, 0x3}), true);

  ASSERT_FALSE(this->f1c_gw.pop_tx_pdu().has_value());
  rrc_container_was_delivered(pdcp_sn);
  auto sent_pdu = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(is_ue_context_modification_response_valid(sent_pdu.value()));

  // Pop RRC Delivery Status Report (after UE context modification response).
  auto msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_EQ(msg.value().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::rrc_delivery_report);
  ASSERT_EQ(msg.value().pdu.init_msg().value.rrc_delivery_report()->srb_id, 1);
  ASSERT_EQ(msg.value().pdu.init_msg().value.rrc_delivery_report()->rrc_delivery_status.trigger_msg, pdcp_sn);
  msg = this->f1c_gw.pop_tx_pdu();
  ASSERT_FALSE(msg.has_value()) << "No more messages should have been sent";
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_du_fails_to_create_drb_then_f1ap_adds_the_failed_drb_in_ue_context_modification_response)
{
  // Prepare DU manager response to F1AP with failed DRB.
  this->f1ap_du_cfg_handler.next_ue_context_update_response.result = true;
  this->f1ap_du_cfg_handler.next_ue_context_update_response.failed_drbs_setups.push_back(
      {drb_id_t::drb1, f1ap_cause_radio_network_t::no_radio_res_available});
  this->f1ap_du_cfg_handler.next_ue_context_update_response.cell_group_cfg =
      byte_buffer::create({0x1, 0x2, 0x3}).value();

  // Initiate procedure in F1AP.
  f1ap_message msg = test_helpers::generate_ue_context_modification_request(
      int_to_gnb_du_ue_f1ap_id(0), int_to_gnb_cu_ue_f1ap_id(0), {drb_id_t::drb1}, {}, {});
  f1ap->handle_message(msg);
  this->last_ue_ctxt_mod_req = msg;

  // F1AP sends UE CONTEXT MODIFICATION RESPONSE to CU-CP with failed DRB.
  auto ret = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(is_ue_context_modification_response_valid(ret.value()));
  const ue_context_mod_resp_s& resp = ret.value().pdu.successful_outcome().value.ue_context_mod_resp();
  ASSERT_FALSE(resp->srbs_failed_to_be_setup_mod_list_present);
  ASSERT_FALSE(resp->srbs_modified_list_present);
  ASSERT_FALSE(resp->srbs_setup_mod_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_modified_list_present);
  ASSERT_FALSE(resp->drbs_modified_list_present);
  ASSERT_TRUE(resp->drbs_failed_to_be_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_failed_to_be_setup_mod_list.size(), 1);
  ASSERT_EQ(resp->drbs_failed_to_be_setup_mod_list[0].value().drbs_failed_to_be_setup_mod_item().drb_id, 1);
  ASSERT_FALSE(resp->drbs_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_setup_mod_list.size(), 0);
  ASSERT_EQ(resp->du_to_cu_rrc_info.cell_group_cfg,
            this->f1ap_du_cfg_handler.next_ue_context_update_response.cell_group_cfg);
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_multiple_drbs_are_to_be_setup_then_response_contains_the_drbs_in_the_drbs_setup_list)
{
  start_procedure({drb_id_t::drb1, drb_id_t::drb2});

  // F1AP sends UE CONTEXT MODIFICATION RESPONSE to CU-CP.
  auto ret = this->f1c_gw.pop_tx_pdu();
  ASSERT_TRUE(ret.has_value());
  ASSERT_TRUE(is_ue_context_modification_response_valid(ret.value()));
  const ue_context_mod_resp_s& resp = ret.value().pdu.successful_outcome().value.ue_context_mod_resp();
  ASSERT_FALSE(resp->srbs_setup_mod_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_modified_list_present);
  ASSERT_FALSE(resp->drbs_modified_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_setup_mod_list_present);
  ASSERT_TRUE(resp->drbs_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_setup_mod_list.size(), 2);
  auto& drb1_setup = resp->drbs_setup_mod_list[0].value().drbs_setup_mod_item();
  ASSERT_EQ(drb1_setup.drb_id, 1);
  ASSERT_EQ(drb1_setup.dl_up_tnl_info_to_be_setup_list.size(), 1);
  auto& drb2_setup = resp->drbs_setup_mod_list[1].value().drbs_setup_mod_item();
  ASSERT_EQ(drb2_setup.drb_id, 2);
  ASSERT_EQ(drb2_setup.dl_up_tnl_info_to_be_setup_list.size(), 1);
}
