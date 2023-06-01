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

#include "../../../../lib/ofh/transmitter/ofh_data_flow_uplane_downlink_data.h"
#include "../../../../lib/ofh/transmitter/ofh_downlink_handler_impl.h"
#include "../../phy/support/resource_grid_test_doubles.h"
#include "ofh_data_flow_cplane_scheduling_commands_test_doubles.h"
#include "srsran/phy/support/resource_grid_context.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;
using namespace srsran::ofh::testing;

namespace {

/// Spy User-Plane downlink data data flow.
class data_flow_uplane_downlink_data_spy : public data_flow_uplane_downlink_data
{
  bool     has_enqueue_section_type_1_message_method_been_called = false;
  unsigned eaxc                                                  = -1;

public:
  // See interface for documentation.
  void enqueue_section_type_1_message(const resource_grid_context& context,
                                      const resource_grid_reader&  grid,
                                      unsigned                     eaxc_) override
  {
    has_enqueue_section_type_1_message_method_been_called = true;
    eaxc                                                  = eaxc_;
  }

  /// Returns true if the method enqueue section type 1 message has been called, otherwise false.
  bool has_enqueue_section_type_1_method_been_called() const
  {
    return has_enqueue_section_type_1_message_method_been_called;
  }

  /// Returns the configured eAxC.
  unsigned get_eaxc() const { return eaxc; }
};

} // namespace

TEST(ofh_downlink_handler_impl, handling_downlink_data_use_control_and_user_plane)
{
  unsigned    eaxc           = 24U;
  auto        cplane_spy_ptr = std::make_unique<data_flow_cplane_scheduling_commands_spy>();
  const auto& cplane_spy     = *cplane_spy_ptr;
  auto        uplane_spy_ptr = std::make_unique<data_flow_uplane_downlink_data_spy>();
  const auto& uplane_spy     = *uplane_spy_ptr;

  downlink_handler_impl handler(eaxc, std::move(cplane_spy_ptr), std::move(uplane_spy_ptr));

  resource_grid_dummy   rg;
  resource_grid_context rg_context;
  rg_context.slot   = slot_point(1, 1, 1);
  rg_context.sector = 1;

  handler.handle_dl_data(rg_context, rg);

  // Assert Control-Plane.
  ASSERT_TRUE(cplane_spy.has_enqueue_section_type_1_method_been_called());
  const data_flow_cplane_scheduling_commands_spy::spy_info& info = cplane_spy.get_spy_info();
  ASSERT_EQ(rg_context.slot, info.slot);
  ASSERT_EQ(eaxc, info.eaxc);
  ASSERT_EQ(data_direction::downlink, info.direction);
  ASSERT_EQ(filter_index_type::standard_channel_filter, info.filter_type);

  // Assert User-Plane.
  ASSERT_TRUE(uplane_spy.has_enqueue_section_type_1_method_been_called());
  ASSERT_EQ(eaxc, uplane_spy.get_eaxc());
}
