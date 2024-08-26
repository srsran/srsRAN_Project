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
  void run_slot() { cell_harqs.slot_indication(++next_slot); }

  const unsigned    max_ack_wait_timeout = 16;
  cell_harq_manager cell_harqs{1, nullptr, max_ack_wait_timeout};
  du_ue_index_t     ue_index  = to_du_ue_index(0);
  rnti_t            rnti      = to_rnti(0x4601);
  unsigned          max_retxs = 4;

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
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
  unsigned              k1       = 4;
  auto                  h_dl     = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto                  h_ul     = harq_ent.alloc_ul_harq(next_slot, max_retxs);
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

  unsigned k1 = 4;
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(next_slot, max_retxs);
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

  auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto h_ul = harq_ent.alloc_ul_harq(next_slot, max_retxs);
  ASSERT_FALSE(h_dl.has_value());
  ASSERT_FALSE(h_ul.has_value());
}

TEST_F(single_ue_harq_entity_test, when_ue_harq_entity_is_deallocated_then_harq_resources_are_available_again)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  unsigned              k1        = 4;
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(next_slot, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_ul.has_value());
  }

  harq_ent.reset();
  harq_ent = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(next_slot, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_ul.has_value());
  }
}

TEST_F(single_ue_harq_entity_test, positive_ack_sets_harq_to_empty)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  unsigned              k1 = 4, k2 = 6;
  auto                  h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());

  ASSERT_EQ(harq_ent.find_dl_harq(next_slot + k1, 0), h_dl);
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::ack, 5), dl_harq_process_view::status_update::acked);
  ASSERT_FALSE(h_dl.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_ul_harq(next_slot + k2), h_ul);
  ASSERT_GE(h_ul.value().ul_crc_info(true), 0);
  ASSERT_FALSE(h_ul.value().has_pending_retx());
}

TEST_F(single_ue_harq_entity_test, negative_ack_sets_harq_to_pending_retx)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  unsigned              k1 = 4, k2 = 6;
  auto                  h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());

  ASSERT_EQ(harq_ent.find_dl_harq(next_slot + k1, 0), h_dl);
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_TRUE(h_dl.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_ul_harq(next_slot + k2), h_ul);
  ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
  ASSERT_TRUE(h_ul.value().has_pending_retx());
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), h_dl.value());
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), h_ul.value());
}

TEST_F(single_ue_harq_entity_test, when_max_retxs_reached_then_harq_becomes_empty)
{
  unsigned              nof_harqs = 16;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
  unsigned              k1 = 4, k2 = 6;
  auto                  h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto                  h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());
  for (unsigned i = 0; i != max_retxs; ++i) {
    ASSERT_EQ(harq_ent.find_dl_harq(next_slot + k1, 0), h_dl);
    ASSERT_EQ(harq_ent.find_ul_harq(next_slot + k2), h_ul);
    ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5),
              dl_harq_process_view::status_update::nacked);
    ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
    ASSERT_FALSE(h_dl.value().is_waiting_ack());
    ASSERT_FALSE(h_ul.value().is_waiting_ack());
    ASSERT_TRUE(h_dl.value().has_pending_retx());
    ASSERT_TRUE(h_ul.value().has_pending_retx());

    run_slot();
    h_dl.value().new_retx(next_slot, k1, 0);
    h_ul.value().new_retx(next_slot + k2);
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
  unsigned              k1 = 4, k2 = 4;

  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_ul.has_value());
  }
  for (unsigned i = 0; i != this->max_ack_wait_timeout + k1; ++i) {
    ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
    ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
    ASSERT_NE(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
    ASSERT_NE(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
    ASSERT_FALSE(harq_ent.has_empty_dl_harqs());
    ASSERT_FALSE(harq_ent.has_empty_ul_harqs());
    run_slot();
  }
  run_slot();
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
  ASSERT_TRUE(harq_ent.has_empty_dl_harqs());
  ASSERT_TRUE(harq_ent.has_empty_ul_harqs());
  for (unsigned i = 0; i != nof_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
    auto h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs);
    ASSERT_TRUE(h_dl.has_value());
    ASSERT_TRUE(h_ul.has_value());
  }
}

