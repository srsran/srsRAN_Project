/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/f1ap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class du_high_connection_test : public du_high_env_simulator, public testing::Test
{
protected:
  du_high_connection_test()
  {
    // Reset the last sent F1AP PDU (e.g. F1 setup).
    cu_notifier.f1ap_ul_msgs.clear();
  }
};

TEST_F(du_high_connection_test, when_f1_connection_is_lost_then_du_detects_connection_loss)
{
  ASSERT_FALSE(cu_notifier.du_released_connection());

  // Trigger a disconnection.
  cu_notifier.set_f1_channel_state(false);

  // DU releases its F1 connection.
  ASSERT_FALSE(cu_notifier.du_released_connection());
  workers.flush_pending_control_tasks();
  ASSERT_TRUE(cu_notifier.du_released_connection());
}

TEST_F(du_high_connection_test, when_f1_connection_is_lost_then_du_retries_new_connection)
{
  // Signal a temporary F1 connection loss. The DU should retry the connection.
  cu_notifier.set_f1_channel_state(false);
  cu_notifier.set_f1_channel_state(true);

  ASSERT_TRUE(cu_notifier.f1ap_ul_msgs.empty());
  workers.flush_pending_control_tasks();

  // F1 setup was initiated.
  ASSERT_FALSE(cu_notifier.f1ap_ul_msgs.empty());
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}
