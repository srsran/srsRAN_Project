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

#include "../../../../lib/ofh/transmitter/ofh_uplink_request_handler_impl.h"
#include "../../phy/support/resource_grid_test_doubles.h"
#include "ofh_data_flow_cplane_scheduling_commands_test_doubles.h"
#include "srsran/phy/support/prach_buffer.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace srsran::ofh::testing;

static constexpr unsigned eaxc            = 2U;
static constexpr unsigned REPOSITORY_SIZE = 20U;

namespace {

class prach_buffer_dummy : public prach_buffer
{
  std::array<cf_t, 1> buffer;

public:
  unsigned get_max_nof_ports() const override { return 0; }

  unsigned get_max_nof_td_occasions() const override { return 0; }

  unsigned get_max_nof_fd_occasions() const override { return 0; }

  unsigned get_max_nof_symbols() const override { return 0; }

  unsigned get_sequence_length() const override { return 0; }

  span<cf_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) override
  {
    return buffer;
  }

  span<const cf_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override
  {
    return buffer;
  }
};

class ofh_uplink_request_handler_impl_fixture : public ::testing::Test
{
protected:
  uplink_context_repository<ul_slot_context>*  ul_slot_repo;
  uplink_context_repository<ul_prach_context>* ul_prach_repo;
  data_flow_cplane_scheduling_commands_spy*    data_flow;
  uplink_request_handler_impl                  handler;

  explicit ofh_uplink_request_handler_impl_fixture() : handler(get_config()) {}

  uplink_request_handler_impl_config get_config()
  {
    uplink_request_handler_impl_config config;
    config.ul_prach_eaxc = {};
    config.ul_data_eaxc  = eaxc;
    config.ul_slot_repo  = std::make_shared<uplink_context_repository<ul_slot_context>>(REPOSITORY_SIZE);
    ul_slot_repo         = config.ul_slot_repo.get();
    config.ul_prach_repo = std::make_shared<uplink_context_repository<ul_prach_context>>(REPOSITORY_SIZE);
    ul_prach_repo        = config.ul_prach_repo.get();
    auto temp            = std::make_unique<data_flow_cplane_scheduling_commands_spy>();
    data_flow            = temp.get();
    config.data_flow     = std::move(temp);

    return config;
  }
};

} // namespace

TEST_F(ofh_uplink_request_handler_impl_fixture,
       handle_prach_request_when_cplane_message_is_disable_for_prach_does_not_generate_cplane_message)
{
  prach_buffer_context context;
  context.slot = slot_point(1, 20, 1);
  prach_buffer_dummy buffer_dummy;

  handler.handle_prach_occasion(context, buffer_dummy);

  // Assert data flow.
  ASSERT_FALSE(data_flow->has_enqueue_section_type_1_method_been_called());

  // Assert repository.
  const ul_prach_context& prach_ctx = ul_prach_repo->get(context.slot);
  ASSERT_EQ(prach_ctx.buffer, &buffer_dummy);
}

TEST_F(ofh_uplink_request_handler_impl_fixture, handle_uplink_slot_generates_cplane_message)
{
  resource_grid_dummy   rg;
  resource_grid_context rg_context;
  rg_context.slot   = slot_point(1, 1, 1);
  rg_context.sector = 1;

  handler.handle_new_uplink_slot(rg_context, rg);

  // Assert data flow.
  ASSERT_TRUE(data_flow->has_enqueue_section_type_1_method_been_called());
  const data_flow_cplane_scheduling_commands_spy::spy_info& info = data_flow->get_spy_info();
  ASSERT_EQ(rg_context.slot, info.slot);
  ASSERT_EQ(eaxc, info.eaxc);
  ASSERT_EQ(data_direction::uplink, info.direction);

  // Assert repository.
  const ul_slot_context& slot_ctx = ul_slot_repo->get(rg_context.slot);
  ASSERT_EQ(slot_ctx.grid, &rg);
}
