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

void set_dl_newtx(dl_harq_process& h_dl, slot_point sl_tx, unsigned k1, unsigned max_harq_retx)
{
  h_dl.new_tx(dci_dl_rnti_config_type::c_rnti_f1_0,
              to_bwp_id(0),
              sl_tx,
              k1,
              prb_interval{5, 10},
              sch_mcs_index{5},
              100,
              max_harq_retx);
}

TEST(dl_harq_process, harq_starts_empty)
{
  dl_harq_process h_dl(to_harq_id(0));
  ASSERT_TRUE(h_dl.empty(0));
  ASSERT_TRUE(h_dl.empty());
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_FALSE(h_dl.has_pending_retx(0));
}

TEST(dl_harq_process, reset_of_empty_harq_is_no_op)
{
  dl_harq_process h_dl(to_harq_id(0));

  h_dl.reset();
  ASSERT_TRUE(h_dl.empty());
  ASSERT_TRUE(h_dl.empty(0));
  ASSERT_FALSE(h_dl.has_pending_retx());
  ASSERT_FALSE(h_dl.has_pending_retx(0));
}

TEST(dl_harq_process, newtx_set_harq_to_not_empty)
{
  dl_harq_process h_dl(to_harq_id(0));
  slot_point      sl_tx{0, 0};
  prb_interval    prbs{5, 10};
  unsigned        k1 = 4, max_harq_retxs = 5, tbs_bytes = 1000;
  sch_mcs_index   mcs = 10;

  h_dl.new_tx(dci_dl_rnti_config_type::c_rnti_f1_0, to_bwp_id(0), sl_tx, k1, prbs, mcs, tbs_bytes, max_harq_retxs);
  ASSERT_FALSE(h_dl.empty());
  ASSERT_FALSE(h_dl.empty(0));
  ASSERT_TRUE(h_dl.empty(1));
  ASSERT_EQ(h_dl.slot_tx(), sl_tx);
  ASSERT_EQ(h_dl.slot_ack(), sl_tx + k1);
  ASSERT_EQ(h_dl.freq_ra().prbs(), prbs);
  ASSERT_EQ(h_dl.tb(0).max_nof_harq_retxs, max_harq_retxs);
  ASSERT_EQ(h_dl.tb(0).mcs, mcs);
  ASSERT_EQ(h_dl.tb(0).tbs_bytes, tbs_bytes);
  ASSERT_EQ(h_dl.tb(0).nof_retxs, 0);
}

TEST(dl_harq_process, retx_of_empty_harq_asserts)
{
  dl_harq_process h_dl(to_harq_id(0));
  slot_point      sl_tx{0, 0};
  ASSERT_DEATH(h_dl.new_retx(to_bwp_id(0), sl_tx, 4, prb_interval{5, 10}, 5), ".*");
}

TEST(dl_harq_process, ack_of_empty_harq_is_noop)
{
  dl_harq_process h_dl(to_harq_id(0));

  ASSERT_TRUE(h_dl.ack_info(0, true) < 0) << "ACK of empty HARQ should fail";
}

TEST(dl_harq_process, when_max_retx_exceeded_and_nack_is_received_harq_becomes_empty)
{
  unsigned        max_ack_wait_slots = 1, k1 = 1, max_harq_retxs = 1;
  dl_harq_process h_dl(to_harq_id(0), max_ack_wait_slots);
  slot_point      sl_tx{0, 0};

  set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs);
  h_dl.slot_indication(++sl_tx);
  ASSERT_FALSE(h_dl.has_pending_retx(0));
  ASSERT_EQ(h_dl.ack_info(0, false), 0);
  h_dl.new_retx(to_bwp_id(0), sl_tx, k1, prb_interval{4, 6}, 5);
  h_dl.slot_indication(++sl_tx);
  ASSERT_EQ(h_dl.ack_info(0, false), 0);
  ASSERT_TRUE(h_dl.empty());
  ASSERT_FALSE(h_dl.has_pending_retx());
}

TEST(dl_harq_process, when_harq_has_no_pending_retx_calling_newtx_or_retx_asserts)
{
  unsigned        max_ack_wait_slots = 1, k1 = 1, max_harq_retxs = 1;
  dl_harq_process h_dl(to_harq_id(0), max_ack_wait_slots);
  slot_point      sl_tx{0, 0};

  set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs);
  ASSERT_TRUE(not h_dl.empty(0) and not h_dl.has_pending_retx(0));
  ASSERT_DEATH(set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs), ".*");
  ASSERT_DEATH(h_dl.new_retx(to_bwp_id(0), sl_tx, k1, h_dl.freq_ra(), h_dl.tb(0).mcs), ".*");
}

