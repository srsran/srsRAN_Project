/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/cu_up/ue_manager.h"
#include "srsgnb/cu_up/cu_up_types.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_up;

/// Fixture class for UE manager tests
class ue_manager_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create DUT object
    ue_mng = std::make_unique<ue_manager>(test_logger, timers);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<ue_manager_ctrl> ue_mng;
  srslog::basic_logger&            test_logger = srslog::fetch_basic_logger("TEST", false);
  timer_manager                    timers;
};

/// UE object handling tests (creation/deletion)
TEST_F(ue_manager_test, when_ue_db_not_full_new_ue_can_be_added)
{
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
  ue_context* ue = ue_mng->add_ue();
  ASSERT_NE(ue, nullptr);
  ASSERT_EQ(ue_mng->get_nof_ues(), 1);
}

TEST_F(ue_manager_test, when_ue_db_is_full_new_ue_cannot_be_added)
{
  // add maximum number of UE objects
  for (uint32_t i = 0; i < MAX_NOF_UES; i++) {
    ue_context* ue = ue_mng->add_ue();
    ASSERT_NE(ue, nullptr);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), MAX_NOF_UES);

  // try to add one more
  ue_context* ue = ue_mng->add_ue();
  ASSERT_EQ(ue, nullptr);
}

TEST_F(ue_manager_test, when_ue_are_deleted_ue_db_is_empty)
{
  // add maximum number of UE objects
  for (uint32_t i = 0; i < MAX_NOF_UES; i++) {
    ue_context* ue = ue_mng->add_ue();
    ASSERT_NE(ue, nullptr);
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), MAX_NOF_UES);

  // delete all UE objects
  for (uint32_t i = 0; i < MAX_NOF_UES; i++) {
    ue_mng->remove_ue(int_to_ue_index(i));
  }
  ASSERT_EQ(ue_mng->get_nof_ues(), 0);
}
