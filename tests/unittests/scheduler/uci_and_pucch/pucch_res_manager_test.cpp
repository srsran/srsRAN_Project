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

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "uci_test_utils.h"
#include "srsran/scheduler/config/pucch_resource_generator.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace srsran;

////////////    Test the PUCCH resource manager: all UEs sharing the same config     ////////////

static serving_cell_config create_initial_ue_serving_cell_config_with_csi()
{
  cell_config_builder_params cfg_params{};
  cfg_params.csi_rs_enabled = true;
  serving_cell_config cfg   = config_helpers::create_default_initial_ue_serving_cell_config(cfg_params);
  cfg.ul_config             = test_helpers::make_test_ue_uplink_config(cfg_params);
  return cfg;
}

class test_pucch_resource_manager : public ::testing::Test
{
public:
  test_pucch_resource_manager() :
    // TODO: when the CSI is enabled in the main config, replace create_initial_ue_serving_cell_config_with_csi() with
    //       config_helpers::create_default_initial_ue_serving_cell_config().
    ue_cell_cfg(to_rnti(0x4601), cell_cfg, cell_cfg_pool.update_ue(ue_create_req)),
    pucch_cfg{ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value()},
    sl_tx(slot_point(0, 0))
  {
    uplink_config ul_cfg = test_helpers::make_test_ue_uplink_config(cell_config_builder_params{});
    ul_cfg.init_ul_bwp.pucch_cfg.value().sr_res_list.front().pucch_res_id = pucch_res_id_t{10, 10};
    pucch_cfg_ue_2                                                        = ul_cfg.init_ul_bwp.pucch_cfg.value();
    res_manager.slot_indication(sl_tx);
  }

protected:
  const scheduler_expert_config                  sched_cfg = config_helpers::make_default_scheduler_expert_config();
  const sched_cell_configuration_request_message cell_req =
      sched_config_helper::make_default_sched_cell_configuration_request();
  const serving_cell_config ue_create_req = create_initial_ue_serving_cell_config_with_csi();
  du_cell_config_pool       cell_cfg_pool{cell_req};
  cell_configuration        cell_cfg{sched_cfg, cell_req};
  ue_cell_configuration     ue_cell_cfg;
  // Helper variable.
  const pucch_config& pucch_cfg;
  // Config with alternative configuration for SR using PUCCH resource idx 10.
  pucch_config           pucch_cfg_ue_2;
  pucch_resource_manager res_manager;
  slot_point             sl_tx;

  // Allocate PUCCH for a given number of UEs, in increasing order of RNTI.
  void allocate_ues(unsigned nof_ues_to_allocate, bool format_2 = false)
  {
    if (not format_2) {
      for (size_t n = 0; n != nof_ues_to_allocate; ++n) {
        const rnti_t rnti = to_rnti(0x4601 + n);
        res_manager.reserve_next_set_0_harq_res_available(sl_tx, rnti, pucch_cfg);
      }
    } else {
      for (size_t n = 0; n != nof_ues_to_allocate; ++n) {
        const rnti_t rnti = to_rnti(0x4601 + n);
        res_manager.reserve_next_set_1_harq_res_available(sl_tx, rnti, pucch_cfg);
      }
    }
  }
};

TEST_F(test_pucch_resource_manager, common_res_available_intialization)
{
  for (unsigned r_pucch = 0; r_pucch != 16; ++r_pucch) {
    ASSERT_TRUE(res_manager.is_common_resource_available(sl_tx, r_pucch));
  }
}

TEST_F(test_pucch_resource_manager, common_res_available_reserve_and_check)
{
  const unsigned r_pucch = 13;
  res_manager.reserve_common_resource(sl_tx, r_pucch);
  for (unsigned r_pucch_it = 0; r_pucch_it != 16; ++r_pucch_it) {
    if (r_pucch_it == r_pucch) {
      continue;
    }
    ASSERT_TRUE(res_manager.is_common_resource_available(sl_tx, r_pucch_it));
  }

  // Increment slot point and invoke slot_indication(), which should reset the previous UE's resource allocation.
  ++sl_tx;
  res_manager.slot_indication(sl_tx);
  for (unsigned r_pucch_it = 0; r_pucch_it != 16; ++r_pucch_it) {
    ASSERT_TRUE(res_manager.is_common_resource_available(sl_tx, r_pucch_it));
  }
}

TEST_F(test_pucch_resource_manager, get_available_f1_with_1_ue_only)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_available_f1_with_2_ues)
{
  allocate_ues(1);
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  ASSERT_EQ(1, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[1], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_available_f1_with_3_ues)
{
  allocate_ues(2);
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  ASSERT_EQ(2, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[2], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_available_f1_with_4_ues)
{
  allocate_ues(3);
  // Attempt to allocate the PUCCH resource to the 4th UE.
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4604), pucch_cfg);

  ASSERT_EQ(nullptr, record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_different_slot)
{
  allocate_ues(1);
  ++sl_tx;
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_f1)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, to_rnti(0x4601), pucch_cfg));

  // Re-allocate the resource.
  const pucch_harq_resource_alloc_record reallocation =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0, reallocation.pucch_res_indicator);
  ASSERT_EQ(record.pucch_res, reallocation.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_multiple_ues)
{
  // Allocate 3 UEs.
  allocate_ues(3);

  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, to_rnti(0x4603), pucch_cfg));

  // Re-allocate the resources to UE1 and UE3.
  const pucch_harq_resource_alloc_record realloc_ue1 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  const pucch_harq_resource_alloc_record realloc_ue3 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  ASSERT_EQ(0, realloc_ue1.pucch_res_indicator);
  ASSERT_EQ(2, realloc_ue3.pucch_res_indicator);
}

