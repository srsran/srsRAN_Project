/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/upper_phy_rx_symbol_handler_impl.h"
#include "../support/prach_buffer_test_doubles.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/upper/upper_phy_factories.h"
#include "uplink_processor_test_doubles.h"
#include <gtest/gtest.h>

using namespace srsgnb;

namespace {

class UpperPhyRxSymbolHandlerFixture : public ::testing::Test
{
protected:
  std::unique_ptr<rx_softbuffer_pool>    soft_pool;
  uplink_processor_spy*                  ul_proc_spy;
  std::unique_ptr<uplink_processor_pool> ul_processor_pool;
  upper_phy_rx_symbol_handler_impl       rx_handler;
  prach_buffer_context                   context;
  prach_buffer_spy                       buffer_dummy;
  prach_detector::configuration          config;
  uplink_slot_pdu_repository             pdu_repo;

  void handle_prach_symbol() { rx_handler.handle_rx_prach_window(context, buffer_dummy); }

  UpperPhyRxSymbolHandlerFixture() :
    soft_pool(create_rx_softbuffer_pool(rx_softbuffer_pool_description())),
    ul_processor_pool(create_ul_processor_pool()),
    rx_handler(*ul_processor_pool, pdu_repo, *soft_pool, srslog::fetch_basic_logger("TEST"))
  {
    context.sector = 0;
    context.slot   = slot_point(0, 0, 0);
  }

  std::unique_ptr<uplink_processor_pool> create_ul_processor_pool()
  {
    uplink_processor_pool_config config_pool;
    config_pool.num_sectors = 1;
    config_pool.ul_processors.emplace_back();
    uplink_processor_pool_config::sector_ul_processors& info = config_pool.ul_processors.back();
    info.sector                                              = 0;
    info.scs                                                 = subcarrier_spacing::kHz15;
    auto ul_proc                                             = std::make_unique<uplink_processor_spy>();
    ul_proc_spy                                              = &(*ul_proc);
    info.procs.push_back(std::move(ul_proc));

    return create_uplink_processor_pool(std::move(config_pool));
  }
};

TEST_F(UpperPhyRxSymbolHandlerFixture, handling_valid_prach_calls_uplink_processor)
{
  ASSERT_FALSE(ul_proc_spy->has_process_prach_method_called());

  handle_prach_symbol();

  ASSERT_TRUE(ul_proc_spy->has_process_prach_method_called());
}

} // namespace
