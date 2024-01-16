/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/scheduler/ue_scheduling/harq_process.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class dummy_harq_timeout_handler : public harq_timeout_handler
{
public:
  du_ue_index_t last_ue_index  = INVALID_DU_UE_INDEX;
  bool          last_dir_is_dl = false;

  void handle_harq_timeout(du_ue_index_t ue_index, bool is_dl) override
  {
    last_ue_index  = ue_index;
    last_dir_is_dl = is_dl;
  }
};

/// Tester for different combinations of max HARQ retxs, ack wait timeouts, and k1s.
class dl_harq_process_tester : public ::testing::Test
{
protected:
  dl_harq_process_tester(unsigned max_ack_timeout_slots = 16) :
    dl_logger(srslog::fetch_basic_logger("SCHED"), to_rnti(0x4601), to_du_cell_index(0), true),
    h_dl(to_harq_id(0), dl_logger, {}, max_ack_timeout_slots)
  {
    srslog::fetch_basic_logger("SCHED").set_level(srslog::basic_levels::debug);

    srslog::init();
  }

  harq_logger     dl_logger;
  dl_harq_process h_dl;
};

TEST_F(dl_harq_process_tester, harq_starts_empty)
{
  ASSERT_TRUE(this->h_dl.empty(0));
  ASSERT_TRUE(this->h_dl.empty());
  ASSERT_FALSE(this->h_dl.has_pending_retx());
  ASSERT_FALSE(this->h_dl.has_pending_retx(0));
}

TEST_F(dl_harq_process_tester, reset_of_empty_harq_is_no_op)
{
  h_dl.reset();
  ASSERT_TRUE(h_dl.empty());
  ASSERT_TRUE(h_dl.empty(0));
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_FALSE(h_dl.has_pending_retx(0));
}

TEST_F(dl_harq_process_tester, newtx_set_harq_to_not_empty)
{
  slot_point    sl_tx{0, 0};
  vrb_interval  vrbs{5, 10};
  unsigned      k1 = 4, max_harq_retxs = 5, tbs_bytes = 1000;
  sch_mcs_index mcs = 10;

  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  ASSERT_FALSE(h_dl.empty());
  ASSERT_FALSE(h_dl.empty(0));
  ASSERT_TRUE(h_dl.empty(1));
  ASSERT_EQ(h_dl.slot_tx(), sl_tx);
  ASSERT_EQ(h_dl.slot_ack(), sl_tx + k1);
  ASSERT_EQ(h_dl.tb(0).nof_retxs, 0);
  ASSERT_EQ(h_dl.tb(0).max_nof_harq_retxs, max_harq_retxs);

  pdsch_information pdsch;
  pdsch.codewords.resize(1);
  pdsch.codewords[0].mcs_table     = srsran::pdsch_mcs_table::qam64;
  pdsch.codewords[0].mcs_index     = mcs;
  pdsch.codewords[0].tb_size_bytes = tbs_bytes;
  pdsch.rbs                        = vrbs;
  h_dl.save_alloc_params(srsran::dci_dl_rnti_config_type::c_rnti_f1_0, pdsch);
  ASSERT_EQ(h_dl.last_alloc_params().dci_cfg_type, dci_dl_rnti_config_type::c_rnti_f1_0);
  ASSERT_EQ(h_dl.last_alloc_params().rbs.type1(), vrbs);
  ASSERT_EQ(h_dl.last_alloc_params().tb[0]->mcs, mcs);
  ASSERT_EQ(h_dl.last_alloc_params().tb[0]->tbs_bytes, tbs_bytes);
}

#ifdef ASSERTS_ENABLED
TEST_F(dl_harq_process_tester, retx_of_empty_harq_asserts)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  slot_point sl_tx{0, 0};
  ASSERT_DEATH(h_dl.new_retx(sl_tx, 4, 0), ".*") << "Retxing an empty HARQ should assert";
}
#endif

