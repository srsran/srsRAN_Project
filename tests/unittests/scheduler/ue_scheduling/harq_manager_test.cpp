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
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace {

// Generate random PUCCH SNR value.
float random_snr()
{
  return static_cast<float>(std::uniform_real_distribution<float>{-20.0F, 30.0F}(test_rgen::get()));
}

mac_harq_ack_report_status get_random_harq_ack()
{
  return static_cast<mac_harq_ack_report_status>(test_rgen::uniform_int<unsigned>(0, 2));
}

pdsch_information make_dummy_pdsch_info()
{
  pdsch_information pdsch;
  pdsch.codewords.resize(1);
  pdsch.codewords[0].mcs_table     = srsran::pdsch_mcs_table::qam64;
  pdsch.codewords[0].mcs_index     = 10;
  pdsch.codewords[0].tb_size_bytes = 10000;
  pdsch.rbs                        = vrb_interval{5, 10};
  return pdsch;
}

pusch_information make_dummy_pusch_info()
{
  pusch_information pusch;
  pusch.mcs_table     = pusch_mcs_table::qam64;
  pusch.mcs_index     = 10;
  pusch.tb_size_bytes = 10000;
  pusch.rbs           = vrb_interval{5, 10};
  return pusch;
}

// Dummy HARQ timeout recorder
class dummy_harq_timeout_handler
{
public:
  du_ue_index_t last_ue_index  = INVALID_DU_UE_INDEX;
  bool          last_dir_is_dl = false;
  bool          last_was_ack   = false;

  void handle_harq_timeout(du_ue_index_t ue_index, bool is_dl, bool ack)
  {
    last_ue_index  = ue_index;
    last_dir_is_dl = is_dl;
    last_was_ack   = ack;
  }

  std::unique_ptr<harq_timeout_notifier> make_notifier()
  {
    class dummy_notifier : public harq_timeout_notifier
    {
    public:
      dummy_notifier(dummy_harq_timeout_handler& parent_) : parent(parent_) {}

      void on_harq_timeout(du_ue_index_t ue_index, bool is_dl, bool ack) override
      {
        parent.handle_harq_timeout(ue_index, is_dl, ack);
      }

    private:
      dummy_harq_timeout_handler& parent;
    };

    return std::make_unique<dummy_notifier>(*this);
  }
};

// Base test class that instantiates a cell HARQ manager.
class base_harq_manager_test
{
protected:
  base_harq_manager_test(unsigned nof_ues) : cell_harqs(nof_ues, timeout_handler.make_notifier(), max_ack_wait_timeout)
  {
  }

  const unsigned             max_ack_wait_timeout = 16;
  dummy_harq_timeout_handler timeout_handler;

  cell_harq_manager cell_harqs;

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
};

class base_single_harq_entity_test : public base_harq_manager_test
{
protected:
  base_single_harq_entity_test() : base_harq_manager_test(1) {}

  void run_slot()
  {
    ++next_slot;
    srslog::fetch_basic_logger("SCHED").set_context(next_slot.sfn(), next_slot.slot_index());
    cell_harqs.slot_indication(next_slot);
  }

  const du_ue_index_t   ue_index  = to_du_ue_index(0);
  const rnti_t          rnti      = to_rnti(0x4601);
  const unsigned        nof_harqs = 8;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);

  unsigned max_retxs = 4;
  unsigned k1        = 4;
  unsigned k2        = 6;
};

} // namespace

// Test for multiple UEs managed by a single HARQ manager instance.
class multi_ue_harq_manager_test : public base_harq_manager_test, public ::testing::Test
{
protected:
  multi_ue_harq_manager_test() : base_harq_manager_test(max_ues) {}

  constexpr static unsigned max_ues   = 4;
  const unsigned            nof_harqs = 8;
};

// Test for a single UE HARQ entity.
class single_ue_harq_entity_test : public base_single_harq_entity_test, public ::testing::Test
{};