class single_ue_dl_harq_ack_test : public single_ue_harq_entity_test
{
protected:
  const unsigned        nof_harqs = 8;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
};

TEST_F(single_ue_dl_harq_ack_test, when_dtx_received_after_ack_then_dtx_is_ignored)
{
  const unsigned k1 = 4, harq_bit_idx = 0;

  auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, nof_harqs, harq_bit_idx);
  h_dl->increment_pucch_counter();
  h_dl->increment_pucch_counter();
  slot_point pucch_slot = next_slot + k1;

  while (next_slot != pucch_slot) {
    run_slot();
  }

  // ACK received.
  auto h_dl_ack = this->harq_ent.find_dl_harq(pucch_slot, harq_bit_idx);
  ASSERT_EQ(h_dl, h_dl_ack);
  ASSERT_EQ(h_dl_ack->dl_ack_info(mac_harq_ack_report_status::ack, std::nullopt),
            dl_harq_process_view::status_update::no_update);

  // DTX received one slot late.
  run_slot();
  h_dl_ack = this->harq_ent.find_dl_harq(pucch_slot, harq_bit_idx);
  ASSERT_EQ(h_dl, h_dl_ack);
  ASSERT_EQ(h_dl_ack->dl_ack_info(mac_harq_ack_report_status::dtx, std::nullopt),
            dl_harq_process_view::status_update::acked);
}

// Note: When two F1 PUCCHs are decoded (one with SR and the other without), there is a small chance that none of them
// are DTX.
TEST_F(single_ue_dl_harq_ack_test, when_ack_received_after_nack_then_process_becomes_empty)
{
  const unsigned k1 = 4, harq_bit_idx = 0;

  auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, nof_harqs, harq_bit_idx);
  h_dl->increment_pucch_counter();
  h_dl->increment_pucch_counter();
  slot_point pucch_slot = next_slot + k1;
  while (next_slot != pucch_slot) {
    run_slot();
  }

  // NACK received.
  auto h_dl_ack = this->harq_ent.find_dl_harq(pucch_slot, harq_bit_idx);
  ASSERT_EQ(h_dl_ack, h_dl);
  ASSERT_EQ(h_dl_ack->dl_ack_info(mac_harq_ack_report_status::nack, 1.0F),
            dl_harq_process_view::status_update::no_update);

  // ACK received.
  h_dl_ack = this->harq_ent.find_dl_harq(pucch_slot, harq_bit_idx);
  ASSERT_EQ(h_dl_ack, h_dl);
  ASSERT_EQ(h_dl_ack->dl_ack_info(mac_harq_ack_report_status::ack, 2.0F), dl_harq_process_view::status_update::acked);

  // HARQ should be empty.
  ASSERT_FALSE(h_dl->is_waiting_ack() or h_dl->has_pending_retx());
}

namespace {

enum harq_state_outcome { ACKed, NACKed, DTX_timeout };

struct test_2_harq_bits_params {
  // Vector size represents the number of decoded PUCCHs forwarded to the scheduler.
  std::vector<std::array<uint8_t, 2>> ack;
  std::array<harq_state_outcome, 2>   outcome;
};

/// \brief With this test suite, we intend to test the scenario where two HARQ bits arrive in a single PUCCH PDU to the
/// scheduler.
class single_ue_harq_entity_2_bits_tester : public ::testing::TestWithParam<test_2_harq_bits_params>
{
protected:
  single_ue_harq_entity_2_bits_tester()
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    // Allocate 2 HARQs with same PUCCH slot.
    // > First HARQ, DAI=0.
    run_slot();
    auto h_dl1 = harq_ent.alloc_dl_harq(next_slot, 5, max_harq_retxs, 0);
    h_dl1->increment_pucch_counter();
    h_dls.push_back(h_dl1->id());
    // > Second HARQ, DAI=1.
    run_slot();
    auto h_dl2 = harq_ent.alloc_dl_harq(next_slot, 4, max_harq_retxs, 1);
    h_dl2->increment_pucch_counter();
    h_dls.push_back(h_dl2->id());
    if (GetParam().ack.size() > 1) {
      h_dl1->increment_pucch_counter();
      h_dl2->increment_pucch_counter();
    }

