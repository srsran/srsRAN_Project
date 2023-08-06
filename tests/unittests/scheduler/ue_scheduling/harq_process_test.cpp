/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
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

TEST_F(dl_harq_process_tester, ack_of_empty_harq_is_noop)
{
  ASSERT_FALSE(h_dl.ack_info(0, mac_harq_ack_report_status::ack)) << "ACK of empty HARQ should fail";
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

  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  h_dl.slot_indication(++sl_tx);
  ASSERT_FALSE(h_dl.has_pending_retx(0));
  ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::nack));
  h_dl.new_retx(sl_tx, k1, 0);
  h_dl.slot_indication(++sl_tx);
  ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::nack));
  ASSERT_TRUE(h_dl.empty());
  ASSERT_FALSE(h_dl.has_pending_retx());
}

#ifdef ASSERTS_ENABLED
TEST_F(dl_harq_process_timeout_tester, when_harq_has_no_pending_retx_calling_newtx_or_retx_asserts)
{
  (void)(::testing::GTEST_FLAG(death_test_style) = "threadsafe");
  unsigned   k1 = 1, max_harq_retxs = 1;
  slot_point sl_tx{0, 0};

  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  ASSERT_TRUE(not h_dl.empty(0) and not h_dl.has_pending_retx(0));
  ASSERT_DEATH(h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0), ".*");
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
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 - 1; ++i) {
    ASSERT_FALSE(h_dl.empty());
    ASSERT_FALSE(h_dl.has_pending_retx());
    slot_indication();
  }
  ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::ack));
  ASSERT_TRUE(h_dl.empty()) << "HARQ was not reset after ACK";
  ASSERT_FALSE(h_dl.has_pending_retx()) << "HARQ was not reset after ACK";
}

TEST_P(dl_harq_process_param_tester, when_ack_rx_wait_time_elapsed_harq_is_available_for_retx)
{
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
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
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != this->max_ack_wait_slots + k1 - 1; ++i) {
    ASSERT_FALSE(h_dl.empty());
    ASSERT_FALSE(h_dl.has_pending_retx());
    slot_indication();
  }

  bool prev_ndi = h_dl.tb(0).ndi;
  ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::ack));
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  ASSERT_NE(prev_ndi, h_dl.tb(0).ndi);
}

INSTANTIATE_TEST_SUITE_P(dl_harq_param_combine,
                         dl_harq_process_param_tester,
                         testing::Combine(testing::Values(0, 1, 2, 4),   // max_retxs
                                          testing::Values(2, 4, 6, 8),   // max_ack_wait_slots
                                          testing::Values(1, 2, 4, 6))); // k1

class dl_harq_process_param_tester_dtx : public ::testing::Test
{
protected:
  dl_harq_process_param_tester_dtx() :
    max_harq_retxs(1),
    max_ack_wait_slots(12),
    k1(1),
    dl_logger(srslog::fetch_basic_logger("SCHED"), to_rnti(0x4601), to_du_cell_index(0), true),
    h_dl(to_harq_id(0), dl_logger, {timeout_handler, to_du_ue_index(0)}, max_ack_wait_slots)
  {
    srslog::init();
  }

  void slot_indication()
  {
    ++sl_tx;
    srslog::fetch_basic_logger("SCHED").set_context(sl_tx.sfn(), sl_tx.slot_index());
    h_dl.slot_indication(sl_tx);
  }

  ~dl_harq_process_param_tester_dtx() { srslog::flush(); }

  const unsigned max_harq_retxs;
  const unsigned max_ack_wait_slots;
  const unsigned shortened_ack_wait_slots{10};
  const unsigned k1;

  harq_logger                dl_logger;
  dummy_harq_timeout_handler timeout_handler;
  dl_harq_process            h_dl;
  slot_point                 sl_tx{0, 0};
};

TEST_F(dl_harq_process_param_tester_dtx, test_dtx)
{
  const unsigned dtx_slot = 1;
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    // Notify HARQ process with DTX (ACK not decoded).
    if (i == dtx_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::dtx));
    }

    // Before reaching the ack_wait_slots, the HARQ should be neither empty nor have pending reTX.
    if (i < shortened_ack_wait_slots + k1) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
      ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX);
    }
    // Once the shortened_ack_wait_slots has passed, expect pending reTXs.
    else {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_TRUE(h_dl.has_pending_retx());
      ASSERT_EQ(timeout_handler.last_ue_index, to_du_ue_index(0));
      ASSERT_TRUE(timeout_handler.last_dir_is_dl);
    }
    slot_indication();
  }
}