// Test suite for a single HARQ process.
class single_harq_process_test : public base_single_harq_entity_test, public ::testing::Test
{
protected:
  single_harq_process_test()
  {
    pdsch_info = make_dummy_pdsch_info();
    dl_harq_sched_context harq_ctxt{dci_dl_rnti_config_type::c_rnti_f1_0};
    h_dl.save_grant_params(harq_ctxt, pdsch_info);
    pusch_info = make_dummy_pusch_info();
    ul_harq_sched_context ul_harq_ctxt{dci_ul_rnti_config_type::c_rnti_f0_0};
    h_ul.save_grant_params(ul_harq_ctxt, pusch_info);
  }

  pdsch_information    pdsch_info;
  pusch_information    pusch_info;
  dl_harq_process_view h_dl{harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0).value()};
  ul_harq_process_view h_ul{harq_ent.alloc_ul_harq(next_slot + k2, max_retxs).value()};
};

class dl_harq_process_multi_pucch_test : public base_single_harq_entity_test, public ::testing::Test
{
protected:
  dl_harq_process_multi_pucch_test()
  {
    // Expect two PUCCHs.
    h_dl.increment_pucch_counter();
    h_dl.increment_pucch_counter();
  }

  dl_harq_process_view h_dl{harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0).value()};
};

// Parameters of test with 2 PUCCHs - dl_harq_process_two_pucch_param_test
struct two_ack_test_params {
  std::array<uint8_t, 2> ack;
  std::array<float, 2>   snr;
  bool                   outcome;
};

void PrintTo(const two_ack_test_params& params, ::std::ostream* os)
{
  *os << fmt::format("{{ack={} snr={:.2}}} + {{ack={} snr={:.2}}} -> outcome={}",
                     params.ack[0],
                     params.snr[0],
                     params.ack[1],
                     params.snr[1],
                     params.outcome ? "ACK" : "NACK");
}

class dl_harq_process_two_pucch_param_test : public base_single_harq_entity_test,
                                             public ::testing::TestWithParam<two_ack_test_params>
{
protected:
  dl_harq_process_two_pucch_param_test()
  {
    // Expect two PUCCHs.
    h_dl.increment_pucch_counter();
    h_dl.increment_pucch_counter();
  }

  dl_harq_process_view h_dl{harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0).value()};
};

// HARQ process tests

TEST_F(single_harq_process_test, when_harq_is_allocated_then_it_enters_waiting_ack_state)
{
  ASSERT_TRUE(h_dl.is_waiting_ack());
  ASSERT_TRUE(h_ul.is_waiting_ack());
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_FALSE(h_ul.has_pending_retx());
}

TEST_F(single_harq_process_test, when_harq_is_allocated_then_harq_params_have_correct_values)
{
  ASSERT_EQ(h_dl.pdsch_slot(), next_slot);
  ASSERT_EQ(h_dl.uci_slot(), next_slot + k1);
  ASSERT_EQ(h_dl.max_nof_retxs(), max_retxs);
  ASSERT_EQ(h_dl.nof_retxs(), 0);
  ASSERT_EQ(h_ul.pusch_slot(), next_slot + k2);
  ASSERT_EQ(h_ul.max_nof_retxs(), max_retxs);
  ASSERT_EQ(h_ul.nof_retxs(), 0);
}

TEST_F(single_harq_process_test, when_harq_is_allocated_then_harq_grant_params_have_correct_values)
{
  ASSERT_EQ(h_dl.get_grant_params().mcs, pdsch_info.codewords[0].mcs_index);
  ASSERT_EQ(h_dl.get_grant_params().mcs_table, pdsch_info.codewords[0].mcs_table);
  ASSERT_EQ(h_dl.get_grant_params().tbs_bytes, pdsch_info.codewords[0].tb_size_bytes);
  ASSERT_EQ(h_dl.get_grant_params().rbs.type1(), pdsch_info.rbs.type1());
  ASSERT_EQ(h_dl.get_grant_params().dci_cfg_type, dci_dl_rnti_config_type::c_rnti_f1_0);
}

TEST_F(single_harq_process_test, positive_ack_sets_harq_to_empty)
{
  float pucch_snr = 5;
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, pucch_snr), dl_harq_process_view::status_update::acked);
  ASSERT_FALSE(h_dl.is_waiting_ack());
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_EQ(h_ul.ul_crc_info(true), pusch_info.tb_size_bytes);
  ASSERT_FALSE(h_ul.is_waiting_ack());
  ASSERT_FALSE(h_ul.has_pending_retx());
}

