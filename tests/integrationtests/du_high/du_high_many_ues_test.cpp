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

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

static du_high_env_sim_params create_custom_params()
{
  du_high_env_sim_params params;
  // Reduce number of PUCCH resources, so we do not have to create so many UEs to reach the saturation point.
  params.builder_params.emplace();
  params.builder_params.value().scs_common     = subcarrier_spacing::kHz30;
  params.builder_params.value().dl_f_ref_arfcn = 520002;
  params.builder_params.value().band =
      band_helper::get_band_from_dl_arfcn(params.builder_params.value().dl_f_ref_arfcn);
  params.builder_params.value().tdd_ul_dl_cfg_common =
      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}};
  params.pucch_cfg.emplace();
  params.pucch_cfg->nof_ue_pucch_f0_or_f1_res_harq       = 8;
  params.pucch_cfg->nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8;
  params.pucch_cfg->nof_sr_resources                     = 1;
  // Set the PRACH frequency start to avoid PRACH collisions with the PUCCH on the upper RBs of the BWP (this would
  // trigger an error and abort the test).
  // NOTE: this results in the PRACH overlapping with the PUCCH resources on the lower RBs of the BWP, but it doesn't
  // trigger any error, as it the parameter was user-defined (which skips the validator for prach_frequency_start).
  params.prach_frequency_start.emplace(3U);
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
    rnti_t rnti = to_rnti(next_rnti++);
    ASSERT_TRUE(this->add_ue(rnti));

    byte_buffer container = test_helpers::get_du_to_cu_container(cu_notifier.last_f1ap_msgs.back());
    if (container.empty()) {
      // When DU-to-CU container is empty, it means that the DU could not allocate resources for the UE.

      test_logger.info("rnti={}: UE rejected due to lack of DU resources", rnti);
      ASSERT_TRUE(this->run_rrc_reject(rnti)) << "RRC Reject not scheduled";

      break;
    }

    ASSERT_TRUE(this->run_rrc_setup(rnti));
  }

  ASSERT_GT(ue_count, 5) << "The number of UEs accepted by DU was too low";
  ASSERT_LT(ue_count, MAX_NOF_DU_UES) << "The DU is accepting UEs past its number of PUCCH resources";

  // If we try to add more UEs, they also fail.
  ASSERT_TRUE(this->add_ue(to_rnti(next_rnti++)));
  byte_buffer container = test_helpers::get_du_to_cu_container(cu_notifier.last_f1ap_msgs.back());
  ASSERT_TRUE(container.empty()) << "No more UEs are accepted after the DU runs out of resources";

  // Once we remove a UE, we have space again for another UE.
  ASSERT_TRUE(this->run_ue_context_release(to_rnti(0x4601)));
  ASSERT_TRUE(this->add_ue(to_rnti(next_rnti++)));
  container = test_helpers::get_du_to_cu_container(cu_notifier.last_f1ap_msgs.back());
  ASSERT_FALSE(container.empty()) << "The resources of the released UE were not correctly cleaned up";
}
