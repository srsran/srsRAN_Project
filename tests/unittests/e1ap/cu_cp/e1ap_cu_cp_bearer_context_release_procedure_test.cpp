/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_cp_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::e1ap;

class e1ap_cu_cp_bearer_context_release_test : public e1ap_cu_cp_test
{
protected:
  void start_procedure(const e1ap_bearer_context_release_command& req)
  {
    run_bearer_context_setup(req.ue_index,
                             int_to_gnb_cu_up_ue_e1ap_id(test_rgen::uniform_int<uint64_t>(
                                 gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::min),
                                 gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::max) - 1)));

    t = e1ap->handle_bearer_context_release_command(req);
    t_launcher.emplace(t);

    ASSERT_FALSE(t.ready());
    ASSERT_EQ(this->e1_pdu_notifier.last_e1_msg.pdu.init_msg().value.type().value,
              e1ap_elem_procs_o::init_msg_c::types::bearer_context_release_cmd);

    test_ues[req.ue_index].cu_cp_ue_e1ap_id =
        int_to_gnb_cu_cp_ue_e1ap_id(this->e1_pdu_notifier.last_e1_msg.pdu.init_msg()
                                        .value.bearer_context_release_cmd()
                                        ->gnb_cu_cp_ue_e1ap_id.value);
  }

  bool was_bearer_context_release_command_sent(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const
  {
    if (this->e1_pdu_notifier.last_e1_msg.pdu.type().value != e1ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->e1_pdu_notifier.last_e1_msg.pdu.init_msg().value.type().value !=
        e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_release_cmd) {
      return false;
    }
    auto& req = this->e1_pdu_notifier.last_e1_msg.pdu.init_msg().value.bearer_context_release_cmd();

    return req->gnb_cu_cp_ue_e1ap_id.value == gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id);
  }

  bool was_bearer_context_release_complete_received() const
  {
    if (not t.ready()) {
      return false;
    }
    return true;
  }

  async_task<void>                   t;
  optional<lazy_task_launcher<void>> t_launcher;
};

TEST_F(e1ap_cu_cp_bearer_context_release_test, when_command_sent_then_procedure_waits_for_response)
{
  // Test Preamble.
  auto command = generate_bearer_context_release_command(uint_to_ue_index(
      test_rgen::uniform_int<uint32_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1)));

  // Start BEARER CONTEXT RELEASE procedure.
  this->start_procedure(command);

  // The BEARER CONTEXT RELEASE was sent to CU-UP and CU-CP is waiting for response.
  ASSERT_TRUE(was_bearer_context_release_command_sent(test_ues[command.ue_index].cu_cp_ue_e1ap_id.value()));
  ASSERT_FALSE(t.ready());
}

TEST_F(e1ap_cu_cp_bearer_context_release_test, when_bearer_release_complete_received_then_procedure_is_successful)
{
  // Test Preamble.
  auto command = generate_bearer_context_release_command(uint_to_ue_index(
      test_rgen::uniform_int<uint32_t>(ue_index_to_uint(ue_index_t::min), ue_index_to_uint(ue_index_t::max) - 1)));

  // Start BEARER CONTEXT RELEASE procedure and return back the response from the CU-UP.
  this->start_procedure(command);

  auto&      ue = test_ues[command.ue_index];
  e1_message response =
      generate_bearer_context_release_complete(ue.cu_cp_ue_e1ap_id.value(), ue.cu_up_ue_e1ap_id.value());
  e1ap->handle_message(response);

  // The BEARER CONTEXT RELEASE RESPONSE was received and the CU-CP completed the procedure.
  ASSERT_TRUE(was_bearer_context_release_complete_received());
}
