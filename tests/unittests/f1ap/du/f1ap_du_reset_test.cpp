/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_test_helpers.h"
#include "lib/f1ap/du/procedures/f1ap_du_reset_procedure.h"
#include "test_doubles/f1ap/f1ap_test_message_validators.h"
#include "test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class f1_reset_test : public f1ap_du_test
{
public:
  f1_reset_test()
  {
    // Test Preamble.
    run_f1_setup_procedure();

    this->f1c_gw.clear_tx_pdus();
  }
};

TEST_F(f1_reset_test, when_f1_reset_is_received_but_no_ue_is_created_then_no_request_is_forwarded_to_du)
{
  auto req = test_helpers::create_f1ap_reset_message();
  this->f1ap->handle_message(req);

  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());

  ASSERT_TRUE(test_helpers::is_valid_f1_reset_ack(req, this->f1c_gw.last_tx_pdu()));
}

TEST_F(f1_reset_test, when_f1_reset_is_received_then_reset_request_is_forwarded_to_du)
{
  du_ue_index_t test_ue_index = to_du_ue_index(0);
  this->run_f1ap_ue_create(test_ue_index);

  auto req = test_helpers::create_f1ap_reset_message();
  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.value().empty());

  ASSERT_TRUE(test_helpers::is_valid_f1_reset_ack(req, this->f1c_gw.last_tx_pdu()));
}

TEST_F(f1_reset_test, when_f1_reset_is_received_for_a_specific_ue_then_reset_request_is_forwarded_to_du_with_ue)
{
  du_ue_index_t test_ue_index = to_du_ue_index(0);
  this->run_f1ap_ue_create(test_ue_index);

  auto req = test_helpers::create_f1ap_reset_message({{gnb_du_ue_f1ap_id_t{0}, std::nullopt}});
  this->f1ap->handle_message(req);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ues_to_reset.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ues_to_reset.value().size(), 1);
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ues_to_reset.value()[0], test_ue_index);

  ASSERT_TRUE(test_helpers::is_valid_f1_reset_ack(req, this->f1c_gw.last_tx_pdu()));
}
