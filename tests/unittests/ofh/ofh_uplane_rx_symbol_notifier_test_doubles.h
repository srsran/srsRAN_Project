/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/phy/support/shared_resource_grid.h"

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
  void on_new_uplink_symbol(const uplane_rx_symbol_context& context, shared_resource_grid grid) override
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
