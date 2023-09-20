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

#pragma once

#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

namespace srsran {
namespace ofh {
namespace testing {

/// User-Plane received symbol notifier spy.
class uplane_rx_symbol_notifier_spy : public uplane_rx_symbol_notifier
{
  bool new_uplink_symbol_function_called = false;
  bool new_prach_function_called         = false;

public:
  // See interface for documentation.
  void on_new_uplink_symbol(const uplane_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    new_uplink_symbol_function_called = true;
  }

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    new_prach_function_called = true;
  }

  /// Returns true if on_new_uplink_symbol function has been called, otherwise false.
  bool has_new_uplink_symbol_function_been_called() const { return new_uplink_symbol_function_called; }

  /// Returns true if on_new_prach_window_data function has been called, otherwise false.
  bool has_new_prach_function_been_called() const { return new_prach_function_called; }
};

} // namespace testing
} // namespace ofh
} // namespace srsran
