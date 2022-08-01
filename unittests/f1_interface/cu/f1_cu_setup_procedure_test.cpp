/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../common_test_helpers/f1_cu_test_helpers.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/f1_interface/common/test_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

enum class test_outcome { success, failure };

/// Test the f1 setup procedure
void test_f1_setup(test_outcome outcome)
{
  test_delimit_logger   delimiter{"Test F1 setup procedure. Outcome: {}",
                                outcome == test_outcome::success ? "Success" : "Failure"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  dummy_f1c_pdu_notifier           f1c_pdu_notifier(nullptr);
  dummy_f1c_du_processor_notifier  du_processor_notifier;
  dummy_f1c_du_management_notifier f1c_du_mgmt_notifier(nullptr);
  auto                             f1ap = create_f1ap(f1c_pdu_notifier, du_processor_notifier, f1c_du_mgmt_notifier);

  // Action 1: Receive F1SetupRequest message
  test_logger.info("TEST: Receive F1SetupRequest message...");

  // Successful outcome
  if (outcome == test_outcome::success) {
    // Generate F1SetupRequest
    f1c_msg f1setup_msg = generate_valid_f1_setup_request();

    f1ap->handle_message(f1setup_msg);

    // Action 2: Check if F1SetupRequest was forwarded to CU manager
    TESTASSERT(du_processor_notifier.last_f1_setup_request_msg.request->gnb_du_id.value = 0x11);

    // Action 3: Transmit F1SetupResponse message
    test_logger.info("TEST: Transmit F1SetupResponse message...");
    f1_setup_response_message msg = {};
    msg.success                   = true;
    f1ap->handle_f1ap_setup_response(msg);

    // Check the generated PDU is indeed the F1 Setup response
    TESTASSERT_EQ(asn1::f1ap::f1_ap_pdu_c::types_opts::options::successful_outcome,
                  f1c_pdu_notifier.last_f1c_msg.pdu.type());
    TESTASSERT_EQ(asn1::f1ap::f1_ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp,
                  f1c_pdu_notifier.last_f1c_msg.pdu.successful_outcome().value.type());
  } else {
    // Generate F1SetupRequest
    f1c_msg f1setup_msg = generate_f1_setup_request_base();

    auto& setup_req = f1setup_msg.pdu.init_msg().value.f1_setup_request();

    setup_req->gnb_du_served_cells_list_present = true;
    setup_req->gnb_du_served_cells_list.id      = ASN1_F1AP_ID_G_NB_DU_SERVED_CELLS_LIST;
    setup_req->gnb_du_served_cells_list.crit    = asn1::crit_opts::reject;

    f1ap->handle_message(f1setup_msg);

    // Action 2: Check if F1SetupRequest was forwarded to CU manager
    TESTASSERT(du_processor_notifier.last_f1_setup_request_msg.request->gnb_du_id.value = 0x11);

    // Action 3: Transmit F1SetupFailure message
    test_logger.info("TEST: Transmit F1SetupFailure message...");
    f1_setup_response_message msg = {};
    msg.success                   = false;
    f1ap->handle_f1ap_setup_response(msg);

    // Check the generated PDU is indeed the F1 Setup failure
    TESTASSERT_EQ(asn1::f1ap::f1_ap_pdu_c::types_opts::options::unsuccessful_outcome,
                  f1c_pdu_notifier.last_f1c_msg.pdu.type());
    TESTASSERT_EQ(asn1::f1ap::f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail,
                  f1c_pdu_notifier.last_f1c_msg.pdu.unsuccessful_outcome().value.type());
  }
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_f1_setup(test_outcome::success);
  test_f1_setup(test_outcome::failure);
}