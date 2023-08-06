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

#include "lib/mac/mac_dl/mac_dl_ue_manager.h"
#include "mac_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_dl_ue_manager_tester : public ::testing::Test
{
protected:
  static constexpr unsigned MAX_KOS = 100;

  mac_dl_ue_manager_tester() : ue_mng(rnti_table, rlf_handler)
  {
    srslog::init();
    srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  }
  ~mac_dl_ue_manager_tester() { srslog::flush(); }

  du_rnti_table     rnti_table;
  rlf_detector      rlf_handler{MAX_KOS, MAX_KOS};
  mac_dl_ue_manager ue_mng;

  test_helpers::dummy_ue_rlf_notifier rlf_notifier;

  void add_ue(rnti_t rnti, du_ue_index_t ue_index)
  {
    this->rnti_table.add_ue(rnti, ue_index);

    mac_ue_create_request req = test_helpers::make_default_ue_creation_request();
    req.ue_index              = ue_index;
    req.crnti                 = rnti;
    req.rlf_notifier          = &rlf_notifier;
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