    pucch_slot = next_slot + 4;

    while (next_slot <= pucch_slot + pucch_process_delay) {
      run_slot();
    }
  }

  ~single_ue_harq_entity_2_bits_tester() override { srslog::flush(); }

  void run_slot()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    cell_harqs.slot_indication(next_slot);
    ++next_slot;
  }

  const unsigned        nof_harqs = 16, max_harq_retxs = 4, pucch_process_delay = 4;
  const unsigned        max_ack_wait_timeout = 16;
  du_ue_index_t         ue_index             = to_du_ue_index(0);
  rnti_t                rnti                 = to_rnti(0x4601);
  srslog::basic_logger& logger               = srslog::fetch_basic_logger("SCHED");
  cell_harq_manager     cell_harqs{1, nullptr, max_ack_wait_timeout};
  unique_ue_harq_entity harq_ent = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
  slot_point pucch_slot;

  std::vector<harq_id_t> h_dls;
};

} // namespace

TEST_P(single_ue_harq_entity_2_bits_tester, handle_pucchs)
{
  auto params = GetParam();

  // First PUCCH, 2 HARQ bits, different indexes.
  auto h_dl1 = harq_ent.find_dl_harq(pucch_slot, 0);
  auto h_dl2 = harq_ent.find_dl_harq(pucch_slot, 1);
  h_dl1->dl_ack_info((mac_harq_ack_report_status)params.ack[0][0], std::nullopt);
  h_dl2->dl_ack_info((mac_harq_ack_report_status)params.ack[0][1], std::nullopt);

  // Second PUCCH, 2 HARQ bits, different indexes.
  if (params.ack.size() > 1) {
    h_dl1 = harq_ent.find_dl_harq(pucch_slot, 0);
    h_dl2 = harq_ent.find_dl_harq(pucch_slot, 1);
    h_dl1->dl_ack_info((mac_harq_ack_report_status)params.ack[1][0], std::nullopt);
    h_dl2->dl_ack_info((mac_harq_ack_report_status)params.ack[1][1], std::nullopt);
  }

  bool check_timeout = false;
  for (unsigned i = 0; i != params.outcome.size(); ++i) {
    if (params.outcome[i] == ACKed) {
      ASSERT_FALSE(harq_ent.dl_harq(h_dls[i]).has_value());
    } else {
      ASSERT_TRUE(harq_ent.dl_harq(h_dls[i]).value().has_pending_retx());
    }

    if (params.outcome[i] == DTX_timeout) {
      // DTX_timeout
      check_timeout = true;
    }
  }

  // Check if HARQs timeout in case of HARQ-ACK set to DTX.
  if (check_timeout) {
    for (unsigned i = 0; i != cell_harq_manager::SHORT_ACK_TIMEOUT_DTX; ++i) {
      run_slot();
    }
    for (unsigned i = 0; i != params.outcome.size(); ++i) {
      if (params.outcome[i] == DTX_timeout) {
        ASSERT_TRUE(harq_ent.dl_harq(h_dls[i]).value().has_pending_retx());
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    harq_manager_test,
    single_ue_harq_entity_2_bits_tester,
    testing::Values(test_2_harq_bits_params{.ack = {{1, 1}}, .outcome = {ACKed, ACKed}},
                    test_2_harq_bits_params{.ack = {{0, 0}}, .outcome = {NACKed, NACKed}},
                    test_2_harq_bits_params{.ack = {{2, 2}}, .outcome = {DTX_timeout, DTX_timeout}},
                    test_2_harq_bits_params{.ack = {{2, 1}}, .outcome = {DTX_timeout, ACKed}},
                    test_2_harq_bits_params{.ack = {{1, 1}, {2, 2}}, .outcome = {ACKed, ACKed}},
                    test_2_harq_bits_params{.ack = {{0, 0}, {2, 2}}, .outcome = {NACKed, NACKed}},
                    test_2_harq_bits_params{.ack = {{2, 2}, {2, 1}}, .outcome = {NACKed, ACKed}},
                    test_2_harq_bits_params{.ack = {{2, 2}, {2, 2}}, .outcome = {NACKed, NACKed}}));

class single_ue_harq_entity_harq_5bit_tester : public ::testing::Test
{
protected:
  single_ue_harq_entity_harq_5bit_tester()
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();
  }

  void run_slot()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());
    cell_harqs.slot_indication(next_slot);
    ++next_slot;
  }

  const unsigned        nof_harqs = 8, max_harq_retxs = 4, pucch_process_delay = 4;
  const unsigned        max_ack_wait_timeout = 16;
  du_ue_index_t         ue_index             = to_du_ue_index(0);
  rnti_t                rnti                 = to_rnti(0x4601);
  srslog::basic_logger& logger               = srslog::fetch_basic_logger("SCHED");

  cell_harq_manager     cell_harqs{1, nullptr, max_ack_wait_timeout};
  unique_ue_harq_entity harq_ent = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
};