TEST_F(single_harq_process_test, negative_ack_sets_harq_to_pending_retx)
{
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_FALSE(h_dl.is_waiting_ack());
  ASSERT_TRUE(h_dl.has_pending_retx());
  ASSERT_EQ(harq_ent.find_ul_harq(next_slot + k2), h_ul);
  ASSERT_EQ(h_ul.ul_crc_info(false), 0);
  ASSERT_FALSE(h_ul.is_waiting_ack());
  ASSERT_TRUE(h_ul.has_pending_retx());
}

TEST_F(single_harq_process_test, ack_of_empty_harq_is_failure)
{
  float pucch_snr = 5;
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, pucch_snr), dl_harq_process_view::status_update::acked);
  ASSERT_GE(h_ul.ul_crc_info(true), 0);

  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, pucch_snr), dl_harq_process_view::status_update::error);
  ASSERT_LT(h_ul.ul_crc_info(true), 0);
}

TEST_F(single_harq_process_test, retx_of_empty_harq_is_failure)
{
  float pucch_snr = 5;
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, pucch_snr), dl_harq_process_view::status_update::acked);
  ASSERT_GE(h_ul.ul_crc_info(true), 0);

  ASSERT_FALSE(h_dl.new_retx(next_slot, k1, 0));
  ASSERT_FALSE(h_ul.new_retx(next_slot + k2));
}

TEST_F(single_harq_process_test, retx_of_harq_waiting_ack_is_failure)
{
  ASSERT_FALSE(h_dl.new_retx(next_slot, k1, 0));
  ASSERT_FALSE(h_ul.new_retx(next_slot + k2));
}

TEST_F(single_harq_process_test, when_max_retxs_reached_then_harq_becomes_empty)
{
  bool old_dl_ndi = h_dl.ndi();
  bool old_ul_ndi = h_ul.ndi();
  for (unsigned i = 0; i != max_retxs; ++i) {
    ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
    ASSERT_EQ(h_ul.ul_crc_info(false), 0);
    ASSERT_FALSE(h_dl.is_waiting_ack());
    ASSERT_FALSE(h_ul.is_waiting_ack());
    ASSERT_TRUE(h_dl.has_pending_retx());
    ASSERT_TRUE(h_ul.has_pending_retx());

    run_slot();
    ASSERT_TRUE(h_dl.new_retx(next_slot, k1, 0));
    ASSERT_TRUE(h_ul.new_retx(next_slot + k2));
    ASSERT_EQ(h_dl.nof_retxs(), i + 1);
    ASSERT_EQ(h_ul.nof_retxs(), i + 1);
    ASSERT_NE(old_dl_ndi, h_dl.ndi());
    ASSERT_NE(old_ul_ndi, h_ul.ndi());
    old_dl_ndi = h_dl.ndi();
    old_ul_ndi = h_dl.ndi();
  }
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_EQ(h_ul.ul_crc_info(false), 0);
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_FALSE(h_ul.has_pending_retx());
  ASSERT_FALSE(h_dl.is_waiting_ack());
  ASSERT_FALSE(h_ul.is_waiting_ack());
}

TEST_F(single_harq_process_test, when_newtx_after_ack_then_ndi_flips)
{
  float pucch_snr = 5;
  bool  dl_ndi = h_dl.ndi(), ul_ndi = h_ul.ndi();
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, pucch_snr), dl_harq_process_view::status_update::acked);
  ASSERT_GE(h_ul.ul_crc_info(true), 0);

  h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0).value();
  h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs).value();
  ASSERT_EQ(h_dl.nof_retxs(), 0);
  ASSERT_EQ(h_ul.nof_retxs(), 0);
  ASSERT_NE(dl_ndi, h_dl.ndi());
  ASSERT_NE(ul_ndi, h_ul.ndi());
}

