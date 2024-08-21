/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/ue_scheduling/cell_harq_manager.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class harq_entity_test : public ::testing::Test
{
protected:
  cell_harq_manager cell_harqs{1};
  du_ue_index_t     ue_index  = to_du_ue_index(0);
  rnti_t            rnti      = to_rnti(0x4601);
  unsigned          max_retxs = 4;

  unique_ue_harq_entity harq_ent;
};

TEST_F(harq_entity_test, when_ue_harq_entity_is_created_cell_harq_manager_is_updated)
{
  ASSERT_FALSE(cell_harqs.contains(ue_index));
  harq_ent = cell_harqs.add_ue(ue_index, rnti, 16, 16);
  ASSERT_TRUE(cell_harqs.contains(ue_index));
  harq_ent.reset();
  ASSERT_FALSE(cell_harqs.contains(ue_index));
}

TEST_F(harq_entity_test, when_harq_entity_is_created_all_harqs_are_empty)
{
  harq_ent = cell_harqs.add_ue(ue_index, rnti, 16, 16);
  ASSERT_EQ(harq_ent.nof_dl_harqs(), 16);
  ASSERT_EQ(harq_ent.nof_ul_harqs(), 16);
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
  ASSERT_TRUE(harq_ent.has_empty_dl_harqs());
  ASSERT_TRUE(harq_ent.has_empty_ul_harqs());
  for (unsigned i = 0; i != 16; ++i) {
    ASSERT_FALSE(harq_ent.dl_harq(to_harq_id(i)).has_value());
    ASSERT_FALSE(harq_ent.ul_harq(to_harq_id(i)).has_value());
  }
}

TEST_F(harq_entity_test, when_harq_is_allocated_then_it_enters_waiting_ack_state)
{
  harq_ent = cell_harqs.add_ue(ue_index, rnti, 16, 16);
  slot_point            sl_tx{0, 0};
  unsigned              k1   = 4;
  auto                  h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_dl.value().is_waiting_ack());
  ASSERT_FALSE(h_dl.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), h_dl);
  auto h_ul = harq_ent.alloc_ul_harq(sl_tx, max_retxs);
  ASSERT_TRUE(h_ul.has_value());
  ASSERT_TRUE(h_ul.value().is_waiting_ack());
  ASSERT_FALSE(h_ul.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), h_ul);
}