TEST_F(single_ue_harq_entity_harq_5bit_tester, when_5_harq_bits_are_acks_then_all_5_active_harqs_are_updated)
{
  const unsigned active_harqs = 5, k1 = 4;

  std::vector<harq_id_t> h_dls;
  for (unsigned i = 0; i != active_harqs; ++i) {
    h_dls.push_back(harq_ent.alloc_dl_harq(next_slot, k1, max_harq_retxs, i)->id());
  }
  slot_point pucch_slot = next_slot + k1;

  while (next_slot != pucch_slot) {
    run_slot();
  }

  // ACK received.
  for (unsigned i = 0; i != active_harqs; ++i) {
    auto h_dl = this->harq_ent.find_dl_harq(pucch_slot, i);
    ASSERT_EQ(h_dl->dl_ack_info(mac_harq_ack_report_status::ack, std::nullopt),
              dl_harq_process_view::status_update::acked);
  }

  for (unsigned i = 0; i != h_dls.size(); ++i) {
    auto h_dl = this->harq_ent.dl_harq(h_dls[i]);
    ASSERT_FALSE(h_dl.has_value());
  }
}

TEST_F(single_ue_harq_entity_harq_5bit_tester, when_5_harq_bits_are_nacks_then_all_5_active_harqs_are_updated)
{
  const unsigned active_harqs = 5, k1 = 4;

  std::vector<harq_id_t> h_dls(active_harqs);
  for (unsigned i = 0; i != active_harqs; ++i) {
    auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_harq_retxs, i);
    h_dls.push_back(h_dl->id());
  }
  slot_point pucch_slot = next_slot + k1;

  while (next_slot != pucch_slot) {
    run_slot();
  }

  // NACK received.
  for (unsigned i = 0; i != active_harqs; ++i) {
    auto h_dl_ack = this->harq_ent.find_dl_harq(pucch_slot, i);
    ASSERT_EQ(h_dl_ack->dl_ack_info(mac_harq_ack_report_status::nack, std::nullopt),
              dl_harq_process_view::status_update::nacked);
  }

  for (unsigned i = 0; i != h_dls.size(); ++i) {
    ASSERT_TRUE(this->harq_ent.dl_harq(h_dls[i]).value().has_pending_retx());
  }
}
