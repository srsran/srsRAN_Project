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
#include "srsran/f1ap/f1ap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class du_high_connection_test : public du_high_env_simulator, public testing::Test
{
protected:
  du_high_connection_test()
  {
    // Reset the last sent F1AP PDU (e.g. F1 setup).
    cu_notifier.f1ap_ul_msgs.clear();
  }
};

TEST_F(du_high_connection_test, when_cu_disconnects_then_du_detects_connection_loss)
{
  ASSERT_TRUE(cu_notifier.du_released_connection());

  // Trigger a disconnection.
  cu_notifier.on_cu_disconnection();

  // DU releases its F1 connection.
  ASSERT_TRUE(cu_notifier.du_released_connection());
  workers.flush_pending_control_tasks();
  ASSERT_FALSE(cu_notifier.du_released_connection());
}
