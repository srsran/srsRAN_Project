/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/du_manager/converters/f1c_configuration_helpers.h"
#include "lib/f1_interface/common/f1ap_asn1_utils.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/f1_interface/du/f1ap_du_factory.h"
#include "unittests/f1_interface/common/test_helpers.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_du {

/// Fixture class for F1AP
class f1ap_du_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    f1ap = create_f1ap(timers, msg_notifier);
  }

  std::unique_ptr<f1_interface> f1ap;
  f1c_null_notifier             msg_notifier = {};
  timer_manager                 timers;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

f1_setup_request_message generate_f1_setup_request_message()
{
  f1_setup_request_message           request_msg  = {};
  du_setup_params                    setup_params = {};
  du_cell_config                     cell         = du_config_helpers::make_default_du_cell_config();
  std::vector<const du_cell_config*> cells        = {&cell};
  fill_asn1_f1_setup_request(request_msg.msg, setup_params, cells);

  return request_msg;
}

f1c_msg generate_f1_setup_response_message(unsigned transaction_id)
{
  f1c_msg f1_setup_response = {};

  f1_setup_response.pdu.set_successful_outcome();
  f1_setup_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_res                 = f1_setup_response.pdu.successful_outcome().value.f1_setup_resp();
  setup_res->transaction_id.value = transaction_id;
  setup_res->gnb_cu_name_present  = true;
  setup_res->gnb_cu_name.value.from_string("srsCU");
  setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

  return f1_setup_response;
}

f1c_msg generate_f1_setup_failure_message(unsigned transaction_id)
{
  f1c_msg f1_setup_failure = {};

  f1_setup_failure.pdu.set_unsuccessful_outcome();
  f1_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_fail                 = f1_setup_failure.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->transaction_id.value = transaction_id;
  setup_fail->cause.value.set_radio_network();
  setup_fail->cause.value.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  setup_fail->time_to_wait_present     = false;
  setup_fail->crit_diagnostics_present = false;
  // add critical diagnostics

  return f1_setup_failure;
}

f1c_msg generate_f1_setup_failure_message_with_time_to_wait(unsigned                   transaction_id,
                                                            asn1::f1ap::time_to_wait_e time_to_wait)
{
  f1c_msg f1_setup_failure = generate_f1_setup_failure_message(transaction_id);

  auto& setup_fail                 = f1_setup_failure.pdu.unsuccessful_outcome().value.f1_setup_fail();
  setup_fail->time_to_wait_present = true;
  setup_fail->time_to_wait.value   = time_to_wait;

  return f1_setup_failure;
}

} // namespace srs_du
} // namespace srsgnb