TEST_F(test_pucch_resource_manager, allocate_resources_f2_1_ue)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  const unsigned res_idx_from_list =
      pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator].cell_res_id;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_resources_f2_2_ues)
{
  allocate_ues(1, /* format_2= */ true);
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  ASSERT_EQ(1, record.pucch_res_indicator);
  const unsigned res_idx_from_list =
      pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator].cell_res_id;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_resources_f2_3_ues)
{
  allocate_ues(2, /* format_2= */ true);
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  ASSERT_EQ(2, record.pucch_res_indicator);
  const unsigned res_idx_from_list =
      pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator].cell_res_id;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_resources_f2_8_ues)
{
  allocate_ues(7, /* format_2= */ true);
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4608), pucch_cfg);

  // Expect the 8th UE not to get assigned anything, as there are only 7 resources available.
  ASSERT_EQ(nullptr, record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_csi_resource)
{
  const unsigned        expected_csi_res_index = 9;
  const pucch_resource* res                    = res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  ASSERT_EQ(&pucch_cfg.pucch_res_list[expected_csi_res_index], res);
}

TEST_F(test_pucch_resource_manager, release_and_reallocate_csi_resource)
{
  // There is no allocated resource, expects false from the release.
  ASSERT_FALSE(res_manager.release_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg));

  const unsigned        expected_csi_res_index = 9;
  const pucch_resource* res                    = res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[expected_csi_res_index], res);

  const pucch_resource* res_second_allc_no_release =
      res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg);
  ASSERT_EQ(res, res_second_allc_no_release);

  // This time the release it supposed to return true.
  ASSERT_TRUE(res_manager.release_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg));

  const pucch_resource* res_reallocation = res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[expected_csi_res_index], res_reallocation);
}

TEST_F(test_pucch_resource_manager, get_format2_different_slot)
{
  allocate_ues(1);
  ++sl_tx;
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0, record.pucch_res_indicator);
  const unsigned res_idx_from_list =
      pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator].cell_res_id;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_f2)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  unsigned res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator].cell_res_id;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, to_rnti(0x4601), pucch_cfg));

  // Re-allocate the resource.
  const pucch_harq_resource_alloc_record reallocation =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0, reallocation.pucch_res_indicator);
  res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator].cell_res_id;
  ASSERT_EQ(record.pucch_res, reallocation.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_and_release_f2_multiple_ues)
{
  // Allocate 6 UEs.
  allocate_ues(6, /* format_2*/ true);

  // Release the resource and verify that the UEs do not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, to_rnti(0x4603), pucch_cfg));
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, to_rnti(0x4606), pucch_cfg));

  // Re-allocate the resources to UE1, UE3, UE6.
  const pucch_harq_resource_alloc_record realloc_ue1 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  const pucch_harq_resource_alloc_record realloc_ue3 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);
  const pucch_harq_resource_alloc_record realloc_ue6 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4606), pucch_cfg);

  // Check whether the UEs get returned (again) the corresponding PUCCH resource indicator.
  ASSERT_EQ(0, realloc_ue1.pucch_res_indicator);
  ASSERT_EQ(2, realloc_ue3.pucch_res_indicator);
  ASSERT_EQ(5, realloc_ue6.pucch_res_indicator);
}

TEST_F(test_pucch_resource_manager, test_allocation_sr_resource)
{
  const pucch_resource* sr_resource = res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  const unsigned sr_pucch_res_idx = pucch_cfg.pucch_res_list.size() - 1;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[sr_pucch_res_idx], sr_resource);
}

TEST_F(test_pucch_resource_manager, test_allocation_release_sr_resource)
{
  res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_sr_resource(sl_tx, to_rnti(0x4601), pucch_cfg));

  // Allocate SR to another UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue2 = res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);
  ASSERT_FALSE(nullptr == sr_resource_ue2);
  const unsigned sr_pucch_res_idx = pucch_cfg.pucch_res_list.size() - 1;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[sr_pucch_res_idx], sr_resource_ue2);
}

TEST_F(test_pucch_resource_manager, test_allocation_2_sr_resource)
{
  res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4602), pucch_cfg_ue_2);

  // Attempt to allocate UEs over already used SR resources.
  ASSERT_EQ(nullptr, res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4603), pucch_cfg));
  ASSERT_EQ(nullptr, res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4604), pucch_cfg_ue_2));

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_sr_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_TRUE(res_manager.release_sr_resource(sl_tx, to_rnti(0x4602), pucch_cfg_ue_2));

  // Attempt to allocate UEs over released SR resources.
  ASSERT_NE(nullptr, res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4603), pucch_cfg));
  ASSERT_NE(nullptr, res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4604), pucch_cfg_ue_2));
}

