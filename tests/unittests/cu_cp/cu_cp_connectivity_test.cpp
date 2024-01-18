/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_test_environment.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_connectivity_test : public cu_cp_test_environment, public ::testing::Test
{};

TEST_F(cu_cp_connectivity_test, when_cu_cp_is_created_then_it_is_not_connected_to_any_other_node)
{
  ngap_message ngap_pdu;
  ASSERT_FALSE(try_pop_ngap_tx_pdu(ngap_pdu))
      << "The CU-CP should not send a message to the NG interface before being started";

  e1ap_message e1ap_pdu;
  ASSERT_FALSE(try_pop_e1ap_tx_pdu(e1ap_pdu))
      << "The CU-CP should not send a message to the E1 interface before being started";
}