TEST_F(single_harq_process_test, when_ack_wait_timeout_reached_then_harq_is_available_for_newtx)
{
  harq_id_t h_dl_id = h_dl.id(), h_ul_id = h_ul.id();
  bool      dl_ndi = h_dl.ndi(), ul_ndi = h_ul.ndi();
  for (unsigned i = 0; i != this->max_ack_wait_timeout + k1; ++i) {
    ASSERT_TRUE(h_dl.is_waiting_ack() and not h_dl.has_pending_retx());
    ASSERT_TRUE(h_ul.is_waiting_ack() and not h_ul.has_pending_retx());
    run_slot();
  }
  // Note: k1 < k2, so there is an intermediate state where h_dl timed out but h_ul didn't.
  ASSERT_TRUE(not h_dl.is_waiting_ack() and not h_dl.has_pending_retx());
  ASSERT_FALSE(harq_ent.dl_harq(h_dl_id).has_value());
  for (unsigned i = 0; i != (k2 - k1); ++i) {
    ASSERT_TRUE(h_ul.is_waiting_ack() and not h_ul.has_pending_retx());
    run_slot();
  }
  ASSERT_TRUE(not h_ul.is_waiting_ack() and not h_ul.has_pending_retx());
  ASSERT_FALSE(harq_ent.ul_harq(h_ul_id).has_value());

  h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs - 1, 0).value();
  h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs - 1).value();
  ASSERT_TRUE(h_dl.is_waiting_ack() and not h_dl.has_pending_retx());
  ASSERT_TRUE(h_ul.is_waiting_ack() and not h_ul.has_pending_retx());
  ASSERT_EQ(h_dl.nof_retxs(), 0);
  ASSERT_EQ(h_dl.max_nof_retxs(), max_retxs - 1);
  ASSERT_EQ(h_ul.nof_retxs(), 0);
  ASSERT_EQ(h_ul.max_nof_retxs(), max_retxs - 1);
  if (h_dl.id() == h_dl_id) {
    ASSERT_NE(dl_ndi, h_dl.ndi());
  }
  if (h_ul.id() == h_ul_id) {
    ASSERT_NE(ul_ndi, h_ul.ndi());
  }
}

// DL HARQ process with multi PUCCH test

TEST_F(dl_harq_process_multi_pucch_test, when_dtx_received_after_ack_then_dtx_is_ignored)
{
  slot_point pucch_slot = next_slot + k1;
  while (next_slot != pucch_slot) {
    run_slot();
  }

  // ACK received.
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, std::nullopt),
            dl_harq_process_view::status_update::no_update);

  // DTX received one slot late.
  run_slot();
  ASSERT_TRUE(h_dl.is_waiting_ack());
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::dtx, std::nullopt),
            dl_harq_process_view::status_update::acked);
}

// Note: When two F1 PUCCHs are decoded (one with SR and the other without), there is a small chance that none of them
// are DTX.
TEST_F(dl_harq_process_multi_pucch_test, when_stronger_ack_received_after_nack_then_process_becomes_empty)
{
  slot_point pucch_slot = next_slot + k1;
  while (next_slot != pucch_slot) {
    run_slot();
  }

  // NACK received.
  harq_id_t h_id = h_dl.id();
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::nack, 1.0F), dl_harq_process_view::status_update::no_update);

  // ACK received.
  ASSERT_EQ(harq_ent.dl_harq(h_id), h_dl);
  ASSERT_TRUE(h_dl.is_waiting_ack());
  ASSERT_EQ(h_dl.dl_ack_info(mac_harq_ack_report_status::ack, 2.0F), dl_harq_process_view::status_update::acked);

  // HARQ should be empty.
  ASSERT_FALSE(harq_ent.dl_harq(h_id).has_value());
}