TEST_F(test_pucch_resource_manager, test_allocation_specific_f1)
{
  const unsigned res_indicator = 2;

  // Attempt to allocate PUCCH resource Format 2 with given resource indicator.
  ASSERT_TRUE(nullptr !=
              res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4601), res_indicator, pucch_cfg));

  // Attempt to allocate another UE to the same resource and verify it gets returned nullptr.
  ASSERT_TRUE(nullptr ==
              res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4602), res_indicator, pucch_cfg));

  // Attempt to allocate a third UE with wrong resource indicator and verify it gets returned nullptr.
  ASSERT_TRUE(nullptr ==
              res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4603), res_indicator, pucch_cfg));
}

TEST_F(test_pucch_resource_manager, test_allocation_specific_f2)
{
  const unsigned res_indicator = 3;

  // Attempt to allocate PUCCH resource Format 2 with given resource indicator.
  ASSERT_TRUE(nullptr !=
              res_manager.reserve_set_1_res_by_res_indicator(sl_tx, to_rnti(0x4601), res_indicator, pucch_cfg));

  // Attempt to allocate another UE to the same resource and verify it gets returned nullptr.
  ASSERT_TRUE(nullptr ==
              res_manager.reserve_set_1_res_by_res_indicator(sl_tx, to_rnti(0x4602), res_indicator, pucch_cfg));

  // Attempt to allocate a third UE with wrong resource indicator and verify it gets returned nullptr.
  ASSERT_TRUE(nullptr ==
              res_manager.reserve_set_1_res_by_res_indicator(sl_tx, to_rnti(0x4603), res_indicator, pucch_cfg));
}

TEST_F(test_pucch_resource_manager, cancel_last_ue_res_reservations_when_nothing_was_reserved_should_not_assert)
{
  res_manager.reset_latest_reserved_res_tracker();

  // Only expects that it doesn't assert.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_specific_resources)
{
  res_manager.reset_latest_reserved_res_tracker();

  auto alloc = res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  res_manager.set_new_resource_allocation(to_rnti(0x4601), srsran::pucch_resource_usage::HARQ_SET_0);

  // Try to allocate a new UE using the PUCCH resource indicator assigned to UE 0x4601; it should fail.
  ASSERT_EQ(
      nullptr,
      res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4602), alloc.pucch_res_indicator, pucch_cfg));

  // Release the resources of the first UE.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  // Try to allocate the new UE again, now it should succeed.
  ASSERT_NE(
      nullptr,
      res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4602), alloc.pucch_res_indicator, pucch_cfg));

  alloc = res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  res_manager.set_new_resource_allocation(to_rnti(0x4601), srsran::pucch_resource_usage::HARQ_SET_1);

  // Try to allocate a new UE using the PUCCH resource indicator assigned to UE 0x4601; it should fail.
  ASSERT_EQ(
      nullptr,
      res_manager.reserve_set_1_res_by_res_indicator(sl_tx, to_rnti(0x4602), alloc.pucch_res_indicator, pucch_cfg));

  // Release the resources of the first UE.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  // Try to allocate the new UE again, now it should succeed.
  ASSERT_NE(
      nullptr,
      res_manager.reserve_set_1_res_by_res_indicator(sl_tx, to_rnti(0x4602), alloc.pucch_res_indicator, pucch_cfg));
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_harq_resources_different_sets)
{
  res_manager.reset_latest_reserved_res_tracker();

  auto alloc = res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  res_manager.set_new_resource_allocation(to_rnti(0x4601), srsran::pucch_resource_usage::HARQ_SET_0);

  // Don't set this reservation in the tracker, as we want to preserve it.
  alloc = res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  // Try to allocate a new UE using the PUCCH resource indicator assigned to UE 0x4601; it should fail.
  ASSERT_EQ(
      nullptr,
      res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4602), alloc.pucch_res_indicator, pucch_cfg));

  // Release the resources of the first UE.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  // Release the tracked resources of the first UE.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  // Try to allocate the new UE again, it should succeed.
  ASSERT_NE(nullptr, res_manager.reserve_set_0_res_by_res_indicator(sl_tx, to_rnti(0x4602), 0U, pucch_cfg));
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_harq_and_csi)
{
  res_manager.reset_latest_reserved_res_tracker();

  auto alloc = res_manager.reserve_next_set_1_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  // Don't set this reservation in the tracker, as we want to preserve this resource.
  res_manager.set_new_resource_allocation(to_rnti(0x4601), srsran::pucch_resource_usage::CSI);

  const auto* csi_resource = res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4601), ue_cell_cfg);
  ASSERT_NE(nullptr, csi_resource);

  const auto* csi_resource_1 = res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4602), ue_cell_cfg);
  ASSERT_EQ(nullptr, csi_resource_1);

  // Release the tracked resources of the first UE.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  csi_resource_1 = res_manager.reserve_csi_resource(sl_tx, to_rnti(0x4602), ue_cell_cfg);
  ASSERT_NE(nullptr, csi_resource_1);
}

