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

#include "lib/e2/common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/e2/e2_event_manager.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/async/coroutine.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
/// Test successful cu-cp initiated e2 setup procedure
TEST_F(e2_test_subscriber, when_e2_subscription_request_correct_sent_subscription_response)
{
  using namespace asn1::e2ap;
  // Action 1: Create valid e2 message
  uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                            0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                            0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                            0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer e2ap_sub_req_buf(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req));
  packer->handle_packed_pdu(std::move(e2ap_sub_req_buf));

  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome);
}
TEST_F(e2_test_subscriber, when_e2_subscription_request_received_start_indication_procedure)
{
  using namespace asn1::e2ap;
  // Action 1: Create valid e2 message
  uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                            0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                            0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                            0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer e2ap_sub_req_buf(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req));
  packer->handle_packed_pdu(std::move(e2ap_sub_req_buf));
  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome);
  for (int i = 0; i < 1500; i++) {
    this->tick();
  }
  asn1::cbit_ref bref1(gw->last_pdu);
  e2_message     msg1 = {};
  if (msg1.pdu.unpack(bref1) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }

  ASSERT_EQ(msg1.pdu.init_msg().value.type(), e2_ap_elem_procs_o::init_msg_c::types_opts::ri_cind);
}

TEST_F(e2_test_subscriber, start_infication_procedure_check_contents)
{
  using namespace asn1::e2ap;
  // subscription info
  e2_subscription_info_t sub_info        = {};
  sub_info.report_period                 = 1000;
  sub_info.request_id.ric_instance_id    = 1;
  sub_info.request_id.ric_requestor_id   = 2;
  ric_action_t                    action = {};
  asn1::unbounded_octstring<true> action_def;
  action_def.from_number(01020304);
  sub_info.action_list.push_back({action_def.deep_copy(), 1, asn1::e2ap::ri_caction_type_e::report});
  std::unique_ptr<e2_event_manager> ev_mng = std::make_unique<e2_event_manager>(factory);
  ev_mng->add_sub_del_req(sub_info.request_id.ric_instance_id, factory);
  auto task =
      launch_async<e2_indication_procedure>(*msg_notifier,
                                            *(e2_subscription_mngr->get_e2sm_interface("1.3.6.1.4.1.53148.1.2.2.2")),
                                            *ev_mng,
                                            sub_info,
                                            test_logger);

  ASSERT_FALSE(task.ready());
  for (int i = 0; i < 1500; i++) {
    this->tick();
  }

  asn1::cbit_ref bref1(msg_notifier->last_e2_msg.pdu.init_msg().value.ri_cind()->ri_cind_msg.value);
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s ind_msg1 = {};
  if (ind_msg1.unpack(bref1) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(ind_msg1.ind_msg_formats.type(),
            asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format2);

  ASSERT_EQ(ind_msg1.ind_msg_formats.ind_msg_format2().granul_period, 10);
  for (auto& meas_item : ind_msg1.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list) {
    for (auto match_cond_it : meas_item.matching_cond) {
      ASSERT_EQ(srsran::e2sm_kpm_impl::supported_test_cond_type(
                    match_cond_it.matching_cond_choice.test_cond_info().test_type),
                true);
    }
  }
}

TEST_F(e2_test_subscriber, start_subscription_then_delete_subscription)
{
  using namespace asn1::e2ap;
  // Action 1: Create valid e2 message
  uint8_t e2ap_sub_req[] = {0x00, 0x08, 0x40, 0x2b, 0x00, 0x00, 0x03, 0x00, 0x1d, 0x00, 0x05, 0x00,
                            0x00, 0x7b, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1e,
                            0x00, 0x15, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x13, 0x40,
                            0x0a, 0x60, 0x01, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x02, 0x00};

  byte_buffer e2ap_sub_req_buf(e2ap_sub_req, e2ap_sub_req + sizeof(e2ap_sub_req));
  packer->handle_packed_pdu(std::move(e2ap_sub_req_buf));

  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg.pdu.successful_outcome().value.type(),
            e2_ap_elem_procs_o::successful_outcome_c::types_opts::ricsubscription_resp);

  uint8_t     sub_del_req[] = {0x00, 0x09, 0x00, 0x12, 0x00, 0x00, 0x02, 0x00, 0x1d, 0x00, 0x05,
                               0x00, 0x03, 0xfd, 0x00, 0x15, 0x00, 0x05, 0x00, 0x02, 0x00, 0x93};
  byte_buffer sub_del_req_buf(sub_del_req, sub_del_req + sizeof(sub_del_req));

  packer->handle_packed_pdu(std::move(sub_del_req_buf));

  asn1::cbit_ref bref1(gw->last_pdu);
  e2_message     msg1 = {};
  if (msg1.pdu.unpack(bref1) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg1.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg1.pdu.successful_outcome().value.type(),
            e2_ap_elem_procs_o::successful_outcome_c::types_opts::ricsubscription_delete_resp);
}