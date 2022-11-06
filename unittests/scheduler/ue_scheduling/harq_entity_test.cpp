/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/ue_scheduling/harq_process.h"
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(harq_entity, when_harq_entity_is_created_all_harqs_are_empty)
{
  harq_entity harq_ent(to_rnti(0x4601), 16, 4);

  ASSERT_EQ(harq_ent.nof_dl_harqs(), 16);
  ASSERT_EQ(harq_ent.nof_ul_harqs(), 16);
  ASSERT_NE(harq_ent.find_empty_dl_harq(), nullptr);
  ASSERT_TRUE(harq_ent.find_empty_dl_harq()->empty());
  ASSERT_NE(harq_ent.find_empty_ul_harq(), nullptr);
  ASSERT_TRUE(harq_ent.find_empty_ul_harq()->empty());
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), nullptr);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), nullptr);
}

TEST(harq_entity, when_all_harqs_are_allocated_harq_entity_cannot_find_empty_harq)
{
  unsigned    nof_harqs = 8;
  harq_entity harq_ent(to_rnti(0x4601), nof_harqs, 4);
  slot_point  sl_tx{0, 0};
  unsigned    ack_delay = 4;

  for (unsigned i = 0; i != nof_harqs; ++i) {
    ASSERT_NE(harq_ent.find_empty_dl_harq()->new_tx(0, sl_tx, ack_delay, 4), nullptr);
    ASSERT_NE(harq_ent.find_empty_ul_harq()->new_tx(sl_tx, 4), nullptr);
  }
  ASSERT_EQ(harq_ent.find_empty_dl_harq(), nullptr);
  ASSERT_EQ(harq_ent.find_empty_ul_harq(), nullptr);
}

TEST(harq_entity, after_max_ack_wait_timeout_dl_harqs_are_available_for_retx)
{
  unsigned    nof_harqs = 8, max_ack_wait_slots = 4;
  harq_entity harq_ent(to_rnti(0x4601), nof_harqs, max_ack_wait_slots);
  slot_point  sl_tx{0, 0};
  unsigned    ack_delay = 4;

  for (unsigned i = 0; i != nof_harqs; ++i) {
    ASSERT_NE(harq_ent.find_empty_dl_harq()->new_tx(0, sl_tx, ack_delay, 4), nullptr);
  }
  for (unsigned i = 0; i != max_ack_wait_slots + ack_delay; ++i) {
    ASSERT_EQ(harq_ent.find_empty_dl_harq(), nullptr);
    ASSERT_EQ(harq_ent.find_pending_dl_retx(), nullptr);
    harq_ent.slot_indication(++sl_tx);
  }
  ASSERT_EQ(harq_ent.find_empty_dl_harq(), nullptr);
  ASSERT_NE(harq_ent.find_pending_dl_retx(), nullptr);
  ASSERT_TRUE(harq_ent.find_pending_dl_retx()->has_pending_retx());
}
