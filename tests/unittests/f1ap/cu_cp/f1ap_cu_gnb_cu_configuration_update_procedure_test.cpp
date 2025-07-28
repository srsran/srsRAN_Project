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

#include "f1ap_cu_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

const std::chrono::milliseconds procedure_timeout{100};

class f1ap_cu_gnb_cu_configuration_update_test : public f1ap_cu_test
{
protected:
  f1ap_cu_gnb_cu_configuration_update_test() : f1ap_cu_test(f1ap_configuration{.proc_timeout = procedure_timeout}) {}

  void start_procedure(const f1ap_gnb_cu_configuration_update& req)
  {
    t = f1ap->handle_gnb_cu_configuration_update(req);
    t_launcher.emplace(t);
  }

  bool was_gnb_cu_configuration_update_sent(const f1ap_gnb_cu_configuration_update& req) const
  {
    if (this->f1ap_pdu_notifier.last_f1ap_msg.pdu.type().value != f1ap_pdu_c::types::init_msg) {
      return false;
    }
    if (this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.type().value !=
        asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cfg_upd) {
      return false;
    }
    const auto& asn1_req = this->f1ap_pdu_notifier.last_f1ap_msg.pdu.init_msg().value.gnb_cu_cfg_upd();

    return asn1_req->cells_to_be_activ_list.size() == req.cells_to_be_activated_list.size() &&
           asn1_req->cells_to_be_deactiv_list.size() == req.cells_to_be_deactivated_list.size();
  }

  bool was_gnb_cu_configuration_update_response_received() const
  {
    if (not t.ready()) {
      return false;
    }
    return t.get().success;
  }

  bool was_gnb_cu_configuration_update_failure_received() const
  {
    if (not t.ready()) {
      return false;
    }

    return !t.get().success;
  }

  async_task<f1ap_gnb_cu_configuration_update_response>                        t;
  std::optional<lazy_task_launcher<f1ap_gnb_cu_configuration_update_response>> t_launcher;
};

TEST_F(f1ap_cu_gnb_cu_configuration_update_test, when_request_sent_then_procedure_waits_for_response)
{
  // Start GNB CU CONFIGURATION UPDATE procedure.
  f1ap_gnb_cu_configuration_update req = create_gnb_cu_configuration_update();
  this->start_procedure(req);

  // The GNB CU CONFIGURATION UPDATE was sent to DU and F1AP-CU is waiting for response.
  ASSERT_TRUE(was_gnb_cu_configuration_update_sent(req));
  ASSERT_FALSE(t.ready());
}

TEST_F(f1ap_cu_gnb_cu_configuration_update_test, when_response_received_then_procedure_successful)
{
  // Start GNB CU CONFIGURATION UPDATE procedure and return back the response from the DU.
  f1ap_gnb_cu_configuration_update req = create_gnb_cu_configuration_update();
  this->start_procedure(req);
  f1ap_message response = test_helpers::generate_gnb_cu_configuration_update_acknowledgement({});
  f1ap->handle_message(response);

  // The GNB CU CONFIGURATION UPDATE ACKNOWLEDGEMENT was received and the F1AP-CU completed the procedure.
  ASSERT_TRUE(was_gnb_cu_configuration_update_response_received());
}

TEST_F(f1ap_cu_gnb_cu_configuration_update_test,
       when_response_with_cells_failed_to_activate_received_then_procedure_successful)
{
  // Start GNB CU CONFIGURATION UPDATE procedure and return back the response from the DU.
  f1ap_gnb_cu_configuration_update req = create_gnb_cu_configuration_update();
  this->start_procedure(req);
  f1ap_message response =
      test_helpers::generate_gnb_cu_configuration_update_acknowledgement({f1ap_cell_failed_to_activate{
          nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0).value()},
          f1ap_cause_radio_network_t::cell_not_available}});
  f1ap->handle_message(response);

  // The GNB CU CONFIGURATION UPDATE ACKNOWLEDGEMENT was received and the F1AP-CU completed the procedure.
  ASSERT_TRUE(was_gnb_cu_configuration_update_response_received());
}

TEST_F(f1ap_cu_gnb_cu_configuration_update_test, when_ue_setup_failure_received_then_procedure_unsuccessful)
{
  // Start GNB CU CONFIGURATION UPDATE procedure and return back the failure response from the DU.
  f1ap_gnb_cu_configuration_update req = create_gnb_cu_configuration_update();
  this->start_procedure(req);

  f1ap_message response = test_helpers::generate_gnb_cu_configuration_update_failure();
  f1ap->handle_message(response);

  // The GNB CU CONFIGURATION UPDATE FAILURE was received and the F1AP-CU completed the procedure with failure.
  ASSERT_TRUE(was_gnb_cu_configuration_update_failure_received());
}

TEST_F(f1ap_cu_gnb_cu_configuration_update_test, when_ue_setup_procedure_timeouts_then_procedure_unsuccessful)
{
  // Start GNB CU CONFIGURATION UPDATE procedure and return back the failure response from the DU.
  f1ap_gnb_cu_configuration_update req = create_gnb_cu_configuration_update();
  this->start_procedure(req);

  for (unsigned i = 0; i != procedure_timeout.count(); ++i) {
    ASSERT_FALSE(was_gnb_cu_configuration_update_failure_received());
    this->tick();
  }
  ASSERT_TRUE(was_gnb_cu_configuration_update_failure_received());
}
