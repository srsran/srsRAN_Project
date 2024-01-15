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

#include "lib/sdap/sdap_entity_impl.h"
#include "srsran/sdap/sdap.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <queue>

#pragma once

namespace srsran {

namespace srs_cu_up {

/// Mocking class of the upper layers invoked by the SDAP entity.
class sdap_upper_dummy : public sdap_rx_sdu_notifier
{
public:
  std::queue<byte_buffer> sdu_queue;

  // sdap_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer pdu, qos_flow_id_t qfi) override { sdu_queue.push(std::move(pdu)); };
};

/// Mocking class of the lower layers invoked by the SDAP entity.
class sdap_lower_dummy : public sdap_tx_pdu_notifier
{
public:
  std::queue<byte_buffer> pdu_queue;

  // sdap_tx_pdu_notifier interface
  void on_new_pdu(byte_buffer pdu) override { pdu_queue.push(std::move(pdu)); };
};

/// Fixture class for SDAP tests
class sdap_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init SDAP logger
    srslog::fetch_basic_logger("SDAP", false).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SDAP", false).set_hex_dump_max_size(-1);

    logger.info("Creating SDAP entity.");

    // Create test frames
    ul_sink  = std::make_unique<sdap_upper_dummy>();
    dl_sink1 = std::make_unique<sdap_lower_dummy>();
    dl_sink2 = std::make_unique<sdap_lower_dummy>();

    // Create SDAP TX entity
    ue_inactivity_timer = timers.create_timer();
    ue_inactivity_timer.set(std::chrono::milliseconds(10000), [](timer_id_t) {});
    sdap = std::make_unique<sdap_entity_impl>(7, pdu_session_id_t::min, ue_inactivity_timer, *ul_sink);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&             logger = srslog::fetch_basic_logger("TEST", false);
  manual_task_worker                worker{64};
  timer_manager                     timers_manager;
  timer_factory                     timers{timers_manager, worker};
  unique_timer                      ue_inactivity_timer;
  std::unique_ptr<sdap_upper_dummy> ul_sink;
  std::unique_ptr<sdap_lower_dummy> dl_sink1;
  std::unique_ptr<sdap_lower_dummy> dl_sink2;
  std::unique_ptr<sdap_entity_impl> sdap;
};

} // namespace srs_cu_up

} // namespace srsran