TEST_F(test_pucch_resource_manager, test_cancel_last_ue_res_reservations_for_harq_and_sr)
{
  res_manager.reset_latest_reserved_res_tracker();

  // Don't set this reservation in the tracker, as we want to preserve this resource.
  auto alloc = res_manager.reserve_next_set_0_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0U, alloc.pucch_res_indicator);

  const auto* sr_resource = res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_NE(nullptr, sr_resource);

  res_manager.set_new_resource_allocation(to_rnti(0x4601), srsran::pucch_resource_usage::SR);

  const auto* sr_resource_1 = res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);
  ASSERT_EQ(nullptr, sr_resource_1);

  // Release the tracked resources of the first UE.
  res_manager.cancel_last_ue_res_reservations(sl_tx, to_rnti(0x4601), ue_cell_cfg);

  sr_resource_1 = res_manager.reserve_sr_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);
  ASSERT_NE(nullptr, sr_resource_1);
}

////////////    Test the PUCCH resource manager: UEs with different configs     ////////////

class test_pucch_res_manager_multiple_cfg : public test_pucch_resource_manager
{
public:
  test_pucch_res_manager_multiple_cfg() = default;

protected:
  struct dummy_ue {
    dummy_ue(rnti_t rnti, const cell_configuration& cell_cfg_common_, ue_cell_config_ptr ue_cell_cfg_) :
      cnrti{rnti}, ue_cell_cfg{rnti, cell_cfg_common_, ue_cell_cfg_}
    {
    }

    [[nodiscard]] const pucch_config& get_pucch_cfg() const { return ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value(); }

    rnti_t                cnrti;
    ue_cell_configuration ue_cell_cfg;
  };

  void generate_ue_serv_cell_cfg(serving_cell_config&         serv_cell_cfg,
                                 unsigned                     ue_idx,
                                 unsigned                     nof_pucch_cfgs,
                                 std::vector<pucch_resource>& cell_res_list) const
  {
    const unsigned nof_ue_pucch_f1_res_harq = nof_res_per_ue;
    const unsigned nof_ue_pucch_f2_res_harq = nof_res_per_ue;
    const unsigned nof_ue_pucch_f1_res_sr   = 1;
    const unsigned nof_ue_pucch_f2_res_csi  = 1;

    pucch_config& default_pucch_cfg = serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();

    // Clears current PUCCH resource list and PUCCH resource list set 0 and 1.
    default_pucch_cfg.pucch_res_list.clear();
    default_pucch_cfg.pucch_res_set[0].pucch_res_id_list.clear();
    default_pucch_cfg.pucch_res_set[1].pucch_res_id_list.clear();

    // Ensure the PUCCH resource sets ID are 0 and 1.
    default_pucch_cfg.pucch_res_set[0].pucch_res_set_id = srsran::pucch_res_set_idx::set_0;
    default_pucch_cfg.pucch_res_set[1].pucch_res_set_id = srsran::pucch_res_set_idx::set_1;

    const unsigned tot_ue_f1_res = nof_ue_pucch_f1_res_harq + nof_ue_pucch_f1_res_sr;
    const unsigned tot_ue_f2_res = nof_ue_pucch_f2_res_harq + nof_ue_pucch_f2_res_csi;

    const unsigned f1_idx_offset = (ue_idx % nof_pucch_cfgs) * tot_ue_f1_res;

    // Add F1 for HARQ and SR.
    for (unsigned ue_f1_cnt = 0; ue_f1_cnt < tot_ue_f1_res; ++ue_f1_cnt) {
      const auto& cell_res = cell_res_list[ue_f1_cnt + f1_idx_offset];

      // Add PUCCH resource to pucch_res_list.
      default_pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id         = cell_res.res_id,
                                                                   .starting_prb   = cell_res.starting_prb,
                                                                   .second_hop_prb = cell_res.second_hop_prb,
                                                                   .format         = cell_res.format,
                                                                   .format_params  = cell_res.format_params});

