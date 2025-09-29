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
#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "srsran/support/async/async_then.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

static du_high_env_sim_params create_few_ues_config()
{
  // Reduce number of PUCCH resources, so we do not have to create so many UEs to reach the saturation point.
  du_high_env_sim_params params;
  params.builder_params = cell_config_builder_profiles::tdd();
  params.builder_params.value().tdd_ul_dl_cfg_common =
      tdd_ul_dl_config_common{subcarrier_spacing::kHz30, {10, 8, 5, 1, 4}};
  params.pucch_cfg.emplace();
  params.pucch_cfg->nof_ue_pucch_f0_or_f1_res_harq       = 8;
  params.pucch_cfg->nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8;
  params.pucch_cfg->nof_sr_resources                     = 1;
  params.prach_frequency_start.emplace(3U);
  return params;
}

static du_high_env_sim_params create_many_ues_config()
{
  du_high_env_sim_params params;
  params.builder_params = cell_config_builder_profiles::tdd();
  params.pucch_cfg.emplace();
  params.pucch_cfg->nof_ue_pucch_f0_or_f1_res_harq       = 8;
  params.pucch_cfg->nof_ue_pucch_f2_or_f3_or_f4_res_harq = 8;
  params.pucch_cfg->nof_sr_resources                     = 4;
  params.pucch_cfg->nof_csi_resources                    = 4;
  auto& f1_params                                        = params.pucch_cfg->f0_or_f1_params.emplace<pucch_f1_params>();
  f1_params.nof_cyc_shifts                               = pucch_nof_cyclic_shifts::twelve;
  f1_params.occ_supported                                = true;
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
  du_high_many_ues_tester() : du_high_env_simulator(create_many_ues_config())
  {
    // Reset the last sent F1AP PDU (e.g. F1 setup).
    cu_notifier.f1ap_ul_msgs.clear();
  }

  uint16_t next_rnti = 0x4601;
};

TEST_F(du_high_many_ues_tester, when_many_ues_are_created_concurrently_then_ues_complete_conres_and_rrc_setup)
{
  const unsigned max_ues  = 30;
  unsigned       ue_count = 0;

  // Launch async tasks for UE creation and RRC setup.
  for (; ue_count != max_ues; ++ue_count) {
    rnti_t rnti             = to_rnti(next_rnti++);
    auto   ue_creation_task = launch_ue_creation_task(rnti, to_du_cell_index(0), true);
    auto   rrc_setup_task   = launch_rrc_setup_task(rnti, true);
    this->schedule_task(async_then(std::move(ue_creation_task), std::move(rrc_setup_task)));
  }

  // Await completion of all tasks.
  this->run_until_all_pending_tasks_completion();
}

class du_high_few_ues_test : public du_high_env_simulator, public testing::Test
{
protected:
  du_high_few_ues_test() : du_high_env_simulator(create_few_ues_config())
  {
    // Reset the last sent F1AP PDU (e.g. F1 setup).
    cu_notifier.f1ap_ul_msgs.clear();
  }

  uint16_t next_rnti = 0x4601;
};

TEST_F(du_high_few_ues_test, when_du_runs_out_of_resources_then_ues_start_being_rejected)
{
  unsigned       ue_count          = 0;
  const unsigned ue_count_limit    = 15;
  auto           sched_ue_creation = [this]() {
    rnti_t rnti             = to_rnti(next_rnti++);
    auto   ue_creation_task = launch_ue_creation_task(rnti, to_du_cell_index(0), true);
    auto   rrc_setup_task   = launch_rrc_setup_task(rnti, false);
    this->schedule_task(async_then(std::move(ue_creation_task), std::move(rrc_setup_task)));
  };

  for (; ue_count != ue_count_limit; ++ue_count) {
    sched_ue_creation();
  }
  this->run_until_all_pending_tasks_completion();

  ASSERT_GT(ue_count, 5) << "The number of UEs accepted by DU was too low";
  ASSERT_LT(this->ues.size(), ue_count_limit)
      << "The DU is accepting UEs past its configured number of PUCCH resources";

  // If we try to add more UEs, they also fail.
  unsigned nof_ues = this->ues.size();
  sched_ue_creation();
  this->run_until_all_pending_tasks_completion();
  ASSERT_EQ(this->ues.count(to_rnti(next_rnti - 1)), 0) << "UE got accepted even if DU is out of resources";
  ASSERT_EQ(this->ues.size(), nof_ues);

  // Once we remove a UE, we have space again for another UE.
  ASSERT_TRUE(this->run_ue_context_release(to_rnti(0x4601)));
  ASSERT_EQ(this->ues.size(), nof_ues - 1);
  sched_ue_creation();
  this->run_until_all_pending_tasks_completion();
  ASSERT_EQ(this->ues.count(to_rnti(next_rnti - 1)), 1) << "UE did not get accepted even if DU now has resources";
  ASSERT_EQ(this->ues.size(), nof_ues);
}
