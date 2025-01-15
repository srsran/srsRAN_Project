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

#include "srsran/ran/slot_point.h"

namespace srsran {
class prach_buffer;
struct prach_buffer_context;
class shared_resource_grid;

namespace ofh {

/// User-Plane reception symbol context.
struct uplane_rx_symbol_context {
  /// Slot point.
  slot_point slot;
  /// Symbol identifier.
  unsigned symbol;
  /// Sector.
  unsigned sector;
};

/// Open Fronthaul User-Plane symbol reception notifier.
class uplane_rx_symbol_notifier
{
public:
  virtual ~uplane_rx_symbol_notifier() = default;

  /// \brief Notifies the completion of an OFDM symbol for a given context.
  ///
  /// \param[in] context Notification context.
  /// \param[in] grid    Resource grid that belongs to the context.
  virtual void on_new_uplink_symbol(const uplane_rx_symbol_context& context, shared_resource_grid grid) = 0;

  /// \brief Notifies the completion of a PRACH window.
  /// \param[in] context PRACH context.
  /// \param[in] buffer  Read-only PRACH buffer.
  virtual void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) = 0;
};

} // namespace ofh
} // namespace srsran
