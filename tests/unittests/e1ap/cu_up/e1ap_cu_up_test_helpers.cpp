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

#include "e1ap_cu_up_test_helpers.h"
#include "lib/e1ap/common/e1ap_asn1_utils.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_up;

namespace {

class dummy_e1ap_tx_pdu_notifier : public e1ap_message_notifier
{
public:
  dummy_e1ap_tx_pdu_notifier(e1ap_message& last_tx_pdu_) : last_tx_pdu(last_tx_pdu_) {}

  void on_new_message(const e1ap_message& msg) override { last_tx_pdu = msg; }

  e1ap_message& last_tx_pdu;
};

} // namespace

std::unique_ptr<e1ap_message_notifier> dummy_e1ap_connection_client::handle_cu_up_connection_request(
    std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier_)
{
  cu_up_rx_pdu_notifier = std::move(cu_up_rx_pdu_notifier_);
  return std::make_unique<dummy_e1ap_tx_pdu_notifier>(last_tx_e1ap_pdu);
}

//////////////////////////////////

e1ap_cu_up_test::e1ap_cu_up_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  e1ap_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  e1ap = create_e1ap(e1ap_gw, cu_up_notifier, timers, cu_up_worker);
}

void e1ap_cu_up_test::run_e1_setup_procedure()
{
  // > Establish connection to CU-CP.
  bool ret = e1ap->connect_to_cu_cp();
  srsran_assert(ret, "Failed to connect to CU-CP");

  // > Launch E1 setup procedure
  cu_up_e1_setup_request request_msg = generate_cu_up_e1_setup_request();
  test_logger.info("Launch CU-UP e1 setup request procedure...");
  async_task<cu_up_e1_setup_response>         t = e1ap->handle_cu_up_e1_setup_request(request_msg);
  lazy_task_launcher<cu_up_e1_setup_response> t_launcher(t);

  // > E1 setup response received.
  unsigned     transaction_id    = get_transaction_id(e1ap_gw.last_tx_e1ap_pdu.pdu).value();
  e1ap_message e1_setup_response = generate_cu_up_e1_setup_response(transaction_id);
  test_logger.info("Injecting CuUpE1SetupResponse");
  e1ap->handle_message(e1_setup_response);
}

e1ap_cu_up_test::~e1ap_cu_up_test()
{
  // flush logger after each test
  srslog::flush();
}

void e1ap_cu_up_test::setup_bearer(unsigned int cu_cp_ue_e1ap_id)
{
  // Generate BearerContextSetupRequest
  e1ap_message bearer_context_setup = generate_bearer_context_setup_request(cu_cp_ue_e1ap_id);

  e1ap->handle_message(bearer_context_setup);
}