TEST_F(dl_harq_process_tester, ack_of_empty_harq_is_failure)
{
  ASSERT_EQ(h_dl.ack_info(0, mac_harq_ack_report_status::ack, nullopt), dl_harq_process::status_update::error)
      << "ACK of empty HARQ should fail";
}

class dl_harq_process_timeout_tester : public dl_harq_process_tester
{
protected:
  dl_harq_process_timeout_tester() : dl_harq_process_tester(1) {}
};

TEST_F(dl_harq_process_timeout_tester, when_max_retx_exceeded_and_nack_is_received_harq_becomes_empty)
{
  unsigned   k1 = 1, max_harq_retxs = 1;
  slot_point sl_tx{0, 0};

  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  h_dl.slot_indication(++sl_tx);
  ASSERT_FALSE(h_dl.has_pending_retx(0));
  ASSERT_EQ(h_dl.ack_info(0, mac_harq_ack_report_status::nack, nullopt), dl_harq_process::status_update::nacked);
  h_dl.new_retx(sl_tx, k1, 0);
  h_dl.slot_indication(++sl_tx);
  ASSERT_EQ(h_dl.ack_info(0, mac_harq_ack_report_status::nack, nullopt), dl_harq_process::status_update::nacked);
  ASSERT_TRUE(h_dl.empty());
  ASSERT_FALSE(h_dl.has_pending_retx());
}

#ifdef ASSERTS_ENABLED
TEST_F(dl_harq_process_timeout_tester, when_harq_has_no_pending_retx_calling_newtx_or_retx_asserts)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  unsigned   k1 = 1, max_harq_retxs = 1;
  slot_point sl_tx{0, 0};

  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  ASSERT_TRUE(not h_dl.empty(0) and not h_dl.has_pending_retx(0));
  ASSERT_DEATH(h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1), ".*");
  ASSERT_DEATH(h_dl.new_retx(sl_tx, k1, 0), ".*");
}
#endif

/// Tester for different combinations of max HARQ retxs, ack wait timeouts, and k1s.
class dl_harq_process_param_tester : public ::testing::TestWithParam<std::tuple<unsigned, unsigned, unsigned>>
{
protected:
  dl_harq_process_param_tester() :
    max_harq_retxs(std::get<0>(GetParam())),
    max_ack_wait_slots(std::get<1>(GetParam())),
    k1(std::get<2>(GetParam())),
    dl_logger(srslog::fetch_basic_logger("SCHED"), to_rnti(0x4601), to_du_cell_index(0), true),
    h_dl(to_harq_id(0), dl_logger, {}, max_ack_wait_slots)
  {
    srslog::init();
  }

  void slot_indication()
  {
    ++sl_tx;
    srslog::fetch_basic_logger("SCHED").set_context(sl_tx.sfn(), sl_tx.slot_index());
    h_dl.slot_indication(sl_tx);
  }

  ~dl_harq_process_param_tester() { srslog::flush(); }

  const unsigned max_harq_retxs;
  const unsigned max_ack_wait_slots;
  const unsigned k1;

  harq_logger     dl_logger;
  dl_harq_process h_dl;
  slot_point      sl_tx{0, 0};
};

TEST_P(dl_harq_process_param_tester, when_ack_is_received_harq_is_set_as_empty)
{
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 - 1; ++i) {
    ASSERT_FALSE(h_dl.empty());
    ASSERT_FALSE(h_dl.has_pending_retx());
    slot_indication();
  }
  ASSERT_EQ(h_dl.ack_info(0, mac_harq_ack_report_status::ack, nullopt), dl_harq_process::status_update::acked);
  ASSERT_TRUE(h_dl.empty()) << "HARQ was not reset after ACK";
  ASSERT_FALSE(h_dl.has_pending_retx()) << "HARQ was not reset after ACK";
}

