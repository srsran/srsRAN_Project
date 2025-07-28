/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "f1ap_du_test_helpers.h"
#include "test_doubles/f1ap/f1ap_test_message_validators.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

static gnbdu_config_update_request create_du_config_update(const f1_setup_request_message& f1setup)
{
  gnbdu_config_update_request req;

  // Copy F1 Setup info to gNB-DU config.
  req.cells_to_mod.resize(1);
  req.cells_to_mod[0].cell_info   = du_served_cell_info{f1setup.served_cells[0].cell_info};
  req.cells_to_mod[0].du_sys_info = f1setup.served_cells[0].du_sys_info;

  // Alter SIB1
  req.cells_to_mod[0].du_sys_info->packed_sib1 = byte_buffer::create(test_rgen::random_vector<uint8_t>(100)).value();

  return req;
}

class f1ap_du_gnbdu_config_update_test : public f1ap_du_test
{
public:
  f1ap_du_gnbdu_config_update_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    this->f1c_gw.clear_tx_pdus();
  }

  const f1_setup_request_message f1setupreq = generate_f1_setup_request_message();
};

TEST_F(f1ap_du_gnbdu_config_update_test,
       when_gnbdu_config_is_started_then_du_sends_respective_initiating_message_and_waits_for_response)
{
  gnbdu_config_update_request req = create_du_config_update(f1setupreq);

  async_task<gnbdu_config_update_response>         t = this->f1ap->handle_du_config_update(req);
  lazy_task_launcher<gnbdu_config_update_response> t_launcher(t);

  // Status: DU sent gNB-DU Config Update Request.
  ASSERT_TRUE(test_helpers::is_gnb_du_config_update_valid(f1c_gw.last_tx_pdu(), req));
}

TEST_F(f1ap_du_gnbdu_config_update_test, when_du_receives_config_ack_then_procedure_completes_with_success)
{
  gnbdu_config_update_request                      req = create_du_config_update(f1setupreq);
  async_task<gnbdu_config_update_response>         t   = this->f1ap->handle_du_config_update(req);
  lazy_task_launcher<gnbdu_config_update_response> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  auto ack = test_helpers::generate_gnb_du_configuration_update_acknowledge(f1c_gw.last_tx_pdu());
  this->f1ap->handle_message(ack);

  // Status: Procedure is complete.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().result);
}

TEST_F(f1ap_du_gnbdu_config_update_test, when_du_receives_config_failure_then_procedure_completes_with_failure)
{
  gnbdu_config_update_request                      req = create_du_config_update(f1setupreq);
  async_task<gnbdu_config_update_response>         t   = this->f1ap->handle_du_config_update(req);
  lazy_task_launcher<gnbdu_config_update_response> t_launcher(t);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  auto fail = test_helpers::generate_gnb_du_configuration_update_failure(f1c_gw.last_tx_pdu());
  this->f1ap->handle_message(fail);

  // Status: Procedure is complete.
  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().result);
}
