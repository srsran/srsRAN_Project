/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../du_processor_test_messages.h"
#include "du_processor_test_helpers.h"
#include "lib/cu_cp/du_processor/du_processor.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* F1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful f1 setup procedure
TEST_F(du_processor_test, when_valid_f1setup_received_then_f1_setup_response_sent)
{
  // Pass F1 Setup Request to DU processor
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check response is F1SetupResponse
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.successful_outcome().value.type(),
            f1ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp);
}

TEST_F(du_processor_test, when_du_served_cells_list_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing du served cells list
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list_present = false;
  f1_setup_req.pdu.init_msg().value.f1_setup_request()->gnb_du_served_cells_list.clear();

  // Pass message to DU processor
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_gnb_du_sys_info_missing_then_f1setup_rejected)
{
  // Generate F1SetupRequest with missing gnb du sys info
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request();
  f1_setup_req.pdu.init_msg()
      .value.f1_setup_request()
      ->gnb_du_served_cells_list[0]
      .value()
      .gnb_du_served_cells_item()
      .gnb_du_sys_info_present = false;

  // Pass message to DU processor
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(du_processor_test, when_max_nof_du_cells_exeeded_then_f1setup_rejected)
{
  // Generate F1SetupRequest with too many cells
  f1ap_message f1ap_msg = create_f1_setup_request_with_too_many_cells();

  // Pass message to DU processor
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(f1ap_msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.type(), f1ap_pdu_c::types_opts::options::unsuccessful_outcome);
  ASSERT_EQ(f1ap_pdu_notifier.last_f1ap_msg.pdu.unsuccessful_outcome().value.type(),
            f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

//////////////////////////////////////////////////////////////////////////////////////
/* UE creation                                                                      */
//////////////////////////////////////////////////////////////////////////////////////

class du_processor_ue_creation_test : public du_processor_test
{
protected:
  du_processor_ue_creation_test()
  {
    du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
        test_helpers::generate_f1_setup_request());
  }

  static f1ap_message create_valid_ue_creation_message()
  {
    return test_helpers::create_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t{0}, rnti_t::MIN_CRNTI);
  }
};

TEST_F(du_processor_ue_creation_test, when_init_ul_rrc_message_is_valid_then_ue_added)
{
  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(
      this->create_valid_ue_creation_message());

  ASSERT_EQ(ue_mng.get_nof_ues(), 1);
}

TEST_F(du_processor_ue_creation_test, when_init_ul_rrc_message_is_invalid_then_ue_is_not_added)
{
  f1ap_message msg = this->create_valid_ue_creation_message();
  msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->nr_cgi.nr_cell_id.from_number(1);

  du_processor_obj->get_f1ap_handler().get_f1ap_message_handler().handle_message(msg);

  ASSERT_EQ(ue_mng.get_nof_ues(), 0);
}
