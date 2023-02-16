/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_test_helpers.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_du;
using namespace asn1::f1ap;

class f1ap_du_ue_context_modification_test : public f1ap_du_test
{
protected:
  f1ap_du_ue_context_modification_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    run_f1_ue_create(test_ue_index);
    run_ue_context_setup_procedure(test_ue_index, generate_f1_ue_context_setup_request({}));
  }

  void start_procedure(const std::initializer_list<drb_id_t>& drbs)
  {
    // Prepare DU manager response to F1AP.
    this->f1c_du_cfg_handler.next_ue_context_update_response.result = true;
    for (drb_id_t drb_id : drbs) {
      this->f1c_du_cfg_handler.next_ue_context_update_response.drbs_setup.emplace_back();
      auto& drb  = this->f1c_du_cfg_handler.next_ue_context_update_response.drbs_setup.back();
      drb.drb_id = drb_id;
      drb.dluptnl_info_list.resize(1);
      drb.dluptnl_info_list[0].gtp_teid   = int_to_gtp_teid(test_rgen::uniform_int<uint32_t>());
      drb.dluptnl_info_list[0].tp_address = transport_layer_address{"127.0.0.1"};
    }
    this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

    // Initiate procedure in F1AP.
    f1c_message msg = generate_f1_ue_context_modification_request(drbs);
    f1ap->handle_message(msg);
  }

  bool was_ue_context_modification_response_sent() const
  {
    return this->msg_notifier.last_f1c_msg.pdu.type().value == f1ap_pdu_c::types_opts::successful_outcome and
           this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.type().value ==
               f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_mod_resp;
  }

  du_ue_index_t test_ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
};

TEST_F(f1ap_du_ue_context_modification_test, when_f1ap_receives_request_then_f1ap_notifies_du_of_ue_context_update)
{
  start_procedure({drb_id_t::drb1});

  // DU manager receives UE Context Update Request.
  ASSERT_TRUE(this->f1c_du_cfg_handler.last_ue_context_update_req.has_value());
  const f1ap_ue_context_update_request& req = *this->f1c_du_cfg_handler.last_ue_context_update_req;
  ASSERT_EQ(req.ue_index, test_ue_index);
  ASSERT_EQ(req.srbs_to_setup.size(), 0);
  ASSERT_EQ(req.drbs_to_setup.size(), 1);
  ASSERT_EQ(req.drbs_to_setup[0].drb_id, drb_id_t::drb1);
  ASSERT_FALSE(req.drbs_to_setup[0].lcid.has_value());
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_f1ap_receives_request_then_f1ap_responds_to_cu_with_ue_context_modification_response)
{
  start_procedure({drb_id_t::drb1});

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  ASSERT_TRUE(was_ue_context_modification_response_sent());
  ue_context_mod_resp_s& resp = this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.ue_context_mod_resp();
  ASSERT_FALSE(resp->srbs_failed_to_be_setup_mod_list_present);
  ASSERT_FALSE(resp->srbs_modified_list_present);
  ASSERT_FALSE(resp->srbs_setup_mod_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_modified_list_present);
  ASSERT_FALSE(resp->drbs_modified_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_setup_mod_list_present);
  ASSERT_TRUE(resp->drbs_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_setup_mod_list->size(), 1);
  auto& drb_setup = resp->drbs_setup_mod_list.value[0].value().drbs_setup_mod_item();
  ASSERT_EQ(drb_setup.drb_id, 1);
  ASSERT_EQ(drb_setup.dl_up_tnl_info_to_be_setup_list.size(), 1);
  ASSERT_EQ(
      int_to_gtp_teid(drb_setup.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.gtp_tunnel().gtp_teid.to_number()),
      this->f1c_du_cfg_handler.next_ue_context_update_response.drbs_setup[0].dluptnl_info_list[0].gtp_teid);
  ASSERT_EQ(
      drb_setup.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.gtp_tunnel().transport_layer_address.to_string(),
      this->f1c_du_cfg_handler.next_ue_context_update_response.drbs_setup[0]
          .dluptnl_info_list[0]
          .tp_address.to_bitstring());
  ASSERT_EQ(resp->du_to_cu_rrc_info.value.cell_group_cfg,
            this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container);
}

TEST_F(f1ap_du_ue_context_modification_test,
       when_du_fails_to_create_drb_then_f1ap_adds_the_failed_drb_in_ue_context_modification_response)
{
  // Prepare DU manager response to F1AP with failed DRB.
  this->f1c_du_cfg_handler.next_ue_context_update_response.result = true;
  this->f1c_du_cfg_handler.next_ue_context_update_response.drbs_failed_to_setup.push_back(drb_id_t::drb1);
  this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container = {0x1, 0x2, 0x3};

  // Initiate procedure in F1AP.
  f1c_message msg = generate_f1_ue_context_modification_request({drb_id_t::drb1});
  f1ap->handle_message(msg);

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP with failed DRB.
  ASSERT_TRUE(was_ue_context_modification_response_sent());
  ue_context_mod_resp_s& resp = this->msg_notifier.last_f1c_msg.pdu.successful_outcome().value.ue_context_mod_resp();
  ASSERT_FALSE(resp->srbs_failed_to_be_setup_mod_list_present);
  ASSERT_FALSE(resp->srbs_modified_list_present);
  ASSERT_FALSE(resp->srbs_setup_mod_list_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_modified_list_present);
  ASSERT_FALSE(resp->drbs_modified_list_present);
  ASSERT_TRUE(resp->drbs_failed_to_be_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_failed_to_be_setup_mod_list->size(), 1);
  ASSERT_EQ(resp->drbs_failed_to_be_setup_mod_list.value[0].value().drbs_failed_to_be_setup_mod_item().drb_id, 1);
  ASSERT_FALSE(resp->drbs_setup_mod_list_present);
  ASSERT_EQ(resp->drbs_setup_mod_list->size(), 0);
  ASSERT_EQ(resp->du_to_cu_rrc_info.value.cell_group_cfg,
            this->f1c_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container);
}
