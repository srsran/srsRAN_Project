/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_dl/mac_dl_ue_manager.h"
#include "mac_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_dl_ue_manager_tester : public ::testing::Test
{
protected:
  static constexpr unsigned MAX_KOS = 100;

  mac_dl_ue_manager_tester() : ue_mng(mac_expert_config{MAX_KOS, MAX_KOS}, rnti_table)
  {
    srslog::init();
    srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  }
  ~mac_dl_ue_manager_tester() { srslog::flush(); }

  du_rnti_table     rnti_table;
  mac_dl_ue_manager ue_mng;

  test_helpers::dummy_ue_rlf_notifier rlf_notifier;

  void add_ue(rnti_t rnti, du_ue_index_t ue_index)
  {
    this->rnti_table.add_ue(rnti, ue_index);

    mac_ue_create_request_message req = test_helpers::make_default_ue_creation_request();
    req.ue_index                      = ue_index;
    req.crnti                         = rnti;
    req.rlf_notifier                  = &rlf_notifier;
    this->ue_mng.add_ue(req, {});
  }
};

TEST_F(mac_dl_ue_manager_tester, when_consecutive_harq_kos_reaches_limit_then_rlf_is_triggered)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  add_ue(to_rnti(0x4601), ue_index);

  unsigned nof_oks = test_rgen::uniform_int<unsigned>(0, 10);
  for (unsigned i = 0; i != nof_oks; ++i) {
    this->ue_mng.report_ack(ue_index, true);
  }

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    ASSERT_FALSE(this->rlf_notifier.rlf_detected);
    this->ue_mng.report_ack(ue_index, false);
  }
  ASSERT_TRUE(this->rlf_notifier.rlf_detected);
}

TEST_F(mac_dl_ue_manager_tester, when_harq_kos_limit_not_reached_then_rlf_is_not_triggered)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  add_ue(to_rnti(0x4601), ue_index);

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    this->ue_mng.report_ack(ue_index, i == 0);
  }
  ASSERT_FALSE(this->rlf_notifier.rlf_detected);
}
