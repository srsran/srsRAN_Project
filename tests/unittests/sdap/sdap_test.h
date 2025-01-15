/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/sdap/sdap_entity_impl.h"
#include "srsran/sdap/sdap.h"
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
    sdap = std::make_unique<sdap_entity_impl>(7, pdu_session_id_t::min, *ul_sink);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  srslog::basic_logger&             logger = srslog::fetch_basic_logger("TEST", false);
  std::unique_ptr<sdap_upper_dummy> ul_sink;
  std::unique_ptr<sdap_lower_dummy> dl_sink1;
  std::unique_ptr<sdap_lower_dummy> dl_sink2;
  std::unique_ptr<sdap_entity_impl> sdap;
};

} // namespace srs_cu_up

} // namespace srsran
