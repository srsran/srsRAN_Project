/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_du_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::f1ap;

TEST_F(f1ap_du_test, when_f1ap_ue_context_setup_request_is_received_f1ap_notifies_du_of_ue_context_update)
{
  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);
  this->f1c_du_cfg_handler.next_ue_context_update_response.result                 = true;
  this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Test Section - Send UE CONTEXT SETUP REQUEST to F1AP.
  f1c_message msg = generate_f1_ue_context_setup_request({drb_id_t::drb1});
  f1ap->handle_message(msg);

  // DU manager receives UE Context Update Request.
  ASSERT_TRUE(this->f1c_du_cfg_handler.last_ue_context_update_req.has_value());
  const f1ap_ue_context_update_request& req = *this->f1c_du_cfg_handler.last_ue_context_update_req;
  ASSERT_EQ(req.ue_index, ue_index);
  ASSERT_EQ(req.srbs_to_setup.size(), 1);
  ASSERT_EQ(req.srbs_to_setup[0], srb_id_t::srb2);
  ASSERT_EQ(req.drbs_to_setup.size(), 1);
  ASSERT_EQ(req.drbs_to_setup[0].drb_id, drb_id_t::drb1);
  ASSERT_FALSE(req.drbs_to_setup[0].lcid.has_value());
}

TEST_F(f1ap_du_test, when_f1ap_ue_context_setup_request_is_received_f1ap_responds_back_with_ue_context_setup_response)
{
  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);
  this->f1c_du_cfg_handler.next_ue_context_update_response.result                 = true;
  this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Test Section - Send UE CONTEXT SETUP REQUEST to F1AP.
  f1c_message msg = generate_f1_ue_context_setup_request({drb_id_t::drb1});
  f1ap->handle_message(msg);

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  ASSERT_EQ(this->msg_notifier.last_f1c_msg.pdu.type().value, f1_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.type().value,
            f1_ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_setup_resp);
  ue_context_setup_resp_s& resp =
      this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.ue_context_setup_resp();
  ASSERT_TRUE(resp->c_rnti_present);
  ASSERT_EQ(resp->c_rnti->value, this->test_ues[ue_index].crnti);
  ASSERT_FALSE(resp->drbs_failed_to_be_setup_list_present);
  ASSERT_TRUE(resp->srbs_setup_list_present);
  ASSERT_EQ(resp->srbs_setup_list->size(), 1);
  ASSERT_EQ(resp->srbs_setup_list.value[0]->srbs_setup_item().srbid, 2);
  ASSERT_TRUE(resp->drbs_setup_list_present);
  ASSERT_EQ(resp->drbs_setup_list.value.size(), 1);
  auto& drb_setup = resp->drbs_setup_list.value[0].value().drbs_setup_item();
  ASSERT_EQ(drb_setup.drbid, 1);
  ASSERT_EQ(resp->duto_currc_info.value.cell_group_cfg,
            this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container);
}

TEST_F(f1ap_du_test, when_f1ap_ue_context_setup_request_is_received_the_rrc_container_is_sent_dl_via_srb1)
{
  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);
  this->f1c_du_cfg_handler.next_ue_context_update_response.result                 = true;
  this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Test Section - Send UE CONTEXT SETUP REQUEST to F1AP.
  f1c_message msg = generate_f1_ue_context_setup_request({drb_id_t::drb1});
  f1ap->handle_message(msg);

  // F1AP sends RRC Container present in UE CONTEXT SETUP REQUEST via SRB1.
  ASSERT_EQ(test_ues[ue_index].f1c_bearers[1].rx_sdu_notifier.last_pdu,
            msg.pdu.init_msg().value.ue_context_setup_request()->rrc_container.value);
}

TEST_F(f1ap_du_test, when_f1ap_ue_context_setup_request_is_received_new_srbs_become_active)
{
  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);
  run_ue_context_setup_procedure(ue_index, {drb_id_t::drb1});

  // UL data through created SRB2 reaches F1-C.
  ASSERT_EQ(this->f1c_du_cfg_handler.last_ue_cfg_response->f1c_bearers_added.size(), 1);
  f1c_bearer* srb2 = this->f1c_du_cfg_handler.last_ue_cfg_response->f1c_bearers_added[0].bearer;
  byte_buffer ul_rrc_msg{test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100))};
  srb2->handle_sdu(byte_buffer_slice_chain{ul_rrc_msg.copy()});
  ASSERT_EQ(this->msg_notifier.last_f1c_msg.pdu.type().value, f1_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            f1_ap_elem_procs_o::init_msg_c::types_opts::ulrrc_msg_transfer);
  const ulrrc_msg_transfer_s& ulmsg = this->msg_notifier.last_f1c_msg.pdu.init_msg().value.ulrrc_msg_transfer();
  ASSERT_EQ(ulmsg->rrc_container.value, ul_rrc_msg);
}