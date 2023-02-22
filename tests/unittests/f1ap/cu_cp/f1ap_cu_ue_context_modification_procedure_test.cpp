/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

class f1ap_cu_ue_context_modification_test : public f1ap_cu_test
{
protected:
  f1ap_cu_ue_context_modification_test() :
    testue(create_ue(int_to_gnb_du_ue_f1ap_id(test_rgen::uniform_int<uint32_t>())))
  {
    run_ue_context_setup(testue.ue_index);
  }

  void start_procedure(const cu_cp_ue_context_modification_request& req)
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

    return req->gnb_du_ue_f1ap_id.value == (unsigned)du_ue_id;
  }

  test_ue& testue;

  async_task<cu_cp_ue_context_modification_response>                   t;
  optional<lazy_task_launcher<cu_cp_ue_context_modification_response>> t_launcher;
};

TEST_F(f1ap_cu_ue_context_modification_test, when_ue_modification_response_received_then_procedure_is_successful)
{
  // Start UE CONTEXT MODIFICATION procedure.
  this->start_procedure(generate_ue_context_modification_request(testue.ue_index, {drb_id_t::drb1}));

  // The UE CONTEXT MODIFICATION was sent to DU and F1AP-CU is waiting for response.
  ASSERT_TRUE(was_ue_context_modification_msg_sent(*testue.du_ue_id));
  ASSERT_FALSE(t.ready());
}

TEST_F(f1ap_cu_ue_context_modification_test, when_f1ap_receives_response_then_procedure_completes)
{
  // Start UE CONTEXT MODIFICATION procedure and return back the response from the DU.
  this->start_procedure(generate_ue_context_modification_request(testue.ue_index, {drb_id_t::drb1}));
  f1ap_message ue_context_modification_response =
      generate_ue_context_modification_response(*testue.cu_ue_id, *testue.du_ue_id, to_rnti(0x4601), {drb_id_t::drb1});
  f1ap->handle_message(ue_context_modification_response);

  // The UE CONTEXT MODIFICATION procedure finished successfully.
  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().drbs_setup_mod_list.size(), 1);
  EXPECT_EQ(t.get().srbs_setup_mod_list.size(), 0);
}

TEST_F(f1ap_cu_ue_context_modification_test,
       when_f1ap_receives_response_with_failed_drb_then_it_forwards_the_failed_drb_outwards)
{
  // Start UE CONTEXT MODIFICATION procedure and return back the Failure Response from the DU.
  this->start_procedure(generate_ue_context_modification_request(testue.ue_index, {drb_id_t::drb1}));
  f1ap_message ue_context_modification_response =
      generate_ue_context_modification_response(*testue.cu_ue_id, *testue.du_ue_id, to_rnti(0x4601), {drb_id_t::drb1});
  auto& resp = *ue_context_modification_response.pdu.successful_outcome().value.ue_context_mod_resp();
  resp.drbs_failed_to_be_setup_mod_list_present = true;
  resp.drbs_failed_to_be_setup_mod_list.value.resize(1);
  resp.drbs_failed_to_be_setup_mod_list.value[0].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_ITEM);
  resp.drbs_failed_to_be_setup_mod_list.value[0]->drbs_failed_to_be_setup_mod_item().drb_id = 1;
  resp.drbs_setup_mod_list_present                                                          = false;
  resp.drbs_setup_mod_list.value.clear();
  f1ap->handle_message(ue_context_modification_response);

  // The UE CONTEXT MODIFICATION procedure finished unsuccessfully.
  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().drbs_setup_mod_list.size(), 0);
  EXPECT_EQ(t.get().srbs_setup_mod_list.size(), 0);
  EXPECT_EQ(t.get().drbs_failed_to_be_setup_mod_list.size(), 1);
  EXPECT_EQ(t.get().drbs_failed_to_be_setup_mod_list[drb_id_t::drb1].drb_id, drb_id_t::drb1);
}

TEST_F(f1ap_cu_ue_context_modification_test, when_ue_modification_failure_received_then_procedure_is_unsuccessful)
{
  // Start UE CONTEXT MODIFICATION procedure and return back the Failure Response from the DU.
  this->start_procedure(generate_ue_context_modification_request(testue.ue_index, {drb_id_t::drb1}));
  f1ap_message ue_context_modification_failure =
      generate_ue_context_modification_failure(*testue.cu_ue_id, *testue.du_ue_id);
  f1ap->handle_message(ue_context_modification_failure);

  // The UE CONTEXT MODIFICATION procedure finished unsuccessfully.
  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
  EXPECT_EQ(t.get().drbs_setup_mod_list.size(), 0);
  EXPECT_EQ(t.get().srbs_setup_mod_list.size(), 0);
}
