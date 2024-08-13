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
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

static du_high_env_sim_params create_custom_params()
{
  du_high_env_sim_params params;
  // Reduce number of PUCCH resources, so we do not have to create so many UEs to reach the saturation point.
  params.pucch_cfg.emplace();
  params.pucch_cfg->nof_ue_pucch_f0_or_f1_res_harq = 8;
  params.pucch_cfg->nof_ue_pucch_f2_res_harq       = 8;
  params.pucch_cfg->nof_sr_resources               = 1;
  return params;
}

class du_high_many_ues_tester : public du_high_env_simulator, public testing::Test
{
protected:
  du_high_many_ues_tester() : du_high_env_simulator(create_custom_params())
  {
    // Reset the last sent F1AP PDU (e.g. F1 setup).
    cu_notifier.last_f1ap_msgs.clear();
  }

  uint16_t next_rnti = 0x4601;
};

TEST_F(du_high_many_ues_tester, when_du_runs_out_of_resources_then_ues_start_being_rejected)
{
  unsigned ue_count = 0;
  for (; ue_count != MAX_NOF_DU_UES; ++ue_count) {
    ASSERT_TRUE(this->add_ue(to_rnti(next_rnti)));

    byte_buffer container = test_helpers::get_du_to_cu_container(cu_notifier.last_f1ap_msgs.back());
    if (container.empty()) {
      break;
    }

    next_rnti++;
  }

  ASSERT_GT(ue_count, 30) << "The number of UEs accepted by DU was too low";
  ASSERT_LT(ue_count, MAX_NOF_DU_UES) << "The DU is accepting UEs past its number of PUCCH resources";

  next_rnti++;
  ASSERT_TRUE(this->add_ue(to_rnti(next_rnti)));
  byte_buffer container = test_helpers::get_du_to_cu_container(cu_notifier.last_f1ap_msgs.back());
  ASSERT_TRUE(container.empty()) << "No more UEs are accepted after the DU runs out of resources";
}
