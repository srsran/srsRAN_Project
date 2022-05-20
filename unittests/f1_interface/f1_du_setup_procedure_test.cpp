/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/du_manager/converters/f1_procedure_helpers.h"
#include "../../lib/du_manager/du_manager_config.h"
#include "../../lib/du_manager/du_manager_context.h"
#include "../../lib/du_manager/procedures/initial_du_setup_procedure.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/f1_interface/f1ap_du_factory.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

enum class test_outcome { success, failure };

/// Test the f1 setup procedure
void test_f1_setup(test_outcome outcome)
{
  test_delimit_logger   delimiter{"Test F12 setup procedure. Outcome: {}",
                                outcome == test_outcome::success ? "Success" : "Failure"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  f1c_message_handler* msg_handler = {};
  auto                 f1ap_du     = create_f1ap_du(*msg_handler);

  f1_setup_request_message request_msg  = {};
  du_setup_params          setup_params = {};
  f1ap::fill_f1_setup_request_message(request_msg, setup_params);

  // Action 1: Launch F1 setup procedure
  test_logger.info("TEST: Lauch f1 setup request procedure...");
  async_task<du_setup_result>         t = f1ap_du->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<du_setup_result> t_launcher(t);

  // Status
  TESTASSERT(not t.ready());

  // Action 2: F1 setup response received.
  asn1::f1ap::f1_ap_pdu_c pdu;
  if (outcome == test_outcome::success) {
    pdu.set_successful_outcome();
    pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    auto& setup_res                 = pdu.successful_outcome().value.f1_setup_resp();
    setup_res->transaction_id.value = 99;
    setup_res->gnb_cu_name_present  = true;
    setup_res->gnb_cu_name.value.from_string("srsCU");
    setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

    f1ap_du->handle_message(pdu);

    TESTASSERT(t.ready());

    TESTASSERT(t.get().result.has_value());

  } else {
    pdu.set_unsuccessful_outcome();
    pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    auto& setup_fail                 = pdu.unsuccessful_outcome().value.f1_setup_fail();
    setup_fail->transaction_id.value = 99;
    setup_fail->cause.value.set_radio_network();
    setup_fail->cause.value.radio_network() =
        asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
    setup_fail->time_to_wait_present     = true;
    setup_fail->time_to_wait.value       = asn1::f1ap::time_to_wait_opts::v10s;
    setup_fail->crit_diagnostics_present = false;
    // add critical diagnostics

    f1ap_du->handle_message(pdu);

    TESTASSERT(t.ready());

    TESTASSERT(t.get().result.is_error());
  }
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_f1_setup(test_outcome::success);
  test_f1_setup(test_outcome::failure);
}