TEST_F(dl_harq_process_param_tester_dtx, test_dtx_ack)
{
  // DTX arrives first, then NACK.
  const unsigned dtx_slot = 1;
  const unsigned ack_slot = 2;
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    if (i == dtx_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::dtx));
    }
    if (i == ack_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::ack));
    }

    // Before ACK, the process is waiting for an ACK.
    // NOTE: The DTX will only change the ack_wait_in_slots, with no effect on pending transmissions.
    if (i < ack_slot) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
    }
    // When ACK arrives, the process will be emptied. NOTE: DTX won't do anything in this case.
    else {
      ASSERT_TRUE(h_dl.empty());
    }
    slot_indication();
  }
}

TEST_F(dl_harq_process_param_tester_dtx, test_ack_dtx)
{
  // ACK arrives first, then DTX.
  const unsigned ack_slot = 1;
  const unsigned dtx_slot = 2;
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    if (i == ack_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::ack));
    }
    if (i == dtx_slot) {
      h_dl.ack_info(0, mac_harq_ack_report_status::dtx);
    }

    // Before ACK, the process is waiting for an ACK.
    if (i < ack_slot) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
    }
    // When ACK arrives, the process will be emptied. NOTE: DTX won't do anything in this case.
    else {
      ASSERT_TRUE(h_dl.empty());
    }
    slot_indication();
  }

  ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX) << "Timeout should not expire";
}

TEST_F(dl_harq_process_param_tester_dtx, test_dtx_nack)
{
  // DTX arrives first, then NACK.
  const unsigned dtx_slot  = 1;
  const unsigned nack_slot = 2;
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    if (i == dtx_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::dtx));
    }
    if (i == nack_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::nack));
    }

    // Before NACK, the process is waiting for an ACK.
    // NOTE: The DTX will only change the ack_wait_in_slots, with no effect on pending transmissions.
    if (i < nack_slot) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
    }
    // When NACK arrives, the process has pending_retx.
    else {
      ASSERT_TRUE(h_dl.has_pending_retx());
    }
    slot_indication();
  }

  ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX) << "Timeout should not expire";
}

TEST_F(dl_harq_process_param_tester_dtx, test_nack_dtx)
{
  // NACK arrives first, then DTX.
  const unsigned nack_slot = 1;
  const unsigned dtx_slot  = 2;
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    if (i == nack_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::nack));
    }
    if (i == dtx_slot) {
      h_dl.ack_info(0, mac_harq_ack_report_status::dtx);
    }

    // Before NACK, the process is waiting for an ACK.
    if (i < nack_slot) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
    }
    // When NACK arrives, the process has pending_retx. NOTE: DTX won't do anything in this case.
    else {
      ASSERT_TRUE(h_dl.has_pending_retx());
    }
    slot_indication();
  }

  ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX) << "Timeout should not expire";
}

// Note: Sometimes, the two F1 PUCCHs (one with SR) are detected.
TEST_F(dl_harq_process_param_tester_dtx, test_nack_ack)
{
  const unsigned nack_slot = 2;
  const unsigned ack_slot  = 2;
  h_dl.new_tx(sl_tx, k1, max_harq_retxs, 0);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 + 1; ++i) {
    if (i == nack_slot) {
      ASSERT_TRUE(h_dl.ack_info(0, mac_harq_ack_report_status::nack));
    }
    if (i == ack_slot) {
      h_dl.ack_info(0, mac_harq_ack_report_status::ack);
    }

    // Before NACK, the process is waiting for an ACK.
    if (i < nack_slot) {
      ASSERT_FALSE(h_dl.empty());
      ASSERT_FALSE(h_dl.has_pending_retx());
    }
    // When NACK+ACK arrives, the process becomes empty.
    else {
      ASSERT_TRUE(h_dl.empty());
    }
    slot_indication();
  }

  ASSERT_EQ(timeout_handler.last_ue_index, INVALID_DU_UE_INDEX) << "Timeout should not expire";
}