TEST_F(dl_harq_process_multi_pucch_test,
       when_one_harq_ack_is_received_and_other_goes_missing_then_harq_timeout_is_shortened)
{
  const mac_harq_ack_report_status ack_val        = get_random_harq_ack();
  const unsigned                   first_ack_slot = 1;

  for (unsigned i = 0; i != this->max_ack_wait_timeout + k1 + 1; ++i) {
    // Notify HARQ process with DTX (ACK not decoded).
    if (i == first_ack_slot) {
      ASSERT_EQ(h_dl.dl_ack_info(ack_val, random_snr()), dl_harq_process_view::status_update::no_update);
    }

    // Before reaching the ack_wait_slots, the HARQ should be neither empty nor have pending reTX.
    if (i < cell_harq_manager::SHORT_ACK_TIMEOUT_DTX + first_ack_slot) {
      ASSERT_FALSE(h_dl.has_pending_retx());
      ASSERT_TRUE(h_dl.is_waiting_ack());
      ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX);
    }
    // Once the shortened_ack_wait_slots has passed, expect HARQ to be reset.
    else {
      ASSERT_TRUE(h_dl.empty());
      ASSERT_EQ(timeout_handler.last_ue_index, to_du_ue_index(0));
      ASSERT_TRUE(timeout_handler.last_dir_is_dl);
      if (ack_val == srsran::mac_harq_ack_report_status::ack) {
        ASSERT_TRUE(timeout_handler.last_was_ack);
      } else {
        // In case of NACK/DTX, the HARQ should report the timeout.
        ASSERT_FALSE(timeout_handler.last_was_ack);
      }
      break;
    }
    run_slot();
  }
}

TEST_P(dl_harq_process_two_pucch_param_test, two_harq_acks_received)
{
  auto     params         = GetParam();
  unsigned first_ack_slot = 1, second_ack_slot = 2;

  for (unsigned i = 0; i != max_ack_wait_timeout + k1 + 1; ++i) {
    if (i == first_ack_slot) {
      ASSERT_EQ(h_dl.dl_ack_info(static_cast<mac_harq_ack_report_status>(params.ack[0]), params.snr[0]),
                dl_harq_process_view::status_update::no_update);
    }
    if (i == second_ack_slot) {
      ASSERT_EQ(h_dl.dl_ack_info(static_cast<mac_harq_ack_report_status>(params.ack[1]), params.snr[1]),
                params.outcome ? dl_harq_process_view::status_update::acked
                               : dl_harq_process_view::status_update::nacked);
    }

    if (i < second_ack_slot) {
      // Before second HARQ-ACK, the process is waiting for an ACK.
      ASSERT_TRUE(h_dl.is_waiting_ack());
    } else {
      // When second HARQ-ACK arrives, the process should be set as either empty or pending reTX.
      ASSERT_FALSE(h_dl.is_waiting_ack());
      if (params.outcome) {
        ASSERT_TRUE(h_dl.empty());
      } else {
        ASSERT_TRUE(h_dl.has_pending_retx());
      }
      break;
    }
    run_slot();
  }

  ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX) << "Timeout should not expire";
}

INSTANTIATE_TEST_SUITE_P(
    dl_harq_process_test,
    dl_harq_process_two_pucch_param_test,
    testing::Values(two_ack_test_params{.ack = {2, 1}, .snr = {random_snr(), random_snr()}, .outcome = true},
                    two_ack_test_params{.ack = {1, 2}, .snr = {random_snr(), random_snr()}, .outcome = true},
                    two_ack_test_params{.ack = {2, 0}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    two_ack_test_params{.ack = {0, 2}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    two_ack_test_params{.ack = {0, 1}, .snr = {10.0, 11.0}, .outcome = true},
                    two_ack_test_params{.ack = {0, 1}, .snr = {10.0, 9.0}, .outcome = false},
                    two_ack_test_params{.ack = {2, 2}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    two_ack_test_params{.ack = {0, 0}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    two_ack_test_params{.ack = {1, 1}, .snr = {random_snr(), random_snr()}, .outcome = true}));

// HARQ entity tests

TEST_F(single_ue_harq_entity_test, when_harq_entity_is_created_all_harqs_are_empty)
{
  ASSERT_EQ(harq_ent.nof_dl_harqs(), this->nof_harqs);
  ASSERT_EQ(harq_ent.nof_ul_harqs(), this->nof_harqs);
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
  ASSERT_TRUE(harq_ent.has_empty_dl_harqs());
  ASSERT_TRUE(harq_ent.has_empty_ul_harqs());
  for (unsigned i = 0; i != harq_ent.nof_dl_harqs(); ++i) {
    ASSERT_FALSE(harq_ent.dl_harq(to_harq_id(i)).has_value());
    ASSERT_FALSE(harq_ent.ul_harq(to_harq_id(i)).has_value());
  }
}

TEST_F(single_ue_harq_entity_test, when_harq_is_allocated_then_harq_entity_finds_harq_in_waiting_ack_state)
{
  auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto h_ul = harq_ent.alloc_ul_harq(next_slot, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), h_dl);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), h_ul);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
}

TEST_F(single_ue_harq_entity_test, when_harq_is_nacked_then_harq_entity_finds_harq_with_pending_retx)
{
  auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto h_ul = harq_ent.alloc_ul_harq(next_slot, max_retxs);
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), h_dl);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), h_ul);
}