      // Add PUCCH resource index to pucch_res_id_list of PUCCH resource set id=0.
      if (ue_f1_cnt < nof_ue_pucch_f1_res_harq) {
        default_pucch_cfg.pucch_res_set[0].pucch_res_id_list.emplace_back(cell_res.res_id);
      } else {
        // Configure SR to point to the correct PUCCH resource.
        default_pucch_cfg.sr_res_list.front().pucch_res_id = cell_res.res_id;
      }
    }

    const unsigned f2_idx_offset = tot_ue_f1_res * nof_pucch_cfgs + (ue_idx % nof_pucch_cfgs) * tot_ue_f2_res;
    auto&          csi_cfg       = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                        serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.front().report_cfg_type)
                        .pucch_csi_res_list.front();

    // Add F2 for HARQ and CSI.
    for (unsigned ue_f2_cnt = 0; ue_f2_cnt < tot_ue_f2_res; ++ue_f2_cnt) {
      const auto& cell_res = cell_res_list[f2_idx_offset + ue_f2_cnt];
      default_pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id         = cell_res.res_id,
                                                                   .starting_prb   = cell_res.starting_prb,
                                                                   .second_hop_prb = cell_res.second_hop_prb,
                                                                   .format         = cell_res.format,
                                                                   .format_params  = cell_res.format_params});

      // Add PUCCH resource index to pucch_res_id_list of PUCCH resource set id=1.
      if (ue_f2_cnt < nof_ue_pucch_f2_res_harq) {
        default_pucch_cfg.pucch_res_set[1].pucch_res_id_list.emplace_back(cell_res.res_id);
      } else {
        // Configure SR to point to the correct PUCCH resource.
        csi_cfg.pucch_res_id = cell_res.res_id;
      }
    }
  }

  void create_ues(unsigned nof_ues, unsigned nof_res_per_ue_, unsigned nof_configurations)
  {
    nof_res_per_ue = nof_res_per_ue_;
    cell_pucch_res_list =
        config_helpers::generate_cell_pucch_res_list((nof_res_per_ue + 1) * nof_configurations,
                                                     (nof_res_per_ue + 1) * nof_configurations,
                                                     pucch_f1_params{},
                                                     pucch_f2_params{},
                                                     cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(),
                                                     NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    for (unsigned ue_idx = 0; ue_idx != nof_ues; ++ue_idx) {
      sched_ue_creation_request_message ue_req   = sched_config_helper::create_default_sched_ue_creation_request();
      serving_cell_config&              serv_cfg = ue_req.cfg.cells->front().serv_cell_cfg;
      generate_ue_serv_cell_cfg(serv_cfg, ue_idx, nof_configurations, cell_pucch_res_list);
      ues.emplace_back(
          std::make_unique<dummy_ue>(to_rnti(0x4601 + ue_idx), cell_cfg, cell_cfg_pool.update_ue(serv_cfg)));
    }
  }

  void allocate_f1_specific_ue(unsigned ue_idx)
  {
    res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_idx]->cnrti, ues[ue_idx]->get_pucch_cfg());
  }

  void allocate_f2_specific_ue(unsigned ue_idx)
  {
    res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_idx]->cnrti, ues[ue_idx]->get_pucch_cfg());
  }

  unsigned                               nof_res_per_ue{8};
  std::vector<pucch_resource>            cell_pucch_res_list;
  std::vector<std::unique_ptr<dummy_ue>> ues;
};

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_f1_only)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const unsigned                         ue_0_idx = 0;
  const pucch_harq_resource_alloc_record record_ue_0 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_0_idx]->cnrti, ues[ue_0_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_0.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_0_idx]->get_pucch_cfg().pucch_res_list[0], record_ue_0.pucch_res);
  ASSERT_EQ(0, record_ue_0.pucch_res->res_id.cell_res_id);

  const unsigned                         ue_1_idx = 1;
  const pucch_harq_resource_alloc_record record_ue_1 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_1_idx]->cnrti, ues[ue_1_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_1.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_1_idx]->get_pucch_cfg().pucch_res_list[0], record_ue_1.pucch_res);
  ASSERT_EQ(9, record_ue_1.pucch_res->res_id.cell_res_id);

  // UE 2 and 3 will get assigned the different pucch_res_indicator from UE 0 and 1, as they share the PUCCH configs.
  const unsigned                         ue_2_idx = 2;
  const pucch_harq_resource_alloc_record record_ue_2 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_2_idx]->cnrti, ues[ue_2_idx]->get_pucch_cfg());
  ASSERT_EQ(1, record_ue_2.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_2_idx]->get_pucch_cfg().pucch_res_list[1], record_ue_2.pucch_res);
  ASSERT_EQ(1, record_ue_2.pucch_res->res_id.cell_res_id);

  const unsigned                         ue_3_idx = 3;
  const pucch_harq_resource_alloc_record record_ue_3 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_3_idx]->cnrti, ues[ue_3_idx]->get_pucch_cfg());

  ASSERT_EQ(1, record_ue_3.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_3_idx]->get_pucch_cfg().pucch_res_list[1], record_ue_3.pucch_res);
  ASSERT_EQ(10, record_ue_3.pucch_res->res_id.cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_f2_only)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const unsigned                         ue_0_idx = 0;
  const pucch_harq_resource_alloc_record record_ue_0 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_0_idx]->cnrti, ues[ue_0_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_0.pucch_res_indicator);
  // The first F2 resource is has index 9 within the UE pucch_res_list (after 8+1 PUCCH F1).
  ASSERT_EQ(&ues[ue_0_idx]->get_pucch_cfg().pucch_res_list[9], record_ue_0.pucch_res);
  ASSERT_EQ(18, record_ue_0.pucch_res->res_id.cell_res_id);

  const unsigned                         ue_1_idx = 1;
  const pucch_harq_resource_alloc_record record_ue_1 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_1_idx]->cnrti, ues[ue_1_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_1.pucch_res_indicator);
  // The first F2 resource is has index 9 within the UE pucch_res_list (after 8+1 PUCCH F1).
  ASSERT_EQ(&ues[ue_1_idx]->get_pucch_cfg().pucch_res_list[9], record_ue_1.pucch_res);
  ASSERT_EQ(27, record_ue_1.pucch_res->res_id.cell_res_id);

  // UE 2 and 3 will get assigned the different pucch_res_indicator from UE 0 and 1, as they share the PUCCH configs.
  const unsigned                         ue_2_idx = 2;
  const pucch_harq_resource_alloc_record record_ue_2 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_2_idx]->cnrti, ues[ue_2_idx]->get_pucch_cfg());
  ASSERT_EQ(1, record_ue_2.pucch_res_indicator);
  // The second F2 resource has index 10 within the UE pucch_res_list (first 8+1 PUCCH F1, then 1 F2).
  ASSERT_EQ(&ues[ue_2_idx]->get_pucch_cfg().pucch_res_list[10], record_ue_2.pucch_res);
  ASSERT_EQ(19, record_ue_2.pucch_res->res_id.cell_res_id);

  const unsigned                         ue_3_idx = 3;
  const pucch_harq_resource_alloc_record record_ue_3 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_3_idx]->cnrti, ues[ue_3_idx]->get_pucch_cfg());

  ASSERT_EQ(1, record_ue_3.pucch_res_indicator);
  // The second F2 resource has index 10 within the UE pucch_res_list (first 8+1 PUCCH F1, then 1 F2).
  ASSERT_EQ(&ues[ue_3_idx]->get_pucch_cfg().pucch_res_list[10], record_ue_3.pucch_res);
  ASSERT_EQ(28, record_ue_3.pucch_res->res_id.cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_fetch_release)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const unsigned                         ue_0_idx = 0;
  const pucch_harq_resource_alloc_record record_ue_0 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_0_idx]->cnrti, ues[ue_0_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_0.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_0_idx]->get_pucch_cfg().pucch_res_list[0], record_ue_0.pucch_res);
  ASSERT_EQ(0, record_ue_0.pucch_res->res_id.cell_res_id);

  const unsigned                         ue_1_idx = 1;
  const pucch_harq_resource_alloc_record record_ue_1 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_1_idx]->cnrti, ues[ue_1_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_1.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_1_idx]->get_pucch_cfg().pucch_res_list[0], record_ue_1.pucch_res);
  ASSERT_EQ(9, record_ue_1.pucch_res->res_id.cell_res_id);

  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, ues[ue_0_idx]->cnrti, ues[ue_0_idx]->get_pucch_cfg()));
  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, ues[ue_1_idx]->cnrti, ues[ue_1_idx]->get_pucch_cfg()));
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_fetch_release_f2)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const unsigned                         ue_0_idx = 0;
  const pucch_harq_resource_alloc_record record_ue_0 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_0_idx]->cnrti, ues[ue_0_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_0.pucch_res_indicator);
  // The first F2 resource is has index 9 within the UE pucch_res_list (after 8+1 PUCCH F1).
  ASSERT_EQ(&ues[ue_0_idx]->get_pucch_cfg().pucch_res_list[9], record_ue_0.pucch_res);
  ASSERT_EQ(18, record_ue_0.pucch_res->res_id.cell_res_id);

  const unsigned                         ue_1_idx = 1;
  const pucch_harq_resource_alloc_record record_ue_1 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_1_idx]->cnrti, ues[ue_1_idx]->get_pucch_cfg());

  ASSERT_EQ(0, record_ue_1.pucch_res_indicator);
  // The first F2 resource is has index 9 within the UE pucch_res_list (after 8+1 PUCCH F1).
  ASSERT_EQ(&ues[ue_1_idx]->get_pucch_cfg().pucch_res_list[9], record_ue_1.pucch_res);
  ASSERT_EQ(27, record_ue_1.pucch_res->res_id.cell_res_id);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, ues[ue_0_idx]->cnrti, ues[ue_0_idx]->get_pucch_cfg()));

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, ues[ue_1_idx]->cnrti, ues[ue_1_idx]->get_pucch_cfg()));
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_2_ues_2_cfgs_alloc_specific_f2)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // UE 0 and 1 will get assigned the same pucch_res_indicator, as they use different PUCCH configs.
  const unsigned        ue_0_idx = 0;
  const pucch_resource* res_ue_0 =
      res_manager.reserve_set_1_res_by_res_indicator(sl_tx, ues[ue_0_idx]->cnrti, 5, ues[ue_0_idx]->get_pucch_cfg());

  ASSERT_EQ(&ues[ue_0_idx]->get_pucch_cfg().pucch_res_list[14], res_ue_0);
  ASSERT_EQ(23, res_ue_0->res_id.cell_res_id);

  const unsigned        ue_1_idx = 1;
  const pucch_resource* res_ue_1 =
      res_manager.reserve_set_1_res_by_res_indicator(sl_tx, ues[ue_1_idx]->cnrti, 5, ues[ue_1_idx]->get_pucch_cfg());

  ASSERT_EQ(&ues[ue_1_idx]->get_pucch_cfg().pucch_res_list[14], res_ue_1);
  ASSERT_EQ(32, res_ue_1->res_id.cell_res_id);

  // Try to allocate the same PUCCH resource (already reserved to UE 0 and 1) and check that the allocation fails.
  const unsigned        ue_2_idx = 2;
  const pucch_resource* res_ue_2 =
      res_manager.reserve_set_1_res_by_res_indicator(sl_tx, ues[ue_2_idx]->cnrti, 5, ues[ue_2_idx]->get_pucch_cfg());
  ASSERT_EQ(nullptr, res_ue_2);

  const unsigned        ue_3_idx = 3;
  const pucch_resource* res_ue_3 =
      res_manager.reserve_set_1_res_by_res_indicator(sl_tx, ues[ue_3_idx]->cnrti, 5, ues[ue_3_idx]->get_pucch_cfg());
  ASSERT_EQ(nullptr, res_ue_3);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_8_ues_2_cfgs_allocate_all_resources)
{
  create_ues(/*nof_ues*/ 8, /*nof_res_per_ue*/ 3, /*nof_cgfs*/ 2);

  // Allocate the first 3 UEs (sharing the same config) to occupy all available resources.
  allocate_f1_specific_ue(/* ue idx */ 0);
  allocate_f1_specific_ue(/* ue idx */ 2);
  allocate_f1_specific_ue(/* ue idx */ 4);

  pucch_harq_resource_alloc_record record_ue_6 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[6]->cnrti, ues[6]->get_pucch_cfg());
  ASSERT_EQ(nullptr, record_ue_6.pucch_res);

  allocate_f1_specific_ue(/* ue idx */ 1);
  allocate_f1_specific_ue(/* ue idx */ 3);
  allocate_f1_specific_ue(/* ue idx */ 5);

  pucch_harq_resource_alloc_record record_ue_7 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[7]->cnrti, ues[7]->get_pucch_cfg());
  ASSERT_EQ(nullptr, record_ue_7.pucch_res);

  // Release one resource and check the next allocation is successful.
  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, ues[2]->cnrti, ues[2]->get_pucch_cfg()));
  const unsigned ue_6_idx = 6;
  record_ue_6 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_6_idx]->cnrti, ues[ue_6_idx]->get_pucch_cfg());
  ASSERT_EQ(1, record_ue_6.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_6_idx]->get_pucch_cfg().pucch_res_list[1], record_ue_6.pucch_res);
  ASSERT_EQ(1, record_ue_6.pucch_res->res_id.cell_res_id);

  ASSERT_TRUE(res_manager.release_harq_set_0_resource(sl_tx, ues[3]->cnrti, ues[3]->get_pucch_cfg()));
  const unsigned ue_7_idx = 7;
  record_ue_7 =
      res_manager.reserve_next_set_0_harq_res_available(sl_tx, ues[ue_7_idx]->cnrti, ues[ue_7_idx]->get_pucch_cfg());
  ASSERT_EQ(1, record_ue_7.pucch_res_indicator);
  ASSERT_EQ(&ues[ue_7_idx]->get_pucch_cfg().pucch_res_list[1], record_ue_7.pucch_res);
  ASSERT_EQ(5, record_ue_7.pucch_res->res_id.cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_8_ues_2_cfgs_allocate_all_resources_f2)
{
  create_ues(/*nof_ues*/ 8, /*nof_res_per_ue*/ 3, /*nof_cgfs*/ 2);

  // Allocate the first 3 UEs (sharing the same config) to occupy all available resources.
  allocate_f2_specific_ue(/* ue idx */ 0);
  allocate_f2_specific_ue(/* ue idx */ 2);
  allocate_f2_specific_ue(/* ue idx */ 4);

  pucch_harq_resource_alloc_record record_ue_6 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[6]->cnrti, ues[6]->get_pucch_cfg());
  ASSERT_EQ(nullptr, record_ue_6.pucch_res);

  allocate_f2_specific_ue(/* ue idx */ 1);
  allocate_f2_specific_ue(/* ue idx */ 3);
  allocate_f2_specific_ue(/* ue idx */ 5);

  pucch_harq_resource_alloc_record record_ue_7 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[7]->cnrti, ues[7]->get_pucch_cfg());
  ASSERT_EQ(nullptr, record_ue_7.pucch_res);

  // Release one resource and check the next allocation is successful.
  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, ues[2]->cnrti, ues[2]->get_pucch_cfg()));
  const unsigned ue_6_idx = 6;
  record_ue_6 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_6_idx]->cnrti, ues[ue_6_idx]->get_pucch_cfg());
  ASSERT_EQ(1, record_ue_6.pucch_res_indicator);
  // The F2 resource corresponding to pucch_res_indicator = 1 has index 5 within the UE pucch_res_list (after 3+1 PUCCH
  // F1, then 1 F2).
  ASSERT_EQ(&ues[ue_6_idx]->get_pucch_cfg().pucch_res_list[5], record_ue_6.pucch_res);
  // The F2 resource corresponding to pucch_res_indicator = 1 has res ID 9.
  ASSERT_EQ(9, record_ue_6.pucch_res->res_id.cell_res_id);

  ASSERT_TRUE(res_manager.release_harq_set_1_resource(sl_tx, ues[3]->cnrti, ues[3]->get_pucch_cfg()));
  const unsigned ue_7_idx = 7;
  record_ue_7 =
      res_manager.reserve_next_set_1_harq_res_available(sl_tx, ues[ue_7_idx]->cnrti, ues[ue_7_idx]->get_pucch_cfg());
  ASSERT_EQ(1, record_ue_7.pucch_res_indicator);
  // The F2 resource corresponding to pucch_res_indicator = 1 has index 5 within the UE pucch_res_list (after 3+1 PUCCH
  // F1, then 1 F2).
  ASSERT_EQ(&ues[ue_7_idx]->get_pucch_cfg().pucch_res_list[5], record_ue_7.pucch_res);
  // The F2 resource corresponding to pucch_res_indicator = 1 has res ID 13.
  ASSERT_EQ(13, record_ue_7.pucch_res->res_id.cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_4_ues_2_cfgs_allocate_sr)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // Allocate SR for UE 0.
  const pucch_resource* sr_resource =
      res_manager.reserve_sr_res_available(sl_tx, ues[0]->cnrti, ues[0]->get_pucch_cfg());
  ASSERT_EQ(&ues[0]->get_pucch_cfg().pucch_res_list[8], sr_resource);
  ASSERT_EQ(nullptr, res_manager.reserve_sr_res_available(sl_tx, ues[2]->cnrti, ues[2]->get_pucch_cfg()));
  ASSERT_EQ(8, sr_resource->res_id.cell_res_id);

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_sr_resource(sl_tx, ues[0]->cnrti, ues[0]->get_pucch_cfg()));

  // Allocate SR to another UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue2 =
      res_manager.reserve_sr_res_available(sl_tx, ues[2]->cnrti, ues[2]->get_pucch_cfg());
  ASSERT_EQ(&ues[2]->get_pucch_cfg().pucch_res_list[8], sr_resource_ue2);
  ASSERT_EQ(8, sr_resource_ue2->res_id.cell_res_id);

  const pucch_resource* sr_resource_ue1 =
      res_manager.reserve_sr_res_available(sl_tx, ues[1]->cnrti, ues[1]->get_pucch_cfg());
  ASSERT_EQ(&ues[1]->get_pucch_cfg().pucch_res_list[8], sr_resource_ue1);
  ASSERT_EQ(sr_resource_ue1, res_manager.reserve_sr_res_available(sl_tx, ues[1]->cnrti, ues[1]->get_pucch_cfg()));
  ASSERT_EQ(17, sr_resource_ue1->res_id.cell_res_id);

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_sr_resource(sl_tx, ues[1]->cnrti, ues[1]->get_pucch_cfg()));

  // Allocate SR to another UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue3 =
      res_manager.reserve_sr_res_available(sl_tx, ues[3]->cnrti, ues[3]->get_pucch_cfg());
  ASSERT_EQ(&ues[3]->get_pucch_cfg().pucch_res_list[8], sr_resource_ue3);
  ASSERT_EQ(17, sr_resource_ue3->res_id.cell_res_id);
}

