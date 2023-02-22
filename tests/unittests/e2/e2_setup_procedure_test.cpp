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
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test successful cu-cp initiated e2 setup procedure
TEST_F(e2_test, when_e2_setup_response_received_then_E2_connected)
{
  // Action 1: Launch E2 setup procedure
  e2_setup_request_message request_msg = generate_e2_setup_request_message();
  test_logger.info("Launch e2 setup request procedure...");

  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request_msg);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(msg_notifier->last_e2_msg.pdu.type().value, asn1::e2ap::e2_ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_e2_msg.pdu.init_msg().value.type().value,
            asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(msg_notifier->last_e2_msg.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  test_logger.info("Injecting E2SetupResponse");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}
