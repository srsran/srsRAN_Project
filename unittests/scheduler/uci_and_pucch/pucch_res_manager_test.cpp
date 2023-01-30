/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_test_utils.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>

using namespace srsgnb;

////////////    Test the PUCCH resource manager class     ////////////

class test_pucch_resource_manager : public ::testing::Test
{
public:
  test_pucch_resource_manager() :
    pucch_cfg{config_helpers::make_default_ue_uplink_config().init_ul_bwp.pucch_cfg.value()}, sl_tx(slot_point(0, 0))
  {
    res_manager.slot_indication(sl_tx);
  };

protected:
  pucch_config           pucch_cfg;
  pucch_resource_manager res_manager;
  slot_point             sl_tx;

  // Allocate PUCCH for a given number of UEs, in increasing order of RNTI.
  void allocate_ues(unsigned nof_ues_to_allocate, bool format_2 = false)
  {
    if (format_2 == false) {
      for (size_t n = 0; n != nof_ues_to_allocate; ++n) {
        rnti_t rnti = to_rnti(0x4601 + n);
        res_manager.get_next_harq_res_available(sl_tx, rnti, pucch_cfg);
      }
    } else {
      for (size_t n = 0; n != nof_ues_to_allocate; ++n) {
        rnti_t rnti = to_rnti(0x4601 + n);
        res_manager.get_next_format2_res_available(sl_tx, rnti, pucch_cfg);
      }
    }
  };
};

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for 1 UE only.
TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_1)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_2)
{
  allocate_ues(1);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  ASSERT_EQ(1, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[1], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_3)
{
  allocate_ues(2);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  ASSERT_EQ(2, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[2], record.pucch_res);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for n UEs.
TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_4)
{
  allocate_ues(3);
  // Attempt to allocate the PUCCH resource to the 4th UE.
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4604), pucch_cfg);

  // Verify that the 4th UE did not get any resource assigned.
  ASSERT_EQ(nullptr, record.pucch_res);
}

// Tests allocation in different slots.
TEST_F(test_pucch_resource_manager, get_next_harq_different_slot)
{
  allocate_ues(1);
  ++sl_tx;
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);
}

// Tests slot indication for PUCCH resource manager.
TEST_F(test_pucch_resource_manager, slot_indication)
{
  res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  // Increment slot point and invoke slot_indication(), which should reset the previous UE's resource allocation.
  ++sl_tx;
  res_manager.slot_indication(sl_tx);

  // Slot point pointing at the last slot, that has been cleared by that slot_indication().
  slot_point old_slot{0, sl_tx.to_uint() - 1};
  int        res_id = res_manager.get_f1_pucch_res_indic(old_slot, to_rnti(0x4601));

  // Expect that pucch_res_indicator = -1 is returned (due to the slot_indication() resetting the resource records for
  // old slots).
  ASSERT_EQ(-1, res_id);
}

// Tests allocation and removal of PUCCH resource format 1 for 1 UE.
TEST_F(test_pucch_resource_manager, allocate_and_release_f1)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_EQ(-1, res_manager.get_f1_pucch_res_indic(sl_tx, to_rnti(0x4601)));

  // Re-allocate the resource.
  const pucch_harq_resource_alloc_record reallocation =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0, reallocation.pucch_res_indicator);
  ASSERT_EQ(record.pucch_res, reallocation.pucch_res);
}

// Tests allocation and removal of PUCCH resource format 1 for multiple UEs.
TEST_F(test_pucch_resource_manager, allocate_and_release_f1_multiple_ues)
{
  // Allocate 3 UEs.
  allocate_ues(3);

  // Check whether the UEs get returned the corresponding PUCCH resource indicator.
  ASSERT_EQ(0, res_manager.get_f1_pucch_res_indic(sl_tx, to_rnti(0x4601)));
  ASSERT_EQ(2, res_manager.get_f1_pucch_res_indic(sl_tx, to_rnti(0x4603)));

  // Release the resource and verify that the UEs do not hold it anymore.
  ASSERT_TRUE(res_manager.release_harq_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_TRUE(res_manager.release_harq_resource(sl_tx, to_rnti(0x4603), pucch_cfg));
  ASSERT_EQ(-1, res_manager.get_f1_pucch_res_indic(sl_tx, to_rnti(0x4601)));
  ASSERT_EQ(-1, res_manager.get_f1_pucch_res_indic(sl_tx, to_rnti(0x4603)));

  // Re-allocate the resources to UE1 and UE3.
  const pucch_harq_resource_alloc_record realloc_ue1 =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  const pucch_harq_resource_alloc_record realloc_ue3 =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  // Check whether the UEs get returned (again) the corresponding PUCCH resource indicator.
  ASSERT_EQ(0, realloc_ue1.pucch_res_indicator);
  ASSERT_EQ(2, realloc_ue3.pucch_res_indicator);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for 1 UE only.
TEST_F(test_pucch_resource_manager, allocate_1_ue_res_f2)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  unsigned res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator];
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for n UEs.
TEST_F(test_pucch_resource_manager, allocate_2_ue_res_f2)
{
  allocate_ues(1, /* format_2= */ true);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  ASSERT_EQ(1, record.pucch_res_indicator);
  unsigned res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator];
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for n UEs.
TEST_F(test_pucch_resource_manager, allocate_3_ue_res_f2)
{
  allocate_ues(2, /* format_2= */ true);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  ASSERT_EQ(2, record.pucch_res_indicator);
  unsigned res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator];
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, allocate_8_ue_res_f2)
{
  allocate_ues(7, /* format_2= */ true);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4608), pucch_cfg);

  // Expect the 8th UE not to get assigned anything, as there are only 7 resources available.
  ASSERT_EQ(nullptr, record.pucch_res);
}

