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
  }

  bool was_ue_context_modification_msg_sent(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id) const
  {
    if (this->f1c_pdu_notifier.last_f1c_msg.pdu.type().value != f1_ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->f1c_pdu_notifier.last_f1c_msg.pdu.init_msg().value.type().value !=
        asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request) {
      return false;
    }
    auto& req = this->f1c_pdu_notifier.last_f1c_msg.pdu.init_msg().value.ue_context_mod_request();

    return req->gnb_cu_ue_f1_ap_id.value == (unsigned)cu_ue_id and req->gnb_du_ue_f1_ap_id.value == (unsigned)du_ue_id;
  }

  async_task<f1ap_ue_context_modification_response>                   t;
  optional<lazy_task_launcher<f1ap_ue_context_modification_response>> t_launcher;
};

TEST_F(f1ap_cu_ue_context_modification_test, when_f1ap_receives_request_then_procedure_waits_for_du_response)
{
  // Test Preamble.
  gnb_cu_ue_f1ap_id_t                        cu_ue_id = int_to_gnb_cu_ue_f1ap_id(test_rgen::uniform_int<uint32_t>());
  gnb_du_ue_f1ap_id_t                        du_ue_id = int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint32_t>());
  const f1ap_ue_context_modification_request request_msg =
      create_ue_context_modification_request(cu_ue_id, du_ue_id, {drb_id_t::drb1});
  this->start_procedure(request_msg);

  // The UE CONTEXT MODIFICATION was sent to DU and F1AP-CU is waiting for response.
  ASSERT_TRUE(was_ue_context_modification_msg_sent(cu_ue_id, du_ue_id));
  ASSERT_FALSE(t.ready());
}

TEST_F(f1ap_cu_ue_context_modification_test, when_f1ap_receives_response_then_procedure_completes)
{
  // Test Preamble.
  gnb_cu_ue_f1ap_id_t                        cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t                        du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  const f1ap_ue_context_modification_request request_msg =
      create_ue_context_modification_request(cu_ue_id, du_ue_id, {drb_id_t::drb1});
  this->start_procedure(request_msg);
  f1c_message ue_context_modification_response = generate_ue_context_modification_response(cu_ue_id, du_ue_id);
  f1ap->handle_message(ue_context_modification_response);

  // The UE CONTEXT MODIFICATION RESPONSE was received.
  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().response->gnb_du_ue_f1_ap_id.value, (unsigned)du_ue_id);
  EXPECT_EQ(t.get().response->drbs_setup_mod_list.value.size(), 1);
}

/// Test the unsuccessful UE context modification procedure (gNB-CU initiated)
TEST_F(f1ap_cu_test, when_ue_modification_failure_received_then_procedure_unsuccessful)
{
  // Action 1: Launch UE Context modification procedure
  f1ap_ue_context_modification_request request_msg = {};
  test_logger.info("Launch ue context modification procedure...");
  async_task<f1ap_ue_context_modification_response> t = f1ap->handle_ue_context_modification_request(request_msg);
  lazy_task_launcher<f1ap_ue_context_modification_response> t_launcher(t);

  // Status: DU received UE Context modification Request message.
  EXPECT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(f1c_pdu_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 UE Context modification Failure received.
  f1c_message ue_context_modification_failure =
      generate_ue_context_modification_failure(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255));
  test_logger.info("Injecting UEContextModificationFailure");
  f1ap->handle_message(ue_context_modification_failure);

  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
  EXPECT_EQ(t.get().failure->gnb_du_ue_f1_ap_id.value, 41255U);
}
