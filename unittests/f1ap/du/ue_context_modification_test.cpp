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
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::f1ap;

TEST_F(f1ap_du_test, when_f1ap_receives_ue_context_modification_request_then_f1ap_notifies_du_of_ue_context_update)
{
  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);
  run_ue_context_setup_procedure(ue_index, {});
  this->f1c_du_cfg_handler.next_ue_context_update_response.result                 = true;
  this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Test Section - Send UE CONTEXT MODIFICATION REQUEST to F1AP.
  f1c_message msg = generate_f1_ue_context_modification_request({drb_id_t::drb1});
  f1ap->handle_message(msg);

  // DU manager receives UE Context Update Request.
  ASSERT_TRUE(this->f1c_du_cfg_handler.last_ue_context_update_req.has_value());
  const f1ap_ue_context_update_request& req = *this->f1c_du_cfg_handler.last_ue_context_update_req;
  ASSERT_EQ(req.ue_index, ue_index);
  ASSERT_EQ(req.srbs_to_setup.size(), 0);
  ASSERT_EQ(req.drbs_to_setup.size(), 1);
  ASSERT_EQ(req.drbs_to_setup[0].drb_id, drb_id_t::drb1);
  ASSERT_FALSE(req.drbs_to_setup[0].lcid.has_value());
}

TEST_F(f1ap_du_test,
       when_f1ap_receives_ue_context_modification_request_then_f1ap_responds_with_ue_context_modification_response)
{
  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);
  run_ue_context_setup_procedure(ue_index, {});
  this->f1c_du_cfg_handler.next_ue_context_update_response.result                 = true;
  this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Test Section - Send UE CONTEXT MODIFICATION REQUEST to F1AP.
  f1c_message msg = generate_f1_ue_context_modification_request({drb_id_t::drb1});
  f1ap->handle_message(msg);

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  ASSERT_EQ(this->msg_notifier.last_f1c_msg.pdu.type().value, f1_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.type().value,
            f1_ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_mod_resp);
  ue_context_mod_resp_s& resp = this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.ue_context_mod_resp();
  ASSERT_FALSE(resp->srbs_failed_to_be_setup_mod_list_present);
  ASSERT_FALSE(resp->srbs_modified_list_present);
  ASSERT_FALSE(resp->srbs_setup_mod_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_modified_list_present);
  ASSERT_FALSE(resp->drbs_modified_list_present);
  ASSERT_TRUE(resp->drbs_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_setup_mod_list->size(), 1);
  auto& drb_setup = resp->drbs_setup_mod_list.value[0].value().drbs_setup_mod_item();
  ASSERT_EQ(drb_setup.drbid, 1);
  ASSERT_EQ(resp->duto_currc_info.value.cell_group_cfg,
            this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container);
}
