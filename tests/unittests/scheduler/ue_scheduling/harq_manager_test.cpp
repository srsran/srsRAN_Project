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

/// Test for a single UE HARQ entity.
class single_ue_harq_entity_test : public ::testing::Test
{
protected:
  const unsigned    max_ack_wait_timeout = 16;
  cell_harq_manager cell_harqs{1, nullptr, max_ack_wait_timeout};
  du_ue_index_t     ue_index  = to_du_ue_index(0);
  rnti_t            rnti      = to_rnti(0x4601);
  unsigned          max_retxs = 4;
};

TEST_F(single_ue_harq_entity_test, when_ue_harq_entity_is_created_cell_harq_manager_is_updated)
{
  ASSERT_FALSE(cell_harqs.contains(ue_index));
  unique_ue_harq_entity harq_ent = cell_harqs.add_ue(ue_index, rnti, 16, 16);
  ASSERT_TRUE(cell_harqs.contains(ue_index));
  harq_ent.reset();
  ASSERT_FALSE(cell_harqs.contains(ue_index));
}

TEST_F(single_ue_harq_entity_test, when_harq_entity_is_created_all_harqs_are_empty)
{
  unique_ue_harq_entity harq_ent = cell_harqs.add_ue(ue_index, rnti, 16, 16);
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

TEST_F(single_ue_harq_entity_test, when_harq_is_allocated_then_it_enters_waiting_ack_state)
{
  unique_ue_harq_entity harq_ent = cell_harqs.add_ue(ue_index, rnti, 16, 16);
  slot_point            sl_tx{0, 0};
  unsigned              k1   = 4;
  auto                  h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(sl_tx, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());
  ASSERT_TRUE(h_dl.value().is_waiting_ack());
  ASSERT_TRUE(h_ul.value().is_waiting_ack());
  ASSERT_FALSE(h_dl.value().has_pending_retx());
  ASSERT_FALSE(h_ul.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), h_dl);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), h_ul);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
}

TEST_F(single_ue_harq_entity_test, when_all_harqs_are_allocated_harq_entity_cannot_find_empty_harq)
{
  unsigned              nof_harqs = 4;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  ASSERT_EQ(harq_ent.nof_dl_harqs(), nof_harqs);
  ASSERT_EQ(harq_ent.nof_ul_harqs(), nof_harqs);

  slot_point sl_tx{0, 0};
  unsigned   k1 = 4;
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(sl_tx, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_dl.value().is_waiting_ack());
    ASSERT_TRUE(h_ul.has_value());
    ASSERT_TRUE(h_ul.value().is_waiting_ack());
  }

  ASSERT_NE(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_NE(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
  ASSERT_FALSE(harq_ent.has_empty_dl_harqs());
  ASSERT_FALSE(harq_ent.has_empty_ul_harqs());

  auto h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
  auto h_ul = harq_ent.alloc_ul_harq(sl_tx, max_retxs);
  ASSERT_FALSE(h_dl.has_value());
  ASSERT_FALSE(h_ul.has_value());
}

TEST_F(single_ue_harq_entity_test, when_ue_harq_entity_is_deallocated_then_harq_resources_are_available_again)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  slot_point            sl_tx{0, 0};
  unsigned              k1 = 4;
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(sl_tx, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_ul.has_value());
  }

  harq_ent.reset();
  harq_ent = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(sl_tx, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_ul.has_value());
  }
}

TEST_F(single_ue_harq_entity_test, positive_ack_sets_harq_to_empty)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  slot_point            sl_tx{0, 0};
  unsigned              k1 = 4, k2 = 6;
  auto                  h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(sl_tx + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());

  ASSERT_EQ(harq_ent.find_dl_harq(sl_tx + k1, 0), h_dl);
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::ack, 5), dl_harq_process_view::status_update::acked);
  ASSERT_FALSE(h_dl.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_ul_harq(sl_tx + k2), h_ul);
  ASSERT_GE(h_ul.value().ul_crc_info(true), 0);
  ASSERT_FALSE(h_ul.value().has_pending_retx());
}

TEST_F(single_ue_harq_entity_test, negative_ack_sets_harq_to_pending_retx)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  slot_point            sl_tx{0, 0};
  unsigned              k1 = 4, k2 = 6;
  auto                  h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(sl_tx + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());

  ASSERT_EQ(harq_ent.find_dl_harq(sl_tx + k1, 0), h_dl);
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_TRUE(h_dl.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_ul_harq(sl_tx + k2), h_ul);
  ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
  ASSERT_TRUE(h_ul.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), h_dl.value());
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), h_ul.value());
}

TEST_F(single_ue_harq_entity_test, when_max_retxs_reached_then_harq_becomes_empty)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  slot_point            sl_tx{0, 0};
  unsigned              k1 = 4, k2 = 6;
  auto                  h_dl = harq_ent.alloc_dl_harq(sl_tx, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(sl_tx + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());
  for (unsigned i = 0; i != max_retxs; ++i) {
    ASSERT_EQ(harq_ent.find_dl_harq(sl_tx + k1, 0), h_dl);
    ASSERT_EQ(harq_ent.find_ul_harq(sl_tx + k2), h_ul);
    ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5),
              dl_harq_process_view::status_update::nacked);
    ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
    ASSERT_FALSE(h_dl.value().is_waiting_ack());
    ASSERT_FALSE(h_ul.value().is_waiting_ack());
    ASSERT_TRUE(h_dl.value().has_pending_retx());
    ASSERT_TRUE(h_ul.value().has_pending_retx());

    ++sl_tx;
    h_dl.value().new_retx(sl_tx, k1, 0);
    h_ul.value().new_retx(sl_tx + k2);
  }
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
  ASSERT_FALSE(h_dl.value().has_pending_retx());
  ASSERT_FALSE(h_ul.value().has_pending_retx());
  ASSERT_FALSE(h_dl.value().is_waiting_ack());
  ASSERT_FALSE(h_ul.value().is_waiting_ack());
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
}

TEST_F(single_ue_harq_entity_test, after_max_ack_wait_timeout_dl_harqs_are_available_for_newtx)
{
  const unsigned        nof_harqs = 8;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  slot_point            sl_tx{0, 0};
  unsigned              ack_delay = 4;

  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(sl_tx, ack_delay, max_retxs, 0);
    ASSERT_TRUE(h_dl.has_value());
  }
  for (unsigned i = 0; i != this->max_ack_wait_timeout + ack_delay; ++i) {
    ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
    ASSERT_NE(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
    ASSERT_FALSE(harq_ent.has_empty_dl_harqs());
    cell_harqs.slot_indication(++sl_tx);
  }
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_TRUE(harq_ent.has_empty_dl_harqs());
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(sl_tx, ack_delay, max_retxs, 0);
    ASSERT_TRUE(h_dl.has_value());
  }
}