// Tests allocation in different slots.
TEST_F(test_pucch_resource_manager, get_format2_different_slot)
{
  allocate_ues(1);
  ++sl_tx;
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0, record.pucch_res_indicator);
  unsigned res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator];
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);
}

// Tests allocation and release of resources for 1 UE.
TEST_F(test_pucch_resource_manager, allocate_and_release_f2)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  unsigned res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator];
  ASSERT_EQ(&pucch_cfg.pucch_res_list[res_idx_from_list], record.pucch_res);

  // Release the resource and verify the UE does not hold it anymore.
  ASSERT_TRUE(res_manager.release_format2_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_EQ(-1, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4601)));

  // Re-allocate the resource.
  const pucch_harq_resource_alloc_record reallocation =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  ASSERT_EQ(0, reallocation.pucch_res_indicator);
  res_idx_from_list = pucch_cfg.pucch_res_set[1].pucch_res_id_list[record.pucch_res_indicator];
  ASSERT_EQ(record.pucch_res, reallocation.pucch_res);
}

// Tests allocation and release of resources for multiple UE.
TEST_F(test_pucch_resource_manager, allocate_and_release_f2_multiple_ues)
{
  // Allocate 7 UEs.
  allocate_ues(7, /* format_2*/ true);

  // Check whether the UEs get returned the corresponding PUCCH resource indicator.
  ASSERT_EQ(0, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4601)));
  ASSERT_EQ(2, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4603)));
  ASSERT_EQ(6, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4607)));

  // Release the resource and verify that the UEs do not hold it anymore.
  ASSERT_TRUE(res_manager.release_format2_resource(sl_tx, to_rnti(0x4601), pucch_cfg));
  ASSERT_TRUE(res_manager.release_format2_resource(sl_tx, to_rnti(0x4603), pucch_cfg));
  ASSERT_TRUE(res_manager.release_format2_resource(sl_tx, to_rnti(0x4607), pucch_cfg));
  ASSERT_EQ(-1, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4601)));
  ASSERT_EQ(-1, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4603)));
  ASSERT_EQ(-1, res_manager.get_f2_pucch_res_indic(sl_tx, to_rnti(0x4607)));

  // Re-allocate the resources to UE1, UE3, UE7.
  const pucch_harq_resource_alloc_record realloc_ue1 =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);
  const pucch_harq_resource_alloc_record realloc_ue3 =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);
  const pucch_harq_resource_alloc_record realloc_ue7 =
      res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4608), pucch_cfg);

  // Check whether the UEs get returned (again) the corresponding PUCCH resource indicator.
  ASSERT_EQ(0, realloc_ue1.pucch_res_indicator);
  ASSERT_EQ(2, realloc_ue3.pucch_res_indicator);
  ASSERT_EQ(6, realloc_ue7.pucch_res_indicator);
}

// Tests slot indication for PUCCH resource manager, format 2.
TEST_F(test_pucch_resource_manager, slot_indication_format2)
{
  res_manager.get_next_format2_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  // Increment slot point and invoke slot_indication(), which should reset the previous UE's resource allocation.
  ++sl_tx;
  res_manager.slot_indication(sl_tx);

  // Slot point pointing at the last slot, that has been cleared by that slot_indication().
  slot_point old_slot{0, sl_tx.to_uint() - 1};
  int        res_id = res_manager.get_f2_pucch_res_indic(old_slot, to_rnti(0x4601));

  // Expect that pucch_res_indicator = -1 is returned (due to the slot_indication() resetting the resource records for
  // old slots).
  ASSERT_EQ(-1, res_id);
}

// Tests allocation of SR resource.
TEST_F(test_pucch_resource_manager, test_allocation_sr_resource)
{
  const pucch_resource* sr_resource = res_manager.get_next_sr_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  unsigned sr_pucch_res_idx = pucch_cfg.pucch_res_list.size() - 1;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[sr_pucch_res_idx], sr_resource);
}

// Tests release of SR resource.
TEST_F(test_pucch_resource_manager, test_allocation_release_sr_resource)
{
  res_manager.get_next_sr_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  // Release resource and verify it was successful.
  ASSERT_TRUE(res_manager.release_sr_resource(sl_tx, to_rnti(0x4601)));

  // Assigne a different UE and verify it is assigned a resource.
  const pucch_resource* sr_resource_ue2 = res_manager.get_next_sr_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);
  ASSERT_FALSE(nullptr == sr_resource_ue2);
  unsigned sr_pucch_res_idx = pucch_cfg.pucch_res_list.size() - 1;
  ASSERT_EQ(&pucch_cfg.pucch_res_list[sr_pucch_res_idx], sr_resource_ue2);
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