TEST_P(dl_harq_process_param_tester, when_ack_rx_wait_time_elapsed_harq_is_available_for_retx)
{
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  bool ndi = h_dl.tb(0).ndi;
  for (unsigned i = 0; i != this->max_ack_wait_slots + this->k1; ++i) {
    ASSERT_FALSE(h_dl.empty()) << "It is too early for HARQ to be reset";
    ASSERT_FALSE(h_dl.has_pending_retx()) << "It is too early for HARQ to be available for retx";
    ASSERT_EQ(h_dl.tb(0).nof_retxs, 0);
    slot_indication();
  }

  for (unsigned i = 0; i != this->max_harq_retxs; ++i) {
    ASSERT_FALSE(h_dl.empty()) << "It is too early for HARQ to be reset";
    ASSERT_TRUE(h_dl.has_pending_retx()) << "It is too early for HARQ to be available for retx";

    h_dl.new_retx(sl_tx, this->k1, 0);
    ASSERT_EQ(h_dl.tb(0).ndi, ndi) << "NDI should not change during retxs";
    for (unsigned j = 0; j != max_ack_wait_slots + this->k1; ++j) {
      ASSERT_FALSE(h_dl.empty()) << "It is too early for HARQ to be reset";
      ASSERT_FALSE(h_dl.has_pending_retx()) << "It is too early for HARQ to be available for retx";
      ASSERT_EQ(h_dl.tb(0).nof_retxs, i + 1) << "nof_retxs() has not been updated";
      slot_indication();
    }
  }

  ASSERT_TRUE(h_dl.empty()) << "HARQ should be automatically reset once max HARQ retxs is achieved";
}

TEST_P(dl_harq_process_param_tester, harq_newtxs_flip_ndi)
{
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  for (unsigned i = 0; i != this->max_ack_wait_slots + k1 - 1; ++i) {
    ASSERT_FALSE(h_dl.empty());
    ASSERT_FALSE(h_dl.has_pending_retx());
    slot_indication();
  }

  bool prev_ndi = h_dl.tb(0).ndi;
  ASSERT_EQ(h_dl.ack_info(0, mac_harq_ack_report_status::ack, nullopt), dl_harq_process::status_update::acked);
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
  ASSERT_NE(prev_ndi, h_dl.tb(0).ndi);
}

INSTANTIATE_TEST_SUITE_P(dl_harq_param_combine,
                         dl_harq_process_param_tester,
                         testing::Combine(testing::Values(0, 1, 2, 4),   // max_retxs
                                          testing::Values(2, 4, 6, 8),   // max_ack_wait_slots
                                          testing::Values(1, 2, 4, 6))); // k1

class base_dl_harq_process_multi_harq_ack_test
{
public:
  base_dl_harq_process_multi_harq_ack_test() :
    dl_logger(srslog::fetch_basic_logger("SCHED"), to_rnti(0x4601), to_du_cell_index(0), true),
    h_dl(to_harq_id(0), dl_logger, {timeout_handler, to_du_ue_index(0)}, max_ack_wait_slots)
  {
    srslog::init();

    // Allocate HARQ expecting two PUCCHs.
    h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0, 15, 1);
    h_dl.increment_pucch_counter();
    h_dl.increment_pucch_counter();
  }
  ~base_dl_harq_process_multi_harq_ack_test() { srslog::flush(); }

  void slot_indication()
  {
    ++sl_tx;
    srslog::fetch_basic_logger("SCHED").set_context(sl_tx.sfn(), sl_tx.slot_index());
    h_dl.slot_indication(sl_tx);
  }

  static mac_harq_ack_report_status get_random_harq_ack()
  {
    return static_cast<mac_harq_ack_report_status>(test_rgen::uniform_int<unsigned>(0, 2));
  }

  const unsigned max_harq_retxs     = 1;
  const unsigned max_ack_wait_slots = 12;
  const unsigned shortened_ack_wait_slots{4};
  const unsigned k1              = 1;
  const unsigned first_ack_slot  = 1;
  const unsigned second_ack_slot = 2;

  harq_logger                dl_logger;
  dummy_harq_timeout_handler timeout_handler;
  dl_harq_process            h_dl;
  slot_point                 sl_tx{0, 0};
};

static float random_snr()
{
  return static_cast<float>(std::uniform_real_distribution<float>{-20.0F, 30.0F}(test_rgen::get()));
}