TEST_F(single_ue_harq_entity_test, when_all_harqs_are_allocated_harq_entity_cannot_find_empty_harq)
{
  ASSERT_EQ(harq_ent.nof_dl_harqs(), nof_harqs);
  ASSERT_EQ(harq_ent.nof_ul_harqs(), nof_harqs);

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

TEST_F(single_ue_harq_entity_test, when_max_retxs_reached_then_harq_entity_does_not_find_pending_retx)
{
  auto h_dl = harq_ent.alloc_dl_harq(next_slot, k1, max_retxs, 0);
  auto h_ul = harq_ent.alloc_ul_harq(next_slot + k2, max_retxs);
  ASSERT_TRUE(h_dl.has_value());
  ASSERT_TRUE(h_ul.has_value());
  for (unsigned i = 0; i != max_retxs; ++i) {
    ASSERT_EQ(harq_ent.find_dl_harq(next_slot + k1, 0), h_dl);
    ASSERT_EQ(harq_ent.find_ul_harq(next_slot + k2), h_ul);
    ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5),
              dl_harq_process_view::status_update::nacked);
    ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
    ASSERT_EQ(harq_ent.find_pending_dl_retx(), h_dl);
    ASSERT_EQ(harq_ent.find_pending_ul_retx(), h_ul);
    ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
    ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);

    run_slot();
    ASSERT_TRUE(h_dl.value().new_retx(next_slot, k1, 0));
    ASSERT_TRUE(h_ul.value().new_retx(next_slot + k2));
    ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), h_dl);
    ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), h_ul);
    ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
    ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
  }
  ASSERT_EQ(h_dl.value().dl_ack_info(mac_harq_ack_report_status::nack, 5), dl_harq_process_view::status_update::nacked);
  ASSERT_EQ(h_ul.value().ul_crc_info(false), 0);
  ASSERT_EQ(harq_ent.find_dl_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_ul_harq_waiting_ack(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_dl_retx(), std::nullopt);
  ASSERT_EQ(harq_ent.find_pending_ul_retx(), std::nullopt);
}

TEST_F(single_ue_harq_entity_test, after_max_ack_wait_timeout_dl_harqs_are_available_for_newtx)
{
  k2 = 4;
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

TEST_F(multi_ue_harq_manager_test, when_ue_harq_entity_is_created_or_reset_then_cell_harq_manager_is_updated)
{
  for (unsigned i = 0; i != this->max_ues; ++i) {
    ASSERT_FALSE(cell_harqs.contains(to_du_ue_index(i)));
  }
  std::vector<unique_ue_harq_entity> harq_ents;
  for (unsigned i = 0; i != this->max_ues; ++i) {
    harq_ents.push_back(cell_harqs.add_ue(to_du_ue_index(i), to_rnti(0x4601 + i), nof_harqs, nof_harqs));
    ASSERT_TRUE(cell_harqs.contains(to_du_ue_index(i)));
  }
  for (unsigned i = 0; i != this->max_ues; ++i) {
    harq_ents[i].reset();
    ASSERT_FALSE(cell_harqs.contains(to_du_ue_index(i)));
  }
}

class single_ue_dl_harq_ack_test : public single_ue_harq_entity_test
{
protected:
  const unsigned        nof_harqs = 8;
  unique_ue_harq_entity harq_ent  = cell_harqs.add_ue(ue_index, rnti, nof_harqs, nof_harqs);
};

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
