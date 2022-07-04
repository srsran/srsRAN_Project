/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/du_manager/converters/f1c_configuration_helpers.h"
#include "lib/f1_interface/common/f1ap_asn1_utils.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/f1_interface/du/f1ap_du_factory.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/f1_interface/common/test_helpers.h"

using namespace srsgnb;
using namespace srs_du;

enum class test_outcome { success, failure };

/// Test the f1 setup procedure
void test_f1_setup(test_outcome initial_outcome, test_outcome retry_outcome)
{
  test_delimit_logger   delimiter{"Test F1 setup procedure. Initial outcome: {}, Retry outcome: {}",
                                initial_outcome == test_outcome::success ? "Success" : "Failure",
                                retry_outcome == test_outcome::success ? "Success" : "Failure"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
  timer_manager         timers;

  f1c_null_notifier msg_notifier = {};
  auto              f1ap_du      = create_f1ap(timers, msg_notifier);

  f1_setup_request_message           request_msg  = {};
  du_setup_params                    setup_params = {};
  du_cell_config                     cell         = du_config_helpers::make_default_du_cell_config();
  std::vector<const du_cell_config*> cells        = {&cell};
  fill_asn1_f1_setup_request(request_msg.msg, setup_params, cells);

  // Action 1: Launch F1 setup procedure
  test_logger.info("TEST: Lauch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap_du->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  TESTASSERT(msg_notifier.last_f1c_msg.pdu.type().value == asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  TESTASSERT(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value ==
             asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  TESTASSERT(not t.ready());

  // Action 2: F1 setup response received.
  f1c_msg  f1c_msg;
  unsigned transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  // Successful initial outcome
  if (initial_outcome == test_outcome::success) {
    f1c_msg.pdu.set_successful_outcome();
    f1c_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    auto& setup_res                 = f1c_msg.pdu.successful_outcome().value.f1_setup_resp();
    setup_res->transaction_id.value = transaction_id;
    setup_res->gnb_cu_name_present  = true;
    setup_res->gnb_cu_name.value.from_string("srsCU");
    setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

    f1ap_du->handle_message(f1c_msg);

    TESTASSERT(t.ready());
    TESTASSERT(t.get().success);
    TESTASSERT(t.get().msg->gnb_cu_rrc_version.value.latest_rrc_version.to_number() == 2);
  } else {
    // Unsuccessful initial outcome
    f1c_msg.pdu.set_unsuccessful_outcome();
    f1c_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    auto& setup_fail                 = f1c_msg.pdu.unsuccessful_outcome().value.f1_setup_fail();
    setup_fail->transaction_id.value = transaction_id;
    setup_fail->cause.value.set_radio_network();
    setup_fail->cause.value.radio_network() =
        asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
    setup_fail->time_to_wait_present     = true;
    setup_fail->time_to_wait.value       = asn1::f1ap::time_to_wait_opts::v10s;
    setup_fail->crit_diagnostics_present = false;
    // add critical diagnostics

    f1ap_du->handle_message(f1c_msg);

    // Status: CU received F1 Setup Request again.
    TESTASSERT(msg_notifier.last_f1c_msg.pdu.type().value == asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
    TESTASSERT(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value ==
               asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);
    unsigned transaction_id2 = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
    TESTASSERT_NEQ(transaction_id, transaction_id2);

    // Successful outcome after reinitiated F1 Setup
    if (retry_outcome == test_outcome::success) {
      f1c_msg.pdu.set_successful_outcome();
      f1c_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
      auto& setup_res                 = f1c_msg.pdu.successful_outcome().value.f1_setup_resp();
      setup_res->transaction_id.value = transaction_id2;
      setup_res->gnb_cu_name_present  = true;
      setup_res->gnb_cu_name.value.from_string("srsCU");
      setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

      f1ap_du->handle_message(f1c_msg);

      TESTASSERT(t.ready());
      TESTASSERT(t.get().success);
      TESTASSERT(t.get().msg->gnb_cu_rrc_version.value.latest_rrc_version.to_number() == 2);

      // Unsuccessful outcome after reinitiated F1 Setup
    } else {
      setup_fail->time_to_wait_present = false;
      setup_fail->transaction_id.value = transaction_id2;
      f1ap_du->handle_message(f1c_msg);

      TESTASSERT(t.ready());
      TESTASSERT(not t.get().success);
    }
  }
}

/// Test the f1 setup procedure
void test_f1_setup_retry_limit()
{
  test_delimit_logger   delimiter{"Test F1 setup procedure retry limit."};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
  timer_manager         timers;

  f1c_null_notifier msg_notifier = {};
  auto              f1ap_du      = create_f1ap(timers, msg_notifier);

  f1_setup_request_message           request_msg  = {};
  du_setup_params                    setup_params = {};
  du_cell_config                     cell         = du_config_helpers::make_default_du_cell_config();
  std::vector<const du_cell_config*> cells        = {&cell};
  fill_asn1_f1_setup_request(request_msg.msg, setup_params, cells);

  // Action 1: Launch F1 setup procedure
  test_logger.info("TEST: Lauch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap_du->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  TESTASSERT(msg_notifier.last_f1c_msg.pdu.type().value == asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  TESTASSERT(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value ==
             asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status
  TESTASSERT(not t.ready());

  // Action 2: F1 setup response received.
  f1c_msg  f1c_msg;
  unsigned transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();

  f1c_msg.pdu.set_unsuccessful_outcome();
  f1c_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  auto& setup_fail                 = f1c_msg.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->transaction_id.value = transaction_id;
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  setup_fail->time_to_wait_present     = true;
  setup_fail->time_to_wait.value       = asn1::f1ap::time_to_wait_opts::v10s;
  setup_fail->crit_diagnostics_present = false;
  // add critical diagnostics

  for (unsigned i = 0; i < request_msg.max_setup_retries + 1; i++) {
    f1ap_du->handle_message(f1c_msg);

    // Update transaction id.
    transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
    f1c_msg.pdu.unsuccessful_outcome().value.f1_setup_fail()->transaction_id.value = transaction_id;
  }

  TESTASSERT(t.ready());
  TESTASSERT(not t.get().success);
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_f1_setup(test_outcome::success, test_outcome::success);
  test_f1_setup(test_outcome::failure, test_outcome::success);
  test_f1_setup(test_outcome::failure, test_outcome::failure);
  test_f1_setup_retry_limit();
}