TEST_F(test_pucch_res_manager_multiple_cfg, test_4_ues_2_cfgs_allocate_csi)
{
  create_ues(/*nof_ues*/ 4, /*nof_res_per_ue*/ 8, /*nof_cgfs*/ 2);

  // Allocate SR for UE 0.
  const pucch_resource* csi_resource = res_manager.reserve_csi_resource(sl_tx, ues[0]->cnrti, ues[0]->ue_cell_cfg);
  ASSERT_EQ(&ues[0]->get_pucch_cfg().pucch_res_list[17], csi_resource);
  ASSERT_EQ(nullptr, res_manager.reserve_csi_resource(sl_tx, ues[2]->cnrti, ues[2]->ue_cell_cfg));
  ASSERT_EQ(26, csi_resource->res_id.cell_res_id);

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_csi_resource(sl_tx, ues[0]->cnrti, ues[0]->ue_cell_cfg));

  // Allocate SR to another UE and verify it is assigned a resource.
  const pucch_resource* csi_resource_ue2 = res_manager.reserve_csi_resource(sl_tx, ues[2]->cnrti, ues[2]->ue_cell_cfg);
  ASSERT_EQ(&ues[2]->get_pucch_cfg().pucch_res_list[17], csi_resource_ue2);
  ASSERT_EQ(26, csi_resource_ue2->res_id.cell_res_id);

  const pucch_resource* csi_resource_ue1 = res_manager.reserve_csi_resource(sl_tx, ues[1]->cnrti, ues[1]->ue_cell_cfg);
  ASSERT_EQ(&ues[1]->get_pucch_cfg().pucch_res_list[17], csi_resource_ue1);
  ASSERT_EQ(nullptr, res_manager.reserve_csi_resource(sl_tx, ues[3]->cnrti, ues[3]->ue_cell_cfg));
  ASSERT_EQ(35, csi_resource_ue1->res_id.cell_res_id);

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_csi_resource(sl_tx, ues[1]->cnrti, ues[1]->ue_cell_cfg));

  // Allocate SR to another UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue3 = res_manager.reserve_csi_resource(sl_tx, ues[3]->cnrti, ues[3]->ue_cell_cfg);
  ASSERT_EQ(&ues[3]->get_pucch_cfg().pucch_res_list[17], sr_resource_ue3);
  ASSERT_EQ(35, sr_resource_ue3->res_id.cell_res_id);
}
