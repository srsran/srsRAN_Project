/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_test_helpers.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class f1ap_du_gnbcu_config_update_test : public f1ap_du_test
{
public:
  f1ap_du_gnbcu_config_update_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();
    this->f1c_gw.clear_tx_pdus();
  }

  const f1_setup_request_message f1setupreq = generate_f1_setup_request_message();
};

TEST_F(f1ap_du_gnbcu_config_update_test, when_gnbcu_config_request_is_received_then_du_receives_cells_to_activate)
{
  // DU receives GNB-CU Config Update Request.
  std::vector<nr_cell_global_id_t> cgis = {
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(1).value()}};
  f1ap_message msg = test_helpers::create_gnb_cu_configuration_update_request(0, cgis);
  this->f1ap->handle_message(msg);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_cu_upd_req.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate.size(), 1);
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_cu_upd_req.value().cells_to_activate[0].cgi, cgis[0]);

  ASSERT_TRUE(test_helpers::is_gnb_cu_config_update_acknowledge_valid(this->f1c_gw.last_tx_pdu(), msg));
}