class dl_harq_process_multi_harq_ack_timeout_test : public base_dl_harq_process_multi_harq_ack_test,
                                                    public ::testing::Test
{};

TEST_F(dl_harq_process_multi_harq_ack_timeout_test,
       when_one_harq_ack_is_received_and_other_goes_missing_then_harq_timeout_is_shortened)
{
  const mac_harq_ack_report_status ack_val = get_random_harq_ack();

  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    // Notify HARQ process with DTX (ACK not decoded).
    if (i == first_ack_slot) {
      ASSERT_EQ(h_dl.ack_info(0, ack_val, random_snr()), dl_harq_process::status_update::no_update);
    }

    // Before reaching the ack_wait_slots, the HARQ should be neither empty nor have pending reTX.
    if (i < shortened_ack_wait_slots + k1) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
      ASSERT_TRUE(h_dl.is_waiting_ack());
      ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX);
    }
    // Once the shortened_ack_wait_slots has passed, expect pending reTXs.
    else {
      if (ack_val == srsran::mac_harq_ack_report_status::ack) {
        ASSERT_TRUE(h_dl.empty());
        ASSERT_NE(timeout_handler.last_ue_index, to_du_ue_index(0));
      } else {
        ASSERT_TRUE(h_dl.has_pending_retx());
        ASSERT_EQ(timeout_handler.last_ue_index, to_du_ue_index(0));
        ASSERT_TRUE(timeout_handler.last_dir_is_dl);
      }
      break;
    }
    slot_indication();
  }
}

struct multi_ack_test_params {
  std::array<uint8_t, 2> ack;
  std::array<float, 2>   snr;
  bool                   outcome;
};

void PrintTo(const multi_ack_test_params& params, ::std::ostream* os)
{
  *os << fmt::format("{{ack={} snr={:.2}}} + {{ack={} snr={:.2}}} -> outcome={}",
                     params.ack[0],
                     params.snr[0],
                     params.ack[1],
                     params.snr[1],
                     params.outcome ? "ACK" : "NACK");
}

class dl_harq_process_multi_harq_ack_test : public base_dl_harq_process_multi_harq_ack_test,
                                            public ::testing::TestWithParam<multi_ack_test_params>
{};

TEST_P(dl_harq_process_multi_harq_ack_test, two_harq_acks_received)
{
  auto params = GetParam();

  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    if (i == first_ack_slot) {
      ASSERT_EQ(h_dl.ack_info(0, static_cast<mac_harq_ack_report_status>(params.ack[0]), params.snr[0]),
                dl_harq_process::status_update::no_update);
    }
    if (i == second_ack_slot) {
      ASSERT_EQ(h_dl.ack_info(0, static_cast<mac_harq_ack_report_status>(params.ack[1]), params.snr[1]),
                params.outcome ? dl_harq_process::status_update::acked : dl_harq_process::status_update::nacked);
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
    slot_indication();
  }

  ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX) << "Timeout should not expire";
}

INSTANTIATE_TEST_SUITE_P(
    dl_harq_process_tester,
    dl_harq_process_multi_harq_ack_test,
    testing::Values(multi_ack_test_params{.ack = {2, 1}, .snr = {random_snr(), random_snr()}, .outcome = true},
                    multi_ack_test_params{.ack = {1, 2}, .snr = {random_snr(), random_snr()}, .outcome = true},
                    multi_ack_test_params{.ack = {2, 0}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    multi_ack_test_params{.ack = {0, 2}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    multi_ack_test_params{.ack = {0, 1}, .snr = {10.0, 11.0}, .outcome = true},
                    multi_ack_test_params{.ack = {0, 1}, .snr = {10.0, 9.0}, .outcome = false},
                    multi_ack_test_params{.ack = {2, 2}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    multi_ack_test_params{.ack = {0, 0}, .snr = {random_snr(), random_snr()}, .outcome = false},
                    multi_ack_test_params{.ack = {1, 1}, .snr = {random_snr(), random_snr()}, .outcome = true}));
