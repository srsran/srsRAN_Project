/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class du_high_many_ues_tester : public du_high_env_simulator, public testing::Test
{
protected:
  du_high_many_ues_tester()
  {
    // Reset the last sent F1AP PDU (e.g. F1 setup).
    cu_notifier.last_f1ap_msgs.clear();
  }

  uint16_t next_rnti = 0x4601;
};

TEST_F(du_high_many_ues_tester, when_du_runs_out_of_resources_then_ues_stop_being_created)
{
  unsigned added_ues = 0;

  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    bool added = this->add_ue(to_rnti(next_rnti));
    if (not added) {
      break;
    }
    added_ues++;
    next_rnti++;
  }

  ASSERT_GT(added_ues, 75) << "The number of UEs created by DU was too low";

  ASSERT_FALSE(this->add_ue(to_rnti(next_rnti))) << "No more UEs can be added after the DU runs out of resources";
}
