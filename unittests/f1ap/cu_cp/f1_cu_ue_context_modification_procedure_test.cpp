/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_cu_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

class f1ap_cu_ue_context_modification_test : public f1ap_cu_test
{
protected:
  void start_procedure(const f1ap_ue_context_modification_request& req)
  {
    t = f1ap->handle_ue_context_modification_request(req);
    t_launcher.emplace(t);

    test_ues[req.ue_index].cu_ue_id = int_to_gnb_cu_ue_f1ap_id(
        this->f1c_pdu_notifier.last_f1c_msg.pdu.init_msg().value.ue_context_mod_request()->gnb_cu_ue_f1_ap_id.value);
  }

  bool was_ue_context_modification_msg_sent(gnb_du_ue_f1ap_id_t du_ue_id) const
  {
    if (this->f1c_pdu_notifier.last_f1c_msg.pdu.type().value != f1_ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->f1c_pdu_notifier.last_f1c_msg.pdu.init_msg().value.type().value !=
        asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request) {
      return false;
    }
    auto& req = this->f1c_pdu_notifier.last_f1c_msg.pdu.init_msg().value.ue_context_mod_request();

    return req->gnb_du_ue_f1_ap_id.value == (unsigned)du_ue_id;
  }

  async_task<f1ap_ue_context_modification_response>                   t;
  optional<lazy_task_launcher<f1ap_ue_context_modification_response>> t_launcher;
};

TEST_F(f1ap_cu_ue_context_modification_test, when_ue_modification_response_received_then_procedure_is_successful)
{
  // Test Preamble.
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint32_t>());
  test_ue&            u        = create_ue(du_ue_id);

  // Start UE CONTEXT MODIFICATION procedure.
  this->start_procedure(create_ue_context_modification_request(u.ue_index, {drb_id_t::drb1}));

  // The UE CONTEXT MODIFICATION was sent to DU and F1AP-CU is waiting for response.
  ASSERT_TRUE(was_ue_context_modification_msg_sent(du_ue_id));
  ASSERT_FALSE(t.ready());
}

TEST_F(f1ap_cu_ue_context_modification_test, when_f1ap_receives_response_then_procedure_completes)
{
  // Test Preamble.
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint32_t>());
  test_ue&            u        = create_ue(du_ue_id);

  // Start UE CONTEXT MODIFICATION procedure and return back the response from the DU.
  this->start_procedure(create_ue_context_modification_request(u.ue_index, {drb_id_t::drb1}));
  f1c_message ue_context_modification_response =
      generate_ue_context_modification_response(*u.cu_ue_id, du_ue_id, to_rnti(0x4601), {drb_id_t::drb1});
  f1ap->handle_message(ue_context_modification_response);

  // The UE CONTEXT MODIFICATION procedure finished successfully.
  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().response->drbs_setup_mod_list.value.size(), 1);
  EXPECT_EQ(t.get().response->srbs_setup_mod_list.value.size(), 0);
}

/// Test the unsuccessful UE context modification procedure (gNB-CU initiated)
TEST_F(f1ap_cu_ue_context_modification_test, when_ue_modification_failure_received_then_procedure_is_unsuccessful)
{
  // Test Preamble.
  test_ue& u = create_ue(int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint32_t>()));

  // Start UE CONTEXT MODIFICATION procedure and return back the Failure Response from the DU.
  this->start_procedure(create_ue_context_modification_request(u.ue_index, {drb_id_t::drb1}));
  f1c_message ue_context_modification_failure = generate_ue_context_modification_failure(*u.cu_ue_id, *u.du_ue_id);
  f1ap->handle_message(ue_context_modification_failure);

  // The UE CONTEXT MODIFICATION procedure finished unsuccessfully.
  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
  EXPECT_EQ(t.get().response->drbs_setup_mod_list.value.size(), 0);
  EXPECT_EQ(t.get().response->srbs_setup_mod_list.value.size(), 0);
}
