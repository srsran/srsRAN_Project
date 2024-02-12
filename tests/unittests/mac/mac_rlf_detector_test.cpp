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

#include "lib/mac/mac_sched/rlf_detector.h"
#include "mac_test_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_rlf_detector_test : public ::testing::Test
{
protected:
  static constexpr unsigned MAX_KOS = 100;

  mac_rlf_detector_test() : rlf_handler(mac_expert_config{.configs = {{MAX_KOS, MAX_KOS, MAX_KOS}}})
  {
    srslog::init();
    srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  }
  ~mac_rlf_detector_test() { srslog::flush(); }

  test_helpers::dummy_ue_rlf_notifier rlf_notif;
  rlf_detector                        rlf_handler;
};

TEST_F(mac_rlf_detector_test, when_consecutive_harq_kos_reaches_limit_then_rlf_is_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  unsigned nof_oks = test_rgen::uniform_int<unsigned>(0, MAX_KOS + 1);
  for (unsigned i = 0; i != nof_oks; ++i) {
    rlf_handler.handle_ack(ue_index, cell_index, true);
  }

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    ASSERT_FALSE(rlf_notif.rlf_detected);
    rlf_handler.handle_ack(ue_index, cell_index, false);
  }
  ASSERT_TRUE(rlf_notif.rlf_detected);

  // Check that the RLF is not triggered again, if more KOs are detected.
  rlf_notif.rlf_detected = false;
  for (unsigned i = 0; i != MAX_KOS; ++i) {
    rlf_handler.handle_ack(ue_index, cell_index, false);
  }
  ASSERT_FALSE(rlf_notif.rlf_detected);
}

TEST_F(mac_rlf_detector_test, when_consecutive_harq_kos_limit_not_reached_then_rlf_is_not_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  for (unsigned i = 0; i != MAX_KOS - 1; ++i) {
    rlf_handler.handle_ack(ue_index, cell_index, false);
  }
  rlf_handler.handle_ack(ue_index, cell_index, true);
  rlf_handler.handle_ack(ue_index, cell_index, false);
  ASSERT_FALSE(rlf_notif.rlf_detected);
}

TEST_F(mac_rlf_detector_test, when_consecutive_undetected_csi_reaches_limit_then_rlf_is_triggered)
{
  const du_ue_index_t   ue_index   = to_du_ue_index(0);
  const du_cell_index_t cell_index = to_du_cell_index(0);
  rlf_handler.add_ue(ue_index, rlf_notif);

  unsigned nof_oks = test_rgen::uniform_int<unsigned>(0, MAX_KOS + 1);
  for (unsigned i = 0; i != nof_oks; ++i) {
    rlf_handler.handle_csi(ue_index, cell_index, true);
  }
  ASSERT_FALSE(rlf_notif.rlf_detected);

  for (unsigned i = 0; i != MAX_KOS; ++i) {
    ASSERT_FALSE(rlf_notif.rlf_detected);
    rlf_handler.handle_csi(ue_index, cell_index, false);
  }
  ASSERT_TRUE(rlf_notif.rlf_detected);

  // Check that the RLF is not triggered again, if any more undetected CSIs.
  rlf_notif.rlf_detected = false;
  for (unsigned i = 0; i != MAX_KOS; ++i) {
    rlf_handler.handle_csi(ue_index, cell_index, false);
  }
  ASSERT_FALSE(rlf_notif.rlf_detected);
}