/// Tester for different combinations of max HARQ retxs, ack wait timeouts, and k1s.
class dl_harq_process_tester : public ::testing::TestWithParam<std::tuple<unsigned, unsigned, unsigned>>
{
protected:
  dl_harq_process_tester() :
    max_harq_retxs(std::get<0>(GetParam())),
    max_ack_wait_slots(std::get<1>(GetParam())),
    k1(std::get<2>(GetParam())),
    h_dl(to_harq_id(0), max_ack_wait_slots)
  {
  }

  const unsigned max_harq_retxs;
  const unsigned max_ack_wait_slots;
  const unsigned k1;

  dl_harq_process h_dl;
  slot_point      sl_tx{0, 0};
};

TEST_P(dl_harq_process_tester, when_ack_is_received_harq_is_set_as_empty)
{
  set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs);
  for (unsigned i = 0; i != max_ack_wait_slots + k1 - 1; ++i) {
    ASSERT_FALSE(h_dl.empty());
    ASSERT_FALSE(h_dl.has_pending_retx());
    h_dl.slot_indication(++sl_tx);
  }
  ASSERT_TRUE(h_dl.ack_info(0, true) >= 0);
  ASSERT_TRUE(h_dl.empty()) << "HARQ was not reset after ACK";
  ASSERT_FALSE(h_dl.has_pending_retx()) << "HARQ was not reset after ACK";
}

TEST_P(dl_harq_process_tester, when_ack_rx_wait_time_elapsed_harq_is_available_for_retx)
{
  set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs);
  bool ndi = h_dl.tb(0).ndi;
  for (unsigned i = 0; i != this->max_ack_wait_slots + this->k1; ++i) {
    ASSERT_FALSE(h_dl.empty()) << "It is too early for HARQ to be reset";
    ASSERT_FALSE(h_dl.has_pending_retx()) << "It is too early for HARQ to be available for retx";
    ASSERT_EQ(h_dl.tb(0).nof_retxs, 0);
    h_dl.slot_indication(++sl_tx);
  }

  for (unsigned i = 0; i != this->max_harq_retxs; ++i) {
    ASSERT_FALSE(h_dl.empty()) << "It is too early for HARQ to be reset";
    ASSERT_TRUE(h_dl.has_pending_retx()) << "It is too early for HARQ to be available for retx";

    h_dl.new_retx(to_bwp_id(0), sl_tx, this->k1, h_dl.freq_ra(), h_dl.tb(0).mcs);
    ASSERT_EQ(h_dl.tb(0).ndi, ndi) << "NDI should not change during retxs";
    for (unsigned j = 0; j != max_ack_wait_slots + this->k1; ++j) {
      ASSERT_FALSE(h_dl.empty()) << "It is too early for HARQ to be reset";
      ASSERT_FALSE(h_dl.has_pending_retx()) << "It is too early for HARQ to be available for retx";
      ASSERT_EQ(h_dl.tb(0).nof_retxs, i + 1) << "nof_retxs() has not been updated";
      h_dl.slot_indication(++sl_tx);
    }
  }

  ASSERT_TRUE(h_dl.empty()) << "HARQ should be automatically reset once max HARQ retxs is achieved";
}

TEST_P(dl_harq_process_tester, harq_newtxs_flip_ndi)
{
  set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs);
  for (unsigned i = 0; i != this->max_ack_wait_slots + k1 - 1; ++i) {
    ASSERT_FALSE(h_dl.empty());
    ASSERT_FALSE(h_dl.has_pending_retx());
    h_dl.slot_indication(++sl_tx);
  }

  bool prev_ndi = h_dl.tb(0).ndi;
  ASSERT_TRUE(h_dl.ack_info(0, true) > 0);
  set_dl_newtx(h_dl, sl_tx, k1, max_harq_retxs);
  ASSERT_NE(prev_ndi, h_dl.tb(0).ndi);
}

INSTANTIATE_TEST_SUITE_P(dl_harq_param_combine,
                         dl_harq_process_tester,
                         testing::Combine(testing::Values(0, 1, 2, 4),   // max_retxs
                                          testing::Values(1, 2, 4, 6),   // max_ack_wait_slots
                                          testing::Values(1, 2, 4, 6))); // k